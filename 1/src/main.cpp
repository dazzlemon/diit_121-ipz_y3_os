#include <windows.h>
#include <process.h>
#include <iostream>
#include <conio.h>
#include <deque>
#include <random>

#define SLEEP 100
#define REPEAT 3

#define N_MIN 1
#define N_MAX 5

// _enddhread is unnecessary

namespace multithread_queue {
    void add(void*);
    void remove(void*);
    void print(void*);
}

CRITICAL_SECTION critical_section;
std::deque<int> q;// deque because need iteration in order to print, otherwise it used as a normal queue

bool rm1 = false;
bool rm2 = false;
bool add1 = false;
bool add2 = false;// these show if thread ended(see multithread_queue::print)

std::random_device g;
std::uniform_int_distribution<int> d(N_MIN, N_MAX);

int main() {
    InitializeCriticalSection(&critical_section);
    {
        HANDLE print   = (HANDLE)_beginthread(multithread_queue::print,  1024, NULL);

        HANDLE add     = (HANDLE)_beginthread(multithread_queue::add,    1024, NULL);
        HANDLE remove  = (HANDLE)_beginthread(multithread_queue::remove, 1024, NULL);

        HANDLE add2    = (HANDLE)_beginthread(multithread_queue::add,    1024, NULL);
        HANDLE remove2 = (HANDLE)_beginthread(multithread_queue::remove, 1024, NULL);

        Sleep(SLEEP * 10);
    }
    DeleteCriticalSection(&critical_section);
}

void multithread_queue::add(void*) {
    //EnterCriticalSection(&critical_section);
    std::cout << "\t\t\t\t\tADD thread started" << std::endl;
    //LeaveCriticalSection(&critical_section);
    for (int i = 0; i < REPEAT; i++) {
        int n = d(g);

        std::random_device generator;
        std::uniform_int_distribution<int> distribution(-100, 100);
        for (int i = 0; i < n; i++) {
            int number = distribution(generator);
            //EnterCriticalSection(&critical_section);
            {
                std::cout << "pushing " << number << std::endl;
                q.push_back(number);
            }
            //LeaveCriticalSection(&critical_section);
            Sleep(SLEEP);
        }
    }
}

void multithread_queue::remove(void*) {
    //EnterCriticalSection(&critical_section);
    std::cout << "\t\t\t\t\tREMOVE thread started" << std::endl;
    //LeaveCriticalSection(&critical_section);
    for (int i = 0; i < REPEAT; i++) {
        int n = d(g);

        while (q.size() > 0 && n > 0) {
            //EnterCriticalSection(&critical_section);
            {
                std::cout << "\tremoving " << q.front() << std::endl;
                q.pop_front();
            }
            n--;
            //LeaveCriticalSection(&critical_section);
            Sleep(SLEEP);
        }
    }
}

void multithread_queue::print(void*) {
    //EnterCriticalSection(&critical_section);
    std::cout << "\t\t\t\t\tPRINT thread started" << std::endl;
    //LeaveCriticalSection(&critical_section);
    while (!(rm1 && rm2 && add1 && add2)) {// repeat until other threads end
        //EnterCriticalSection(&critical_section);
        {
            std::cout << "\t\tprint ";
            for (auto i : q) {
                std::cout << i << " ";
            }
            std::cout << std::endl;
        }
        //LeaveCriticalSection(&critical_section);
        Sleep(SLEEP);
    }
}