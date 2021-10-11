#include <iostream>
#include <windows.h>

void print_mbi(MEMORY_BASIC_INFORMATION* mbi) {
    std::cout << "PVOID  BaseAddress = " << mbi->BaseAddress << std::endl;
    std::cout << "PVOID  AllocationBase = " << mbi->AllocationBase << std::endl;
    std::cout << "DWORD  AllocationProtect = " << mbi->AllocationProtect << std::endl;
    // std::cout << "WORD   PartitionId = " << mbi->PartitionId << std::endl;
    std::cout << "SIZE_T RegionSize = " << mbi->RegionSize << std::endl;
    std::cout << "DWORD  State = " << mbi->State << std::endl;
    std::cout << "DWORD  Protect = " << mbi->Protect << std::endl;
    std::cout << "DWORD  Type = " << mbi->Type << std::endl;
}

int main() {
    SYSTEM_INFO sysinf;
    GetSystemInfo(&sysinf);

    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(NULL, &mbi, sysinf.dwPageSize);

    print_mbi(&mbi);
}