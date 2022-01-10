#pragma once

#include <iostream>
#include <stdexcept>
#include <system_error>

#include <windows.h>

HANDLE heap;

void heap_init() {
    heap = HeapCreate(0, 0, 0);
    if (!heap) {
        throw new std::system_error(
            GetLastError(), std::system_category(), "Error while trying to create new heap");
    }
}

void* operator new(size_t size) {
    // auto result = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    // if (result) {
    //     return result;
    // }
    // throw new std::system_error(
    //     GetLastError(), std::system_category(), "Error while trying to allocate memory");

    auto result = HeapAlloc(heap, 0, size);
    if (result) {
        return result;
    }
    throw new std::system_error(
        -1, std::system_category(), "Error while trying to allocate memory");
}

void operator delete(void* p) {
    // if (!VirtualFree(p, 0, MEM_RELEASE)) {
    if (!HeapFree(heap, 0, p)) {
        throw new std::system_error(
            GetLastError(), std::system_category(), "Error while trying to release memory");
    }
}