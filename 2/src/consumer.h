#ifndef CONSUMER_H
#define CONSUMER_H

#include <iostream>
#include <windows.h>
#include "buffer.h"

class Consumer {
private:
    HANDLE __semaphore_mutex;
    HANDLE __semaphore_empty;
    HANDLE __semaphore_full;
    Buffer<int>* __buffer;
    void __main_loop();
public:
    Consumer(
        Buffer<int>* buffer,
        HANDLE semaphore_mutex,
        HANDLE semaphore_empty,
        HANDLE semaphore_full
    );
    static void main_loop(void* consumer);
};

Consumer::Consumer(
    Buffer<int>* buffer,
    HANDLE semaphore_mutex,
    HANDLE semaphore_empty,
    HANDLE semaphore_full
) :
    __buffer(buffer),
    __semaphore_mutex(semaphore_mutex),
    __semaphore_empty(semaphore_empty), 
    __semaphore_full(semaphore_full) {}

void Consumer::__main_loop() {
    while (true) {
        std::cout << "consumer" << std::endl;
        WaitForSingleObject(this->__semaphore_full, INFINITE);// P(full)
        WaitForSingleObject(this->__semaphore_mutex, INFINITE);// P(mutex)
        this->__buffer->pop();// consume item
        std::cout << "poped, size: " << this->__buffer->size() << " of " << this->__buffer->max_size << std::endl;
        ReleaseSemaphore(this->__semaphore_mutex, 1, NULL);// V(mutex)
        ReleaseSemaphore(this->__semaphore_empty, 1, NULL);// V(empty)
    }
}

void Consumer::main_loop(void* consumer) {
    static_cast<Consumer*>(consumer)->__main_loop();
}
#endif