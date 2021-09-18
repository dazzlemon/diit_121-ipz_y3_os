// Producer-consumer synchronization problem:
//   1. The producer should produce data only when the buffer is not full.
//   2. Data can only be consumed by the consumer if and only if the memory buffer is not empty.
//   3. Accessing memory buffer should not be allowed to producer and consumer at the same time.

#include <windows.h>
#include <process.h>
#include <string>
#include "producer.h"
#include "consumer.h"
#include "buffer.h"

#define BUFFER_SIZE 15

int main(int argc, char* argv[]) {
    int sleep_time;
    if (argc == 2) {// first arg is path
        sleep_time = std::stoi(argv[1]);
    } else {
        sleep_time = 0;
    }

    //                                       NULL, initCount,   maxCount,    name
    HANDLE semaphore_mutex = CreateSemaphore(NULL, 1,           1,           NULL);
    HANDLE semaphore_empty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);
    HANDLE semaphore_full  = CreateSemaphore(NULL, 0,           BUFFER_SIZE, NULL);

    Buffer<int> buffer(BUFFER_SIZE);
    Producer producer(&buffer, semaphore_mutex, semaphore_empty, semaphore_full);
    Consumer consumer(&buffer, semaphore_mutex, semaphore_empty, semaphore_full);

    HANDLE thread_producer = (HANDLE)_beginthread(Producer::main_loop, 4096, &producer);
    HANDLE thread_consumer = (HANDLE)_beginthread(Consumer::main_loop, 4096, &consumer);
    if (sleep_time != 0) {
        Sleep(sleep_time);
    } else {
        while (true) {}
    }
    consumer.running = false;
    producer.running = false;
    Sleep(10);// let child threads stop
}