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

    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(NULL, &mbi, sysinf.dwPageSize);
    print_mbi("mbi", mbi);

    HANDLE h_process = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc;
    GetProcessMemoryInfo(h_process, &pmc, sizeof(pmc));
    print_pmc("pmc", pmc);

    // DWORD dword_max = std::numeric_limits<DWORD>::max();
    // for (
    //     DWORD address = NULL;
    //     VirtualQuery(reinterpret_cast<LPVOID>(address), &mbi, sysinf.dwPageSize) && address <= dword_max;
    //     address += mbi.RegionSize
    // ) {
    //     print_mbi("mbi", mbi);
    // }
}