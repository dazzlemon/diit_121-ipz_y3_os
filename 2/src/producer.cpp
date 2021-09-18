#include "logger.h"
#include "producer.h"
#include <iostream>

void Producer::_main_loop() {
    WaitForSingleObject(this->_semaphore_empty, INFINITE);// P(empty)
    WaitForSingleObject(this->_semaphore_mutex, INFINITE);// P(mutex)
    this->_buffer->push(1);// produce item
    std::cout << "pushed" << std::endl;
    logger::log_buffer_state(*this->_buffer);
    ReleaseSemaphore(this->_semaphore_mutex, 1, NULL);// V(mutex)
    ReleaseSemaphore(this->_semaphore_full,  1, NULL);// V(full)
}