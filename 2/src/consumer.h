#ifndef CONSUMER_H
#define CONSUMER_H

#include <iostream>
#include <windows.h>
#include "consumer_producer_base.h"
#include "buffer.h"
#include "logger.h"

class Consumer : public ConsumerProducerBase {
protected:
    virtual void _main_loop() final;
    using ConsumerProducerBase::ConsumerProducerBase;
};

void Consumer::_main_loop() {
    WaitForSingleObject(this->_semaphore_full,  INFINITE);// P(full)
    WaitForSingleObject(this->_semaphore_mutex, INFINITE);// P(mutex)
    this->_buffer->pop();// consume item
    std::cout << "poped" << std::endl;
    logger::log_buffer_state(*this->_buffer);
    ReleaseSemaphore(this->_semaphore_mutex, 1, NULL);// V(mutex)
    ReleaseSemaphore(this->_semaphore_empty, 1, NULL);// V(empty)
}
#endif