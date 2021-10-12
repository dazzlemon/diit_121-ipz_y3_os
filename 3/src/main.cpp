#include <iostream>
#include <windows.h>

#include "winapi_objects_cout.h"

int main() {
    SYSTEM_INFO sysinf;
    GetSystemInfo(&sysinf);
    print_sysinf("sysinf", sysinf);

    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(NULL, &mbi, sysinf.dwPageSize);
    print_mbi("mbi", mbi);
}