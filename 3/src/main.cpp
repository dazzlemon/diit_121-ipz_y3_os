#define NOMINMAX

#include <iostream>
#include <limits>
#include <stdexcept>
#include <system_error>
#include <algorithm>
#include <ranges>

#include <windows.h>
#include <psapi.h>

#include "winapi_objects_cout.h"
#include "generator.h"

namespace rv = std::ranges::views;
using MBI = MEMORY_BASIC_INFORMATION;

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

Generator<MBI> pages(const SYSTEM_INFO& sysinf) {
    LPVOID addr = NULL;
    MEMORY_BASIC_INFORMATION mbi;
    DWORD dword_max = std::numeric_limits<DWORD>::max();
    for (
        DWORD address = NULL, i = 0;
        VirtualQuery(reinterpret_cast<LPVOID>(address), &mbi, sysinf.dwPageSize) && 
            address <= dword_max && !addr;
        address += mbi.RegionSize, i++
    ) {
        co_yield mbi;
    }
}

LPVOID find_page(const SYSTEM_INFO& sysinf, DWORD protection) {
    auto p = pages(sysinf);
    auto res = std::find_if(p.begin(), p.end(), [&](MBI mbi) {return mbi.Protect == protection;});
    return res == p.end() ? NULL
                          : res->BaseAddress;
}

void test_memory_protection_(LPVOID addr) {
    // read
    int _ = *reinterpret_cast<int*>(addr);
    std::cout << "read successfull" << std::endl;
    // write
    *reinterpret_cast<int*>(addr) = 3;
    std::cout << "write successfull" << std::endl;
}

void test_memory_protection(int argc, char* argv[], const SYSTEM_INFO& sysinf) {
    if (argc == 2) {
        DWORD mp = 0x0;
        int arg = std::stoi(argv[1]);
        switch (arg) {
            case 1: mp = PAGE_READONLY;  break;
            case 2: mp = PAGE_READWRITE; break;
            case 3: mp = PAGE_NOACCESS;  break;
            default:
                std::cout << "incorrect arg: " << argv[1]  << std::endl;
                break;
        }
        if (mp != 0x0) {
            std::cout << "testing page with protection: "
                      << memory_protection_to_string(mp) << std::endl;
            auto page_addr = find_page(sysinf, mp);
            test_memory_protection_(page_addr);
        }
    } else {
        std::cout << "incorrect amount of args" << std::endl;
    }
}

void test_granularity() {
    // todo
}

void print_memory_map(const SYSTEM_INFO& sysinf) {
    size_t i = 0;
    size_t pages_max = 20;
    for (auto m : pages(sysinf)) {
        if (i >= pages_max) {
            return;
        }
        print_mbi("mbi" + std::to_string(i++), m);
    }
    // MEMORY_BASIC_INFORMATION mbi;
    // DWORD dword_max = std::numeric_limits<DWORD>::max();
    // DWORD pages_max = 20;
    // for (
    //     DWORD address = NULL, i = 0;
    //     VirtualQuery(reinterpret_cast<LPVOID>(address), &mbi, sysinf.dwPageSize) && 
    //         address <= dword_max && i < pages_max;
    //     address += mbi.RegionSize, i++
    // ) {
    //     print_mbi("mbi" + std::to_string(i), mbi);
    // }
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

    print_memory_map(sysinf);
    
    test_memory_protection(argc, argv, sysinf);
    test_granularity();
}