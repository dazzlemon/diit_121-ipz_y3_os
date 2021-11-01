#include <iostream>
#include <windows.h>
#include <list>
#include <vector>

#define CHUNKSIZE 4
#define ITERS 20

HANDLE mutex;
HANDLE empty;
HANDLE full;

int counter = 0;
std::vector<int> buffer;

void produce_item() {
    int number = ++counter;
    std::cout << "produce item: " << number << "\n";
    buffer.push_back(number);
}

void consume_item() {
    std::cout << "item consumed: " << buffer.back() << std::endl;
    buffer.pop_back();
}


DWORD producer(void*) {
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
        Sleep(500);
    }
    return 0;
}

DWORD consumer(void*) {
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
        Sleep(500);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    mutex = CreateSemaphore(NULL, 1, 1, NULL);
    empty = CreateSemaphore(NULL, CHUNKSIZE, CHUNKSIZE, NULL);
    full  = CreateSemaphore(NULL, 0, CHUNKSIZE, NULL);

    HANDLE producer_thread = CreateThread(NULL, 1024, producer, NULL, 0, NULL);
    HANDLE consumer_thread = CreateThread(NULL, 1024, consumer, NULL, 0, NULL);

    while (true) {}
}