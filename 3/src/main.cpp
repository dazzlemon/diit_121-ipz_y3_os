#include <iostream>
#include <windows.h>
#include <vector>

template<class T>
std::string to_string(T t) {
    return std::to_string(t);
}

std::string to_string(LPVOID& lpvoid) {
    return std::to_string(reinterpret_cast<intptr_t>(lpvoid));
}

void print_mbi(MEMORY_BASIC_INFORMATION* mbi) {
    std::vector<std::string> names = {
        "PVOID  BaseAddress = ",
        "PVOID  AllocationBase = ",
        "DWORD  AllocationProtect = ",
        "SIZE_T RegionSize = ",
        "DWORD  State = ",
        "DWORD  Protect = ",
        "DWORD  Type = ",
    };
    std::vector<std::string> values = {
        to_string(mbi->BaseAddress),
        to_string(mbi->AllocationBase),
        to_string(mbi->AllocationProtect),
        to_string(mbi->RegionSize),
        to_string(mbi->State),
        to_string(mbi->Protect),
        to_string(mbi->Type),
    };
    for (int i = 0; i < names.size(); i++) {
        std::cout << "\t" << names[i] << values[i] << std::endl;
    }
}

int main() {
    SYSTEM_INFO sysinf;
    GetSystemInfo(&sysinf);

    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(NULL, &mbi, sysinf.dwPageSize);

    print_mbi(&mbi);
}