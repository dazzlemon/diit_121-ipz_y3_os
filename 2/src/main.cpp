// Producer-consumer synchronization problem:
//   1. The producer should produce data only when the buffer is not full.
//   2. Data can only be consumed by the consumer if and only if the memory buffer is not empty.
//   3. Accessing memory buffer should not be allowed to producer and consumer at the same time.

#include <iostream>
#include <windows.h>
#include "producer.h"
#include "consumer.h"
#include "buffer.h"

#define BUFFER_SIZE 15

int main() {
    //                                       NULL, initCount,   maxCount,    name
    HANDLE semaphore_mutex = CreateSemaphore(NULL, 1,           1,           NULL);
    HANDLE semaphore_empty = CreateSemaphore(NULL, BUFFER_SIZE, 0,           NULL);
    HANDLE semaphore_full  = CreateSemaphore(NULL, 0,           BUFFER_SIZE, NULL);
}