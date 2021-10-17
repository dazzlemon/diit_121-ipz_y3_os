#pragma once

#include <iostream>
#include <sstream>

template<class T>
void cout_name_val(const std::string& name, const T& val) {
    std::cout << "\t" << name << " = " << val << std::endl;
}

template<typename T>
std::string int_to_hex(T i) {
    std::stringstream stream;
    stream << std::showbase << std::hex << i;
    return stream.str();
}

std::string ptr_to_str(void* const t_ptr) {
    return t_ptr == 0 ? "NULL" : int_to_hex(t_ptr);
}

template<class T1, class T2>
bool intersection(const T1& t1, const T2& t2) {
    return (t1 & t2) == t1;
}

template<class T1, class T2>
requires std::unsigned_integral<T1>
bool no_overflow_add(T1& a, const T2& b) {
    if (!b) {
    return true;
    }
    T1 new_ = a + b;
    bool res = new_ > a;
    a = new_;
    return res;
}