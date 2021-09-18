#include "consumer_producer_base.h"

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
        std::cout << "*";
        /*this->*/_main_loop();
    }
}

void ConsumerProducerBase::main_loop(void* cpb) {
    static_cast<ConsumerProducerBase*>(cpb)->running = true;
    static_cast<ConsumerProducerBase*>(cpb)->__main_loop();
}