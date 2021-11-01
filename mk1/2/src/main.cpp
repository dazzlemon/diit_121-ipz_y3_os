#include <iostream>
#include <windows.h>
#include <list>
#include <vector>

#define CHUNKSIZE 4
#define ITERS 20

HANDLE mutex, empty, full;

int counter = 0;
int item;

std::vector<int> items_list;

int produce_item() {
    int number = ++counter;
    std::cout << "produce item: " << number << "\n";
    return number;
}

void put_item(int item) {
    std::cout << "item was put: " << item << "\n";
    items_list.push_back(item);
}

void get_item() {
    std::cout << "get item: " << items_list.back() << "\n";
}

void consume_item() {
    std::cout << "item consumed" << std::endl;
    items_list.pop_back();
}


DWORD WINAPI ProducerThreadProc(PVOID p) {
    for (size_t i = 0, chunk_i = 1; i < ITERS; i++) {
        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        item = produce_item();
        put_item(item);

        ReleaseSemaphore(mutex, 1, NULL);

        if (chunk_i >= CHUNKSIZE) {
            chunk_i = 1;
            ReleaseSemaphore(full, CHUNKSIZE, NULL);
        } else {
            chunk_i++;
        }
        Sleep(500);
    }
    return 0;
}

DWORD WINAPI ConsumerThreadProc(PVOID p) {
    for (size_t i = 0, chunk_i = 1; i < ITERS; i++) {
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        get_item();
        consume_item();

        ReleaseSemaphore(mutex, 1, NULL);

        if (chunk_i >= CHUNKSIZE) {
            chunk_i = 1;
            ReleaseSemaphore(empty, CHUNKSIZE, NULL);
        }
        else {
            chunk_i++;
        }
        Sleep(500);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    mutex = CreateSemaphore(NULL, 1, 1, NULL);
    empty = CreateSemaphore(NULL, CHUNKSIZE, CHUNKSIZE, NULL);
    full = CreateSemaphore(NULL, 0, CHUNKSIZE, NULL);

    HANDLE ProducerThread;
    HANDLE ConsumerThread;

    ProducerThread = CreateThread(NULL, 1024, ProducerThreadProc, NULL, 0, NULL);
    ConsumerThread = CreateThread(NULL, 1024, ConsumerThreadProc, NULL, 0, NULL);

    while (true) {}
}