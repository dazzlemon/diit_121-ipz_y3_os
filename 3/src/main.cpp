#include <iostream>
#include <windows.h>
#include <vector>

void print_mbi(MEMORY_BASIC_INFORMATION* mbi) {
    std::cout << "\tPVOID  BaseAddress = " << mbi->BaseAddress << std::endl;
    std::cout << "\tPVOID  AllocationBase = " << mbi->AllocationBase << std::endl;
    std::cout << "\tDWORD  AllocationProtect = " << mbi->AllocationProtect << std::endl;
    std::cout << "\tSIZE_T RegionSize = " << mbi->RegionSize << std::endl;
    std::cout << "\tDWORD  State = " << mbi->State << std::endl;
    std::cout << "\tDWORD  Protect = " << mbi->Protect << std::endl;
    std::cout << "\tDWORD  Type = " << mbi->Type << std::endl;
}

int main() {
    SYSTEM_INFO sysinf;
    GetSystemInfo(&sysinf);

    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(NULL, &mbi, sysinf.dwPageSize);

    std::cout << "mbi = " << std::endl;
    print_mbi(&mbi);
}