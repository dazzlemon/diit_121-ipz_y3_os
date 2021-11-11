#include <QApplication>
#include <QDebug>

#include <windows.h>

#include <list>
#include <vector>

#include "MainWindow.h" 

#define CHUNKSIZE 4
#define ITERS 20
#define SLEEPTIME 50

HANDLE mutex;
HANDLE empty;
HANDLE full;

int counter = 0;
std::vector<int> buffer;

void produce_item() {
    int number = ++counter;
    qDebug() << "produced: " << number;
    buffer.push_back(number);
}

void consume_item() {
    qDebug() << "consumed: " << buffer.back();
    buffer.pop_back();
}

DWORD __stdcall producer(void*) {
    for (size_t i = 0, chunk_i = 1; i < ITERS; i++) {
        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        produce_item();

        ReleaseSemaphore(mutex, 1, NULL);

        if (chunk_i >= CHUNKSIZE) {
            chunk_i = 1;
            ReleaseSemaphore(full, CHUNKSIZE, NULL);
        } else {
            chunk_i++;
        }
        Sleep(SLEEPTIME);
    }
    return 0;
}

DWORD __stdcall consumer(void*) {
    for (size_t i = 0, chunk_i = 1; i < ITERS; i++) {
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        consume_item();

        ReleaseSemaphore(mutex, 1, NULL);

        if (chunk_i >= CHUNKSIZE) {
            chunk_i = 1;
            ReleaseSemaphore(empty, CHUNKSIZE, NULL);
        }
        else {
            chunk_i++;
        }
        Sleep(SLEEPTIME);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    mutex = CreateSemaphore(NULL, 1, 1, NULL);
    empty = CreateSemaphore(NULL, CHUNKSIZE, CHUNKSIZE, NULL);
    full  = CreateSemaphore(NULL, 0, CHUNKSIZE, NULL);

    HANDLE threads[2];

    threads[0] = CreateThread(NULL, 1024, producer, NULL, 0, NULL);
    threads[1] = CreateThread(NULL, 1024, consumer, NULL, 0, NULL);

    WaitForMultipleObjects(2, threads, true, INFINITE);

    return a.exec();
}