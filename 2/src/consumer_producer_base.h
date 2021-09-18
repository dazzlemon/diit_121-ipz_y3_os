#ifndef CONSUMER_PRODUCER_BASE_H
#define CONSUMER_PRODUCER_BASE_H

#include <iostream>
#include <windows.h>
#include "buffer.h"
#include "logger.h"

/**
 * main_loop sets running to true,
 * and loops _main_loop until running is set to false
 * (can be done from outside) 
 */
class ConsumerProducerBase {
private:
    void __main_loop();
protected:
    HANDLE _semaphore_mutex;
    HANDLE _semaphore_empty;
    HANDLE _semaphore_full;
    Buffer<int>* _buffer;
    virtual void _main_loop() = 0;// single iteration of main loop
public:
    ConsumerProducerBase(
        Buffer<int>* buffer,
        HANDLE semaphore_mutex,
        HANDLE semaphore_empty,
        HANDLE semaphore_full
    );
    static void main_loop(void* consumer);
    bool running = false;
};// class ConsumerProducerBase

ConsumerProducerBase::ConsumerProducerBase(
    Buffer<int>* buffer,
    HANDLE semaphore_mutex,
    HANDLE semaphore_empty,
    HANDLE semaphore_full
) :
    _buffer(buffer),
    _semaphore_mutex(semaphore_mutex),
    _semaphore_empty(semaphore_empty), 
    _semaphore_full(semaphore_full) {}

void ConsumerProducerBase::__main_loop() {
    while (this->running) {
        std::cout << "iteration" << std::endl;
        this->_main_loop();
        std::cout << "iteration'" << std::endl;
    }
}

void ConsumerProducerBase::main_loop(void* cpb) {
    static_cast<ConsumerProducerBase*>(cpb)->running = true;
    static_cast<ConsumerProducerBase*>(cpb)->__main_loop();
}
#endif