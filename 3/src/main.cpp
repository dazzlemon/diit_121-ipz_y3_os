#define NOMINMAX

#include <iostream>
#include <algorithm>

#include <windows.h>
#include <psapi.h>

#include "winapi_objects_cout.h"
#include "generator.h"
#include "util.h"
#include "win_memory_handling.h"

using MBI = MEMORY_BASIC_INFORMATION;

Generator<MBI> pages(const SYSTEM_INFO& sysinf) {
    MBI mbi;
    for (
        DWORD address = NULL;
        VirtualQuery(reinterpret_cast<LPVOID>(address), &mbi, sizeof(MBI)) && 
            no_overflow_add(address, mbi.RegionSize);
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

void test_granularity(const SYSTEM_INFO& sysinf) {
    LPVOID addr = VirtualAlloc(NULL, NULL, MEM_COMMIT, PAGE_READWRITE);
    MBI mbi;
    VirtualQuery(addr, &mbi, sizeof(MBI));
    std::cout << "Tried to allocate 0 bytes, got " 
              << int_to_hex(mbi.RegionSize) << " bytes" << std::endl;
}

void print_memory_map(const SYSTEM_INFO& sysinf) {
    size_t i = 0;
    size_t pages_max = 10;
    for (auto m : pages(sysinf)) {
        if (i >= pages_max) {
            return;
        }
        print_mbi("mbi" + std::to_string(i++), m);
    }
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
    test_granularity(sysinf);
    test_memory_protection(argc, argv, sysinf);
}