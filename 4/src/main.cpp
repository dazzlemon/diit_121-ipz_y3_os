#include <QApplication>
#include <QDebug>
#include <QListWidget>
#include <QRandomGenerator>

#include <windows.h>

#include <list>
#include <vector>

#include "MainWindow.h" 

#define CHUNKSIZE 30
#define SLEEPTIME 100

HANDLE mutex;
HANDLE empty;
HANDLE full;

std::vector<int> buffer;

QApplication* a;
MainWindow* w;

void updateBufferList() {
    QListWidget* bufferListWidget = w->findChild<QWidget*>("centralwidget")
                                    ->findChild<QListWidget*>("bufferListWidget");
    bufferListWidget->clear();
    for (auto i : buffer) {
        bufferListWidget->addItem(QString::number(i));
    }
}

void produce_item() {
    int number = QRandomGenerator::global()->generate();
    qDebug() << "produced: " << number;
    buffer.push_back(number);
}

void consume_item() {
    qDebug() << "consumed: " << buffer.back();
    buffer.pop_back();
}

DWORD __stdcall producer(void*) {
    size_t chunk_i = 1;
    while (true) {
        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        produce_item();
        updateBufferList();

        ReleaseSemaphore(mutex, 1, NULL);

        if (chunk_i >= CHUNKSIZE) {
            chunk_i = 1;
            ReleaseSemaphore(full, CHUNKSIZE, NULL);
        } else {
            chunk_i++;
        }
        Sleep(SLEEPTIME);
    }
}

DWORD __stdcall consumer(void*) {
    size_t chunk_i = 1;
    while (true) {
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        consume_item();
        updateBufferList();

        ReleaseSemaphore(mutex, 1, NULL);

        if (chunk_i >= CHUNKSIZE) {
            chunk_i = 1;
            ReleaseSemaphore(empty, CHUNKSIZE, NULL);
        } else {
            chunk_i++;
        }
        Sleep(SLEEPTIME);
    }
}

int argc_; char** argv_;

DWORD __stdcall gui(void*) {
    a = new QApplication(argc_, argv_);
    w = new MainWindow();
    w->show();
    return a->exec();
}

int main(int argc, char* argv[]) {
    argc_ = argc; argv_ = argv;
    a = new QApplication(argc_, argv_);
    w = new MainWindow();

    mutex = CreateSemaphore(NULL, 1, 1, NULL);
    empty = CreateSemaphore(NULL, CHUNKSIZE, CHUNKSIZE, NULL);
    full  = CreateSemaphore(NULL, 0, CHUNKSIZE, NULL);

    HANDLE threads[3];

    threads[0] = CreateThread(NULL, 1024, gui, NULL, 0, NULL);
    threads[1] = CreateThread(NULL, 1024, producer, NULL, 0, NULL);
    threads[2] = CreateThread(NULL, 1024, consumer, NULL, 0, NULL);

    WaitForMultipleObjects(3, threads, true, INFINITE);
}