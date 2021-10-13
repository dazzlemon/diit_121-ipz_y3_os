#define NOMINMAX

#include <iostream>
#include <limits>
#include <stdexcept>
#include <system_error>

#include <windows.h>
#include <psapi.h>

#include "winapi_objects_cout.h"

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

int main() {
    heap_init();

    SYSTEM_INFO sysinf;
    GetSystemInfo(&sysinf);
    print_sysinf("sysinf", sysinf);

    HANDLE h_process = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(h_process, &pmc, sizeof(pmc));
    print_pmc("pmc", pmc);

    MEMORY_BASIC_INFORMATION mbi;
    DWORD dword_max = std::numeric_limits<DWORD>::max();
    for (
        DWORD address = NULL, i = 0;
        VirtualQuery(reinterpret_cast<LPVOID>(address), &mbi, sysinf.dwPageSize) && 
            address <= dword_max && i < 20;
        address += mbi.RegionSize, i++
    ) {
        print_mbi("mbi" + std::to_string(i), mbi);
    }
}