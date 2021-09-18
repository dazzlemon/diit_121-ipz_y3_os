#include "consumer.h"
#include "logger.h"
#include <iostream>

void Consumer::_main_loop() {
    //WaitForSingleObject(this->_semaphore_full,  INFINITE);// P(full)
    //WaitForSingleObject(this->_semaphore_mutex, INFINITE);// P(mutex)
    this->_buffer->pop();// consume item
    std::cout << "poped" << std::endl;
    logger::log_buffer_state(*this->_buffer);
    //ReleaseSemaphore(this->_semaphore_mutex, 1, NULL);// V(mutex)
    //ReleaseSemaphore(this->_semaphore_empty, 1, NULL);// V(empty)
}