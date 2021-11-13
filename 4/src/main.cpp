#include <QApplication>
#include <QDebug>
#include <QListWidget>
#include <QRandomGenerator>
#include <QList>

#include <windows.h>

#include <functional>

#include "MainWindow.h" 

#define CHUNKSIZE 30

#define MUTEXNAME "mutex"
#define EMPTYNAME "empty"
#define FULLNAME  "full"
#define CONSUMER  "consumer"
#define PRODUCER  "producer"
#define MAPNAME   "map"
#define UPDATE_BUFFER_GUI_SIGNAL "ubgs"

QApplication* a;
MainWindow* w;
HANDLE updateBufferGuiSignal;
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

QList<int> deque_bytes(char* rawdata, size_t size_) {
    QList<int> d;
    auto bytes = QByteArray::fromRawData(rawdata, size_);
    QDataStream stream(&bytes, QIODevice::ReadOnly);
    stream >> d;
    return d;
}

QList<int> deque_bytes_(char* rawdata) {
    return deque_bytes(rawdata + 1, rawdata[0]);
}

QList<int> deque_file(char* filename) {
    QList<int> d;
    doWithMap(filename, [&](void* buf) {
        char* buf_ = reinterpret_cast<char*>(buf);
        d = deque_bytes(buf_ + 1, buf_[0]);
    });
    return d;
}

std::pair<char*, size_t> bytes_deque(QList<int> d) {
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream << d;
    
    char* res = new char[bytes.length()];
    memcpy(res, bytes.constData(), bytes.length());

    return {res, bytes.length()};
}

char* bytes_deque_(QList<int> d) {
    auto b_sz = bytes_deque(d);
    auto b_ = new char[b_sz.second + 1];
    b_[0] = b_sz.second;
    memcpy(b_ + 1, b_sz.first, b_sz.second);
    return b_;
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
    Sleep(50);
}

void upd_buffer(char* filename, std::function<void(QList<int>&)> f) {
    auto updateBufferGuiSignal_ = OpenEventA(EVENT_ALL_ACCESS, false, UPDATE_BUFFER_GUI_SIGNAL);
    doWithMap(filename, [&](void* buf) {
            char* buf_ = reinterpret_cast<char*>(buf);
            auto d = deque_bytes_(buf_);
            f(d);
            auto b = bytes_deque_(d);
            CopyMemory(buf, b, b[0] + 1);
    });
    SetEvent(updateBufferGuiSignal_);
}

void producer(char* filename, HANDLE mutex, HANDLE empty, HANDLE full) {
    while (true) {
        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        upd_buffer(filename, [](QList<int>& b) {
            b.push_back(QRandomGenerator::global()->generate());
        });
        wait();

        ReleaseSemaphore(mutex, 1, NULL);
        ReleaseSemaphore(full,  1, NULL);
    }
}

void consumer(char* filename, HANDLE mutex, HANDLE empty, HANDLE full) {
    while (true) {
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        upd_buffer(filename, [](QList<int>& b) {b.pop_front();});
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

DWORD __stdcall gui_updater(void*) {
    auto mutex = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, false, MUTEXNAME);
    while (true) {
        WaitForSingleObject(updateBufferGuiSignal, INFINITE);
            WaitForSingleObject(mutex, INFINITE);
                updateBufferList(gFilename);
            ReleaseSemaphore(mutex, 1, NULL);
    }
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
    if (argc == 2) {// gui
        argc_ = argc; argv_ = argv;
        a = new QApplication(argc_, argv_);
        w = new MainWindow();// it wouldn't show otherwise idk why

        auto filename = argv[1];
        gFilename = filename;

        auto cl_consumer = cli(argv[0], filename, CONSUMER);
        auto cl_producer = cli(argv[0], filename, PRODUCER);

        auto mutex = CreateSemaphoreA(NULL, 1,         1,         MUTEXNAME);
        auto empty = CreateSemaphoreA(NULL, CHUNKSIZE, CHUNKSIZE, EMPTYNAME);
        auto full  = CreateSemaphoreA(NULL, 0,         CHUNKSIZE, FULLNAME);

        auto hGui = CreateThread(NULL, 1024, gui, NULL, 0, NULL);

        doWithMap(filename, [](void* buf) {// init file with zeroes
            char zeroes[CHUNKSIZE * sizeof(int) + 1] = {'\0'};
            CopyMemory(buf, zeroes, CHUNKSIZE * sizeof(int) + 1);
        });

        updateBufferGuiSignal = CreateEventA(
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
        }
    } else {
    }
}