#include <iostream>
#include <windows.h>
#include <list>
#include <vector>

#define BUFFER_SIZE 4
#define ITERATIONS_NUMBER 20

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
    for (size_t i = 0, local_buffer = 1; i < ITERATIONS_NUMBER; i++) {
        WaitForSingleObject(empty, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        item = produce_item();
        put_item(item);

        ReleaseSemaphore(mutex, 1, NULL);

        if (local_buffer >= BUFFER_SIZE) {
            local_buffer = 1;
            ReleaseSemaphore(full, BUFFER_SIZE, NULL);
        } else {
            local_buffer++;
        }
        Sleep(500);
    }
    return 0;
}

DWORD WINAPI ConsumerThreadProc(PVOID p) {
    for (size_t i = 0, local_buffer = 1; i < ITERATIONS_NUMBER; i++) {
        WaitForSingleObject(full, INFINITE);
        WaitForSingleObject(mutex, INFINITE);

        get_item();
        consume_item();

        ReleaseSemaphore(mutex, 1, NULL);

        if (local_buffer >= BUFFER_SIZE) {
            local_buffer = 1;
            ReleaseSemaphore(empty, BUFFER_SIZE, NULL);
        }
        else {
            local_buffer++;
        }
        Sleep(500);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    mutex = CreateSemaphore(NULL, 1, 1, NULL);
    empty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
    full = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);

    HANDLE ProducerThread;
    HANDLE ConsumerThread;

    ProducerThread = CreateThread(NULL, 1024, ProducerThreadProc, NULL, 0, NULL);
    ConsumerThread = CreateThread(NULL, 1024, ConsumerThreadProc, NULL, 0, NULL);

    while (true) {}
}