#include <iostream>
#include <windows.h>
#include <system_error>
#include <random>

HANDLE heap;

void heap_init() {
    heap = HeapCreate(0, 0, 0);
    if (!heap) {
        throw new std::system_error(GetLastError(), std::system_category(), "heap error");
    }
}

void* operator new(size_t size) {
    auto result = HeapAlloc(heap, 0, size);
    if (result) {
        std::cout << "new"<< std::endl;
        return result;
    }
    throw new std::system_error(-1, std::system_category(), "new error");
}

void operator delete(void* p) {
    if (HeapFree(heap, 0, p)) {
        std::cout << "delete" << std::endl;
    }
    else {
        throw new std::system_error(GetLastError(), std::system_category(), "delete error");
    }
}

DWORD __stdcall test(PVOID p) {
    heap_init();
    while (true) {
        int* number = new int;
        std::cout << number << std::endl;
        Sleep(1500);
    }
    return 0;
}

int main() {
    HANDLE th1 = CreateThread(NULL, 1024, test, NULL, 0, NULL);
    HANDLE th2 = CreateThread(NULL, 1024, test, NULL, 0, NULL);
    getchar();
}