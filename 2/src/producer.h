#ifndef PRODUCER_H
#define PRODUCER_H

#include <iostream>
#include <windows.h>
#include "buffer.h"

class Producer {
private:
    HANDLE __semaphore_mutex;
    HANDLE __semaphore_empty;
    HANDLE __semaphore_full;
    Buffer<int>* __buffer;
public:
    Producer(
        Buffer<int>* buffer,
        HANDLE semaphore_mutex,
        HANDLE semaphore_empty,
        HANDLE semaphore_full
    );
    void main_loop(void*);
};

Producer::Producer(
    Buffer<int>* buffer,
    HANDLE semaphore_mutex,
    HANDLE semaphore_empty,
    HANDLE semaphore_full
) :
    __buffer(buffer),
    __semaphore_mutex(semaphore_mutex),
    __semaphore_empty(semaphore_empty), 
    __semaphore_full(semaphore_full) {}

void Producer::main_loop(void*) {
    while (true) {
        WaitForSingleObject(this->__semaphore_empty, INFINITE);// P(empty)
        WaitForSingleObject(this->__semaphore_mutex, INFINITE);// P(mutex)
        this->__buffer->push(1);// produce item
        std::cout << "pushed, size: " << this->__buffer->size() << " of " << this->__buffer->max_size << std::endl;
        ReleaseSemaphore(this->__semaphore_mutex, 1, NULL);// V(mutex)
        ReleaseSemaphore(this->__semaphore_full, 1, NULL);// V(full)
    }
}
#endif