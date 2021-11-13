#include <QApplication>
#include <QDebug>
#include <QListWidget>
#include <QRandomGenerator>
#include <QList>

#include <windows.h>

#include <functional>

#include "MainWindow.h"
#include "marshalling.h"
#include "util.h"

#define CHUNKSIZE 30
#define SLEEPTIME 50

#define MUTEXNAME "mutex"
#define EMPTYNAME "empty"
#define FULLNAME  "full"
#define CONSUMER  "consumer"
#define PRODUCER  "producer"
#define MAPNAME   "map"
#define UPDATE_BUFFER_GUI_SIGNAL "ubgs"

QApplication* a;
MainWindow* w;
char* gFilename;

void doWithMap(char* filename, std::function<void(void*)> f) {
    auto file = CreateFileA(
        filename,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    auto mapFile = CreateFileMappingA(
        file,
        NULL,                   // default security
        PAGE_READWRITE,         // read/write access
        0,                      // maximum object size (high-order DWORD)
        CHUNKSIZE * sizeof(int) + 1,// maximum object size (low-order DWORD)
        MAPNAME                 // name of mapping object
    );

    auto buf = MapViewOfFile(
        mapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        CHUNKSIZE * sizeof(int) + 1
    );

    f(buf);

    UnmapViewOfFile(buf);
    CloseHandle(mapFile);
    CloseHandle(file);
}

QList<int> deque_file(char* filename) {
    QList<int> d;
    doWithMap(filename, [&](void* buf) {
        char* buf_ = reinterpret_cast<char*>(buf);
        d = deque_bytes_(buf_);
    });
    return d;
}

void updateBufferList(char* filename) {
    QListWidget* bufferListWidget = w->findChild<QWidget*>("centralwidget")
                                     ->findChild<QListWidget*>("bufferListWidget");
    bufferListWidget->clear();
    auto buffer = deque_file(filename);
    for (auto i : buffer) {
        bufferListWidget->addItem(QString::number(i));
        if (i == 0) {
            qDebug() << "alarm";
        }
    }
}

void upd_buffer(char* filename, std::function<void(QList<int>&)> f) {
    auto updateBufferGuiSignal = OpenEventA(EVENT_ALL_ACCESS, false, UPDATE_BUFFER_GUI_SIGNAL);
    doWithMap(filename, [&](void* buf) {
            char* buf_ = reinterpret_cast<char*>(buf);
            auto d = deque_bytes_(buf_);
            f(d);
            auto b = bytes_deque_(d);
            CopyMemory(buf_, b, b[0] + 1);
    });
    SetEvent(updateBufferGuiSignal);
}

void producer(char* filename, HANDLE mutex, HANDLE empty, HANDLE full) {
    while (true) {
        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        upd_buffer(filename, [](QList<int>& b) {
            b.push_back(QRandomGenerator::global()->generate());
        });
        Sleep(SLEEPTIME);

        ReleaseSemaphore(mutex, 1, NULL);
        ReleaseSemaphore(full,  1, NULL);
    }
}

void consumer(char* filename, HANDLE mutex, HANDLE empty, HANDLE full) {
    while (true) {
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        upd_buffer(filename, [](QList<int>& b) {b.pop_front();});
        Sleep(SLEEPTIME);

        ReleaseSemaphore(mutex, 1, NULL);
        ReleaseSemaphore(empty, 1, NULL);
    }
}

DWORD __stdcall gui(void* arg) {
    auto args = reinterpret_cast<std::pair<int, char**>*>(arg);
    a = new QApplication(args->first, args->second);
    w = new MainWindow();
    w->show();
    return a->exec();
}

DWORD __stdcall gui_updater(void*) {
    auto mutex = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, MUTEXNAME);
    auto updateBufferGuiSignal = OpenEventA(EVENT_ALL_ACCESS, false, UPDATE_BUFFER_GUI_SIGNAL);
    while (true) {
        WaitForSingleObject(updateBufferGuiSignal, INFINITE);
            WaitForSingleObject(mutex, INFINITE);
                updateBufferList(gFilename);
            ReleaseSemaphore(mutex, 1, NULL);
    }
}

int main(int argc, char* argv[]) {
    /**
     * argv[1] - filename
     * argv[2] - process type: "consumer" | "producer"
     * 
     * if no argument 2 is provided then process type infered as gui
     */
    if (argc == 2) {// gui
        auto args = std::make_pair(argc, argv);
        auto filename = argv[1];
        gFilename = filename;

        auto cl_consumer = cli(argv[0], filename, CONSUMER);
        auto cl_producer = cli(argv[0], filename, PRODUCER);

        auto mutex = CreateSemaphoreA(NULL, 1,         1,         MUTEXNAME);
        auto empty = CreateSemaphoreA(NULL, CHUNKSIZE, CHUNKSIZE, EMPTYNAME);
        auto full  = CreateSemaphoreA(NULL, 0,         CHUNKSIZE, FULLNAME);

        auto hGui = CreateThread(NULL, 1024, gui, &args, 0, NULL);

        doWithMap(filename, [](void* buf) {// init file with zeroes
            char zeroes[CHUNKSIZE * sizeof(int) + 1] = {'\0'};
            CopyMemory(buf, zeroes, CHUNKSIZE * sizeof(int) + 1);
        });

        auto updateBufferGuiSignal = CreateEventA(
            NULL,
            false,// bManualReset
            false,// bInitialState
            UPDATE_BUFFER_GUI_SIGNAL
        );

        auto guiUpdater = CreateThread(NULL, 1024, gui_updater, NULL, 0, NULL);

        std::vector<PROCESS_INFORMATION> processes;
        for (size_t i = 0; i < 2; i++) {
            processes.push_back(CreateProcess_(cl_producer));
        }
        processes.push_back(CreateProcess_(cl_consumer));

        WaitForSingleObject(hGui, INFINITE);
        for (auto i : processes) {
            TerminateProcess(i.hProcess, 0);
        }
    } else if (argc == 3) {// consumer or producer
        auto filename     = argv[1];
        auto process_type = argv[2];

        auto mutex = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, MUTEXNAME);
        auto empty = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, EMPTYNAME);
        auto full  = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, FULLNAME);
        if (std::string(process_type) == "consumer") {
            consumer(filename, mutex, empty, full);
        } else if (std::string(process_type) == "producer") {
            producer(filename, mutex, empty, full);
        } else {
        }
    } else {
    }
}