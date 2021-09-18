#ifndef PRODUCER_H
#define PRODUCER_H

#include <iostream>
#include <windows.h>
#include "consumer_producer_base.h"
#include "buffer.h"
#include "logger.h"

class Producer : public ConsumerProducerBase {
protected:
    virtual void _main_loop() final;
    using ConsumerProducerBase::ConsumerProducerBase;
};

void Producer::_main_loop() {
    std::cout << "?";
    WaitForSingleObject(this->_semaphore_empty, INFINITE);// P(empty)
    WaitForSingleObject(this->_semaphore_mutex, INFINITE);// P(mutex)
    this->_buffer->push(1);// produce item
    std::cout << "pushed" << std::endl;
    logger::log_buffer_state(*this->_buffer);
    ReleaseSemaphore(this->_semaphore_mutex, 1, NULL);// V(mutex)
    ReleaseSemaphore(this->_semaphore_full,  1, NULL);// V(full)
}
#endif