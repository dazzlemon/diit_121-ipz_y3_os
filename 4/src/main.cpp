#include <QApplication>
#include <QDebug>
#include <QListWidget>
#include <QRandomGenerator>

#include <windows.h>

#include <deque>
#include <vector>

#include "MainWindow.h" 

#define CHUNKSIZE 30

HANDLE mutex;
HANDLE empty;
HANDLE full;

std::deque<int> buffer;

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

void wait() {
    Sleep(1000 / w->tickrate);
}

DWORD __stdcall producer(void*) {
    while (true) {
        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        buffer.push_back(QRandomGenerator::global()->generate());
        updateBufferList();
        wait();

        ReleaseSemaphore(mutex, 1, NULL);
        ReleaseSemaphore(full,  1, NULL);
    }
}

DWORD __stdcall consumer(void*) {
    while (true) {
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        buffer.pop_front();
        updateBufferList();
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

int main(int argc, char* argv[]) {
    argc_ = argc; argv_ = argv;
    a = new QApplication(argc_, argv_);
    w = new MainWindow();// it wouldn't show otherwise idk why

    mutex = CreateSemaphore(NULL, 1,         1,         NULL);
    empty = CreateSemaphore(NULL, CHUNKSIZE, CHUNKSIZE, NULL);
    full  = CreateSemaphore(NULL, 0,         CHUNKSIZE, NULL);

    auto hGui = CreateThread(NULL, 1024, gui,      NULL, 0, NULL);

    std::vector<HANDLE> threads;
    for (size_t i = 0; i < 2; i++) {
        threads.push_back(CreateThread(NULL, 1024, producer, NULL, 0, NULL));
    }
    threads.push_back(CreateThread(NULL, 1024, consumer, NULL, 0, NULL));

    WaitForSingleObject(hGui, INFINITE);
    for (auto i : threads) {
        TerminateThread(i, 0);
    }
}