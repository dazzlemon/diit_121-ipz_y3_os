#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>
#include <vector>
#include <stdexcept>

template<class T>
class Buffer {
private:
    std::vector<T> __buffer;
public:
    Buffer(size_t size);
    const size_t size;
    bool empty() const;
    bool full() const;

    void push(const T& t) throw(out_of_range);
    T pop() throw(out_of_range);
};

template<class T>
Buffer<T>::Buffer(size_t size) : size(size) {}

template<class T>
void Buffer<T>::push(const T& t) {
    if (this->full()) {
        throw new out_of_range("Attempt to push to full buffer");
    }
    this->__buffer.push_back(t);
}

template<class T>
T Buffer<T>::pop() {
    if (this->empty()) {
        throw new out_of_range("Attempt to pop from empty buffer");
    }
    auto r = this->__buffer.back();
    this->__buffer.pop();
    return r;
}

template<class T>
bool Buffer<T>::empty() const {
    return this->__buffer.empty();
}

template<class T>
bool Buffer<T>::full() const {
    return this->__buffer.size() >= this->size;
}
#endif