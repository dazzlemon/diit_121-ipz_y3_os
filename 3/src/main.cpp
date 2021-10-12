#include <iostream>
#include <windows.h>

#include "winapi_objects_cout.h"

#include "util.h"

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

    
    for (
        DWORD address = NULL;
        VirtualQuery(reinterpret_cast<LPVOID>(address), &mbi, sysinf.dwPageSize);
        address += mbi.RegionSize
    ) {
        if (mod(mbi.AllocationProtect) || mod(mbi.Protect))
            print_mbi("mbi", mbi);
    }
}