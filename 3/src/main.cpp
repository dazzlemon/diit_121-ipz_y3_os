#include <iostream>
#include <windows.h>
#include <vector>

using vector_str_str = std::vector<std::pair<std::string, std::string>>;

template<class T>
void cout_name_val(const std::string& name, const T& val) {
    std::cout << "\t" << name << " = " << val << std::endl;
}

void print_mbi(const MEMORY_BASIC_INFORMATION& mbi) {
    cout_name_val("PVOID  BaseAddress      ", mbi.BaseAddress      );
    cout_name_val("PVOID  AllocationBase   ", mbi.AllocationBase   );
    cout_name_val("DWORD  AllocationProtect", mbi.AllocationProtect);
    cout_name_val("SIZE_T RegionSize       ", mbi.RegionSize       );
    cout_name_val("DWORD  State            ", mbi.State            );
    cout_name_val("DWORD  Protect          ", mbi.Protect          );
    cout_name_val("DWORD  Type             ", mbi.Type             );
}

void print_sysinf(const SYSTEM_INFO& si) {
    cout_name_val("WORD      wProcessorArchitecture     ", si.wProcessorArchitecture     );
    cout_name_val("DWORD     dwPageSize                 ", si.dwPageSize                 );
    cout_name_val("LPVOID    lpMinimumApplicationAddress", si.lpMinimumApplicationAddress);
    cout_name_val("LPVOID    lpMaximumApplicationAddress", si.lpMaximumApplicationAddress);
    cout_name_val("DWORD_PTR dwActiveProcessorMask      ", si.dwActiveProcessorMask      );
    cout_name_val("DWORD     dwNumberOfProcessors       ", si.dwNumberOfProcessors       );
    cout_name_val("DWORD     dwProcessorType            ", si.dwProcessorType            );
    cout_name_val("DWORD     dwAllocationGranularity    ", si.dwAllocationGranularity    );
    cout_name_val("WORD      wProcessorLevel            ", si.wProcessorLevel            );
    cout_name_val("WORD      wProcessorRevision         ", si.wProcessorRevision         );
}

int main() {
    SYSTEM_INFO sysinf;
    GetSystemInfo(&sysinf);

    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(NULL, &mbi, sysinf.dwPageSize);

    std::cout << "mbi = " << std::endl;
    print_mbi(mbi);

    std::cout << "sysinf = " << std::endl;
    print_sysinf(sysinf);
}