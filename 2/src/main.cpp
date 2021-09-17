// Producer-consumer synchronization problem:
//   1. The producer should produce data only when the buffer is not full.
//   2. Data can only be consumed by the consumer if and only if the memory buffer is not empty.
//   3. Accessing memory buffer should not be allowed to producer and consumer at the same time.

#include <iostream>
#include <vector>
#include <windows.h>

#define BUFFER_SIZE 15

std::vector<int> buffer;

void producer(void*);

bool buffer_is_full() {
    return buffer.size() >= 15;
}

int main() {
    //                                       NULL, initCount, maxCount, name
    HANDLE semaphore_mutex = CreateSemaphore(NULL, );
    HANDLE semaphore_empty;
    HANDLE semaphore_full;
}

void producer(void*) {
    while (true) {
        if (!buffer.empty()) {
            // consume item
        }
    }
}