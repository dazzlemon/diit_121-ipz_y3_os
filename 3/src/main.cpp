#define NOMINMAX

#include <iostream>
#include <limits>

#include <windows.h>
#include <psapi.h>

#include "winapi_objects_cout.h"

bool mod(DWORD mp) {
    return intersection(PAGE_GUARD, mp) || intersection(PAGE_NOCACHE, mp) || intersection(PAGE_WRITECOMBINE, mp);
} 

int main() {
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