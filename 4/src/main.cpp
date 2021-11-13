#include <QApplication>
// #include <QDebug>
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

QApplication* a;
MainWindow* w;

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

QList<int> deque_bytes(char rawdata[CHUNKSIZE * sizeof(int)]) {
    QList<int> d;
    auto bytes = QByteArray::fromRawData(rawdata, CHUNKSIZE * sizeof(int));
    QDataStream stream(&bytes, QIODevice::ReadOnly);
    stream >> d;
    return d;
}

QList<int> deque_file(char* filename) {
    QList<int> d;
    doWithMap(filename, [&](void* buf) {
        char* rawdata = new char[CHUNKSIZE * sizeof(int)];
        CopyMemory(rawdata, buf, CHUNKSIZE * sizeof(int));
        d = deque_bytes(rawdata);
    });
    return d;
}

char* bytes_deque(QList<int> d) {
    QByteArray bytes;
    QDataStream stream(&bytes, QIODevice::WriteOnly);
    stream << d;
    
    char* res = new char[CHUNKSIZE * sizeof(int)]{'\0'};
    memcpy(res, bytes.constData(), bytes.length() > 30 ? 30 
                                                       : bytes.length());

    return res;
}

void updateBufferList(char* filename) {
    // qDebug() << "ubl1";
    QListWidget* bufferListWidget = w->findChild<QWidget*>("centralwidget")
                                     ->findChild<QListWidget*>("bufferListWidget");
    // qDebug() << "ubl2";
    if (bufferListWidget == NULL) {
        // qDebug() << "bufferListWidget is NULL";
    }
    bufferListWidget->clear();
    // qDebug() << "ubl3";
    auto buffer = deque_file(filename);
    // qDebug() << "ubl4";
    for (auto i : buffer) {
        bufferListWidget->addItem(QString::number(i));
    }
    // qDebug() << "ubl5";
}

void wait() {
    Sleep(1000 / w->tickrate);
}

void producer(char* filename, HANDLE mutex, HANDLE empty, HANDLE full) {
    // qDebug() << "producer started";
    while (true) {
        // qDebug() << "producer loop";
        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        // qDebug() << "producer1";
        doWithMap(filename, [](void* buf) {
            // qDebug() << "producer2";
            auto d = deque_bytes(reinterpret_cast<char*>(buf));
            // qDebug() << "producer3";
            d.push_back(QRandomGenerator::global()->generate());
            // qDebug() << "producer4";
            auto b = bytes_deque(d);
            // qDebug() << "producer5";
            CopyMemory(buf, b, CHUNKSIZE * sizeof(int));
            // qDebug() << "producer6";
        });

        // qDebug() << "producer7";
        updateBufferList(filename);// replace with event/signal
        // qDebug() << "producer8";
        wait();

        ReleaseSemaphore(mutex, 1, NULL);
        ReleaseSemaphore(full,  1, NULL);
    }
}

void consumer(char* filename, HANDLE mutex, HANDLE empty, HANDLE full) {
    // qDebug() << "consumer started";
    while (true) {
        // qDebug() << "consumer loop";
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        // qDebug() << "consumer1";
        doWithMap(filename, [](void* buf) {
            // qDebug() << "consumer2";
            auto d = deque_bytes(reinterpret_cast<char*>(buf));
            // qDebug() << "consumer3";
            d.pop_front();
            // qDebug() << "consumer4";
            auto b = bytes_deque(d);
            // qDebug() << "consumer5";
            CopyMemory(buf, b, CHUNKSIZE);
            // qDebug() << "consumer6";
        });
        updateBufferList(filename);// replace with event/signal
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
    if (argc == 2) {// gui
        argc_ = argc; argv_ = argv;
        a = new QApplication(argc_, argv_);
        w = new MainWindow();// it wouldn't show otherwise idk why

        auto filename = argv[1];

        auto cl_consumer = cli(argv[0], filename, CONSUMER);
        auto cl_producer = cli(argv[0], filename, PRODUCER);

        auto mutex = CreateSemaphoreA(NULL, 1,         1,         MUTEXNAME);
        auto empty = CreateSemaphoreA(NULL, CHUNKSIZE, CHUNKSIZE, EMPTYNAME);
        auto full  = CreateSemaphoreA(NULL, 0,         CHUNKSIZE, FULLNAME);

        auto hGui = CreateThread(NULL, 1024, gui, NULL, 0, NULL);

        doWithMap(filename, [](void* buf) {// init file with zeroes
            char zeroes[CHUNKSIZE * sizeof(int)] = {'\0'};
            CopyMemory(buf, zeroes, CHUNKSIZE * sizeof(int));
        });



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
        // qDebug() << "main1";
        if (std::string(process_type) == "consumer") {
            // qDebug() << "main_consumer";
            consumer(filename, mutex, empty, full);
        } else if (std::string(process_type) == "producer") {
            // qDebug() << "main_producer";
            producer(filename, mutex, empty, full);
        } else {
            // qDebug() << "wrong process type";
        }
    } else {
        // qDebug() << "wrong process type";
    }
}