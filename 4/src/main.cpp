#include <QApplication>
#include <QDebug>
#include <QListWidget>
#include <QRandomGenerator>

#include <windows.h>

#include <deque>
#include <functional>

#include "MainWindow.h" 

#define CHUNKSIZE 30

#define MUTEXNAME "mutex"
#define EMPTYNAME "empty"
#define FULLNAME  "full"
#define CONSUMER  "consumer"
#define PRODUCER  "producer"
#define MAPNAME   "map"

QApplication* a;
MainWindow* w;

std::deque<int> deque_file(char* filename) {

}

std::deque<int> deque_bytes(char* bytes) {

}

char* bytes_deque(std::deque<int> d) {

}

void updateBufferList(char* filename) {
    QListWidget* bufferListWidget = w->findChild<QWidget*>("centralwidget")
                                     ->findChild<QListWidget*>("bufferListWidget");
    bufferListWidget->clear();
    auto buffer = deque_file(filename);
    for (auto i : buffer) {
        bufferListWidget->addItem(QString::number(i));
    }
}

void wait() {
    Sleep(1000 / w->tickrate);
}

void doWithMap(char* filename, std::function<void(void*)> f) {
    auto file = CreateFileA(
        filename,
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    auto mapFile = CreateFileMappingA(
        file,
        NULL,                   // default security
        PAGE_READWRITE,         // read/write access
        0,                      // maximum object size (high-order DWORD)
        CHUNKSIZE * sizeof(int),// maximum object size (low-order DWORD)
        MAPNAME                 // name of mapping object
    );

    auto buf = MapViewOfFile(
        mapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        CHUNKSIZE * sizeof(int)
    );

    f(buf);

    UnmapViewOfFile(buf);
    CloseHandle(mapFile);
    CloseHandle(file);
}

void producer(char* filename, HANDLE mutex, HANDLE empty, HANDLE full) {
    while (true) {
        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        doWithMap(filename, [](void* buf) {
            auto d = deque_bytes(reinterpret_cast<char*>(buf));
            d.push_back(QRandomGenerator::global()->generate());
            auto b = bytes_deque(d);
            CopyMemory(buf, b, CHUNKSIZE);
        });
        updateBufferList(filename);
        wait();

        ReleaseSemaphore(mutex, 1, NULL);
        ReleaseSemaphore(full,  1, NULL);
    }
}

void consumer(char* filename, HANDLE mutex, HANDLE empty, HANDLE full) {
    while (true) {
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        doWithMap(filename, [](void* buf) {
            auto d = deque_bytes(reinterpret_cast<char*>(buf));
            d.pop_front();
            auto b = bytes_deque(d);
            CopyMemory(buf, b, CHUNKSIZE);
        });
        updateBufferList(filename);
        wait();

        ReleaseSemaphore(mutex, 1, NULL);
        ReleaseSemaphore(empty, 1, NULL);
    }
}

int argc_; char** argv_;

DWORD __stdcall gui(void*) {
    a = new QApplication(argc_, argv_);
    w = new MainWindow();
    w->show();
    return a->exec();
}

char* cli(std::string execname, std::string filename, std::string process_type) {
    return strdup((execname
        + " " + filename
        + " " + process_type
        + " " + MUTEXNAME
        + " " + EMPTYNAME
        + " " + FULLNAME).c_str());
}

PROCESS_INFORMATION CreateProcess_(char* cl) {
    auto si = STARTUPINFOA();
    PROCESS_INFORMATION pi;
    CreateProcessA(
        /*lpApplicationName   */ NULL,
        /*lpCommandLine       */ strdup(cl),
        /*lpProcessAttributes */ NULL,
        /*lpThreadAttributes  */ NULL,
        /*bInheritHandles     */ false,
        /*dwCreationFlags     */ NULL,
        /*lpEnvironment       */ NULL,
        /*lpCurrentDirectory  */ NULL,
        /*lpStartupInfo       */ &si,
        /*lpProcessInformation*/ &pi
    );
    return pi;
}

int main(int argc, char* argv[]) {
    /**
     * argv[1] - filename
     * argv[2] - process type: "consumer" | "producer"
     * argv[3] - mutex_semaphore name
     * argv[4] - empty_semaphore name
     * argv[5] = full_semaphore name
     * 
     * if no arguments 2-5 provided then process type infered as gui
     */
    if (argc == 1) {// gui
        argc_ = argc; argv_ = argv;
        a = new QApplication(argc_, argv_);
        w = new MainWindow();// it wouldn't show otherwise idk why

        auto filename = argv[1];

        auto cl_consumer = cli(argv[0], filename, CONSUMER);
        auto cl_producer = cli(argv[0], filename, PRODUCER);

        auto mutex = CreateSemaphoreA(NULL, 1,         1,         MUTEXNAME);
        auto empty = CreateSemaphoreA(NULL, CHUNKSIZE, CHUNKSIZE, EMPTYNAME);
        auto full  = CreateSemaphoreA(NULL, 0,         CHUNKSIZE, FULLNAME);

        auto hGui = CreateThread(NULL, 1024, gui,      NULL, 0, NULL);

        std::vector<PROCESS_INFORMATION> processes;
        for (size_t i = 0; i < 2; i++) {
            processes.push_back(CreateProcess_(cl_producer));
        }
        processes.push_back(CreateProcess_(cl_consumer));

        WaitForSingleObject(hGui, INFINITE);
        for (auto i : processes) {
            TerminateProcess(i.hProcess, 0);
        }
    } else if (argc == 6) {// consumer or producer
        auto filename     = argv[1];
        auto process_type = argv[2];
        auto mutex_name   = argv[3];
        auto empty_name   = argv[4];
        auto full_name    = argv[5];

        auto mutex = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, mutex_name);
        auto empty = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, empty_name);
        auto full  = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, full_name);

        if (std::string(process_type) == "consumer") {
            consumer(filename, mutex, empty, full);
        } else if (std::string(process_type) == "producer") {
            producer(filename, mutex, empty, full);
        } else {
            qDebug() << "wrong process type";
        }
    } else {
        qDebug() << "wrong process type";
    }
}