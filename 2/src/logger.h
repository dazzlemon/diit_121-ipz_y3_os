#ifndef LOGGER_H
#define LOGGER_H

#include "buffer.h"
namespace logger {

template<class T>
void log_buffer_state(const Buffer<T>& buffer) {
    //system("cls");
    std::cout << "buffer state:" << std::endl;
    std::cout << "\tsize: " << buffer.size() << "/" << buffer.max_size << std::endl;
    std::cout << "\tempty:" << (buffer.empty() ? '+' : '-') << std::endl;
    std::cout << "\tfull:"  << (buffer.full()  ? '+' : '-') << std::endl;
    std::cout << std::endl;
    //Sleep(50);
}
}// namespace logger
#endif