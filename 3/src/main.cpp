#include <iostream>
#include <windows.h>
#include <sstream>
#include <iomanip>

template<class T>
void cout_name_val(const std::string& name, const T& val) {
    std::cout << "\t" << name << " = " << val << std::endl;
}

template<typename T>
std::string int_to_hex(T i) {
    std::stringstream stream;
    stream << "0x" 
           << std::setfill ('0') << std::setw(sizeof(T) * 2) 
           << std::hex << i;
    return stream.str();
}

std::string ptr_to_str(void* const t_ptr) {
    return t_ptr == 0 ? "NULL" : int_to_hex(t_ptr);
}

void print_mbi(const MEMORY_BASIC_INFORMATION& mbi) {
    cout_name_val("PVOID  BaseAddress      ", ptr_to_str(mbi.BaseAddress   ));
    cout_name_val("PVOID  AllocationBase   ", ptr_to_str(mbi.AllocationBase));
    cout_name_val("DWORD  AllocationProtect", mbi.AllocationProtect         );
    cout_name_val("SIZE_T RegionSize       ", mbi.RegionSize                );
    cout_name_val("DWORD  State            ", mbi.State                     );
    cout_name_val("DWORD  Protect          ", mbi.Protect                   );
    cout_name_val("DWORD  Type             ", mbi.Type                      );
}

void print_sysinf(const SYSTEM_INFO& si) {
    cout_name_val("WORD      wProcessorArchitecture     ", si.wProcessorArchitecture                 );
    cout_name_val("DWORD     dwPageSize                 ", si.dwPageSize                             );
    cout_name_val("LPVOID    lpMinimumApplicationAddress", ptr_to_str(si.lpMinimumApplicationAddress));
    cout_name_val("LPVOID    lpMaximumApplicationAddress", ptr_to_str(si.lpMaximumApplicationAddress));
    cout_name_val("DWORD_PTR dwActiveProcessorMask      ", si.dwActiveProcessorMask                  );
    cout_name_val("DWORD     dwNumberOfProcessors       ", si.dwNumberOfProcessors                   );
    cout_name_val("DWORD     dwProcessorType            ", si.dwProcessorType                        );
    cout_name_val("DWORD     dwAllocationGranularity    ", si.dwAllocationGranularity                );
    cout_name_val("WORD      wProcessorLevel            ", si.wProcessorLevel                        );
    cout_name_val("WORD      wProcessorRevision         ", si.wProcessorRevision                     );
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