// #include <QDebug>
#include <QRandomGenerator>
#include <QList>
#include <QDataStream>

#include <windows.h>

#include <functional>
#include <iostream>
#include <string>

#define BUFFERSIZE 30
#define SLEEPTIME 50

#define MUTEXNAME "mutex"
#define EMPTYNAME "empty"
#define FULLNAME  "full"
#define CONSUMER  "consumer"
#define PRODUCER  "producer"
#define MAPNAME   "map"
#define PRINT_BUFFER_SIGNAL "ubgs"

char* gFilename;

char* cli(std::string execname, std::string filename, std::string process_type) {
    return strdup((execname
        + " " + filename
        + " " + process_type).c_str());
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

QList<int> deque_bytes(char* rawdata, size_t size_) {
    QList<int> d;
    auto bytes = QByteArray::fromRawData(rawdata, size_);
    QDataStream stream(&bytes, QIODevice::ReadOnly);
    stream >> d;
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

// Versions with underscore are the same,
// but assume that first byte is size of buffer

QList<int> deque_bytes_(char* rawdata) {
    return deque_bytes(rawdata + 1, rawdata[0]);
}

char* bytes_deque_(QList<int> d) {
    auto b_sz = bytes_deque(d);
    auto b_ = new char[b_sz.second + 1];
    b_[0] = b_sz.second;
    memcpy(b_ + 1, b_sz.first, b_sz.second);
    return b_;
}

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
        NULL,                        // default security
        PAGE_READWRITE,              // read/write access
        0,                           // maximum object size (high-order DWORD)
        BUFFERSIZE * sizeof(int) + 1,// maximum object size (low-order DWORD)
        MAPNAME                      // name of mapping object
    );

    auto buf = MapViewOfFile(
        mapFile,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        BUFFERSIZE * sizeof(int) + 1
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

void upd_buffer(char* filename, std::function<void(QList<int>&)> f) {
    auto printBufferSignal = OpenEventA(EVENT_ALL_ACCESS, false, PRINT_BUFFER_SIGNAL);
    doWithMap(filename, [&](void* buf) {
        char* buf_ = reinterpret_cast<char*>(buf);
        auto d = deque_bytes_(buf_);
        f(d);
        auto b = bytes_deque_(d);
        CopyMemory(buf_, b, b[0] + 1);
    });
    SetEvent(printBufferSignal);
    std::cout << "event is true";
}

void producer(char* filename, HANDLE mutex, HANDLE empty, HANDLE full) {
    while (true) {
        std::cout << "producer loop\n";
        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);
        std::cout << "producer loop after semaphores\n";

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
        std::cout << "consumer loop\n";
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);
        std::cout << "consumer loop after semaphores\n";

        upd_buffer(filename, [](QList<int>& b) {b.pop_front();});
        Sleep(SLEEPTIME);

        ReleaseSemaphore(mutex, 1, NULL);
        ReleaseSemaphore(empty, 1, NULL);
    }
}

void cli_runner(HANDLE mutex, HANDLE printBufferSignal) {
    std::cout << "cli\n";
    while (true) {
        std::cout << "\tcli loop\n";
        WaitForSingleObject(printBufferSignal, INFINITE);
            std::cout << "\tcli loop signal\n";
            WaitForSingleObject(mutex, INFINITE);
                for (auto i : deque_file(gFilename)) {
                    std::cout << i << " ";
                }
                std::cout << "\n";
                // std::cout << deque_file(gFilename);
            ReleaseSemaphore(mutex, 1, NULL);
    }
}

int main(int argc, char* argv[]) {
    /**
     * argv[1] - filename
     * argv[2] - process type: "consumer" | "producer"
     * 
     * if no argument 2 is provided then process type infered as cli
     */
    if (argc == 2) {// cli
        auto args = std::make_pair(argc, argv);
        auto filename = argv[1];
        gFilename = filename;

        auto cl_consumer = cli(argv[0], filename, CONSUMER);
        auto cl_producer = cli(argv[0], filename, PRODUCER);

        auto mutex = CreateSemaphoreA(NULL, 1,          1,          MUTEXNAME);
        auto empty = CreateSemaphoreA(NULL, BUFFERSIZE, BUFFERSIZE, EMPTYNAME);
        auto full  = CreateSemaphoreA(NULL, 0,          BUFFERSIZE, FULLNAME);

        doWithMap(filename, [](void* buf) {// init file with zeroes
            char zeroes[BUFFERSIZE * sizeof(int) + 1] = {'\0'};
            CopyMemory(buf, zeroes, BUFFERSIZE * sizeof(int) + 1);
        });

        auto printBufferSignal = CreateEventA(
            NULL,
            false,// bManualReset
            false,// bInitialState
            PRINT_BUFFER_SIGNAL
        );

        std::vector<PROCESS_INFORMATION> processes;
        processes.push_back(CreateProcess_(cl_producer));
        processes.push_back(CreateProcess_(cl_consumer));

        cli_runner(mutex, printBufferSignal);
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
            std::cout << "Wrong process type";
        }
    } else {
        std::cout << "Wrong amount of arguments";
    }
}