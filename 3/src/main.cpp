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

LPVOID find_page(const SYSTEM_INFO& sysinf, DWORD protection) {
    LPVOID addr = NULL;
    MEMORY_BASIC_INFORMATION mbi;
    DWORD dword_max = std::numeric_limits<DWORD>::max();
    for (
        DWORD address = NULL, i = 0;
        VirtualQuery(reinterpret_cast<LPVOID>(address), &mbi, sysinf.dwPageSize) && 
            address <= dword_max && !addr;
        address += mbi.RegionSize, i++
    ) {
        if (mbi.Protect == protection) {
            addr = mbi.BaseAddress;
            // print_mbi("mbi", mbi);
        }
    }
    return addr;
}

void test_memory_protection(LPVOID addr) {
    // read
    int _ = *reinterpret_cast<int*>(addr);
    std::cout << "read successfull" << std::endl;
    // write
    *reinterpret_cast<int*>(addr) = 3;
    std::cout << "write successfull" << std::endl;
}

void test_granularity() {
    // todo
}

int main(int argc, char* argv[]) {
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
    DWORD pages_max = 20;
    for (
        DWORD address = NULL, i = 0;
        VirtualQuery(reinterpret_cast<LPVOID>(address), &mbi, sysinf.dwPageSize) && 
            address <= dword_max && i < pages_max;
        address += mbi.RegionSize, i++
    ) {
        print_mbi("mbi" + std::to_string(i), mbi);
    }

    if (argc == 2) {
        DWORD mp = 0x0;
        int arg = std::stoi(argv[1]);
        switch (arg) {
            case 1: mp = PAGE_READONLY; break;
            case 2: mp = PAGE_READWRITE; break;
            case 3: mp = PAGE_NOACCESS; break;
            default:
                std::cout << "incorrect arg: " << argv[1]  << std::endl;
                break;
        }
        if (mp != 0x0) {
            std::cout << "testing page with protection: "
                      << memory_protection_to_string(mp) << std::endl;
            auto page_addr = find_page(sysinf, mp);
            test_memory_protection(page_addr);
        }
    } else {
        std::cout << "incorrect amount of args" << std::endl;
    }
    test_granularity();
}