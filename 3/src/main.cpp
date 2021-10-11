#include <iostream>
#include <windows.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>

template<class T>
void cout_name_val(const std::string& name, const T& val) {
    std::cout << "\t" << name << " = " << val << std::endl;
}

template<typename T>
std::string int_to_hex(T i) {
    std::stringstream stream;
    stream << std::hex << i;
    return stream.str();
}

std::string ptr_to_str(void* const t_ptr) {
    return t_ptr == 0 ? "NULL" : int_to_hex(t_ptr);
}

std::string mbi_state_to_string(const DWORD& state) {
    switch (state) {
        case 0x1000 : return "MEM_COMMIT";
        case 0x10000: return "MEM_FREE";
        case 0x2000 : return "MEM_RESERVE";
    }
    throw new std::invalid_argument(state + " is wrong value for state");
}

std::string sysinfo_proc_arch_to_string(const WORD& pa) {
    switch (pa) {
        case PROCESSOR_ARCHITECTURE_AMD64  : return "x64";
        case PROCESSOR_ARCHITECTURE_ARM    : return "ARM";
        case PROCESSOR_ARCHITECTURE_ARM64  : return "ARM64";
        case PROCESSOR_ARCHITECTURE_IA64   : return "Intel Itanium-based";
        case PROCESSOR_ARCHITECTURE_INTEL  : return "x86";
        case PROCESSOR_ARCHITECTURE_UNKNOWN: return "Unknown architechture";
    }
    throw new std::invalid_argument(pa + " is wrong value for processor architecture");
}

/**
 * modifiers not included
 * 0x100 PAGE_GUARD
 * 0x200 PAGE_NOCACHE
 * 0x400 PAGE_WRITECOMBINE
 */
std::string memory_protection_to_string(const DWORD& mp) {
    switch (mp) {
        case PAGE_EXECUTE          : return "PAGE_EXECUTE";
        case PAGE_EXECUTE_READ     : return "PAGE_EXECUTE_READ";
        case PAGE_EXECUTE_READWRITE: return "PAGE_EXECUTE_READWRITE";
        case PAGE_EXECUTE_WRITECOPY: return "PAGE_EXECUTE_WRITECOPY";
        case PAGE_NOACCESS         : return "PAGE_NOACCESS";
        case PAGE_READONLY         : return "PAGE_READONLY";
        case PAGE_READWRITE        : return "PAGE_READWRITE";
        case PAGE_WRITECOPY        : return "PAGE_WRITECOPY";
        case 0x40000000            : return "PAGE_TARAGETS_INVALID or PAGE_TARGETS_NO_UPDATE";
        case 0                     : return "no access";
    }
    throw new std::invalid_argument(mp + " is wrong value for memory protection");
}

void print_mbi(const std::string name, const MEMORY_BASIC_INFORMATION& mbi) {
    std::cout << name << " = " << std::endl;
    cout_name_val("PVOID  BaseAddress      ", ptr_to_str(mbi.BaseAddress                       ));
    cout_name_val("PVOID  AllocationBase   ", ptr_to_str(mbi.AllocationBase                    ));
    cout_name_val("DWORD  AllocationProtect", memory_protection_to_string(mbi.AllocationProtect));
    cout_name_val("SIZE_T RegionSize       ", int_to_hex(mbi.RegionSize                        ));
    cout_name_val("DWORD  State            ", mbi_state_to_string(mbi.State                    ));
    cout_name_val("DWORD  Protect          ", memory_protection_to_string(mbi.Protect          ));
    cout_name_val("DWORD  Type             ", mbi.Type                                          );
}

/**
 * obsolete members skipped
 */
void print_sysinf(const std::string name, const SYSTEM_INFO& si) {
    std::cout << name << " = " << std::endl;
    cout_name_val("WORD      wProcessorArchitecture     ", sysinfo_proc_arch_to_string(si.wProcessorArchitecture));
    cout_name_val("DWORD     dwPageSize                 ", int_to_hex(si.dwPageSize                             ));
    cout_name_val("LPVOID    lpMinimumApplicationAddress", ptr_to_str(si.lpMinimumApplicationAddress            ));
    cout_name_val("LPVOID    lpMaximumApplicationAddress", ptr_to_str(si.lpMaximumApplicationAddress            ));
    cout_name_val("DWORD_PTR dwActiveProcessorMask      ", si.dwActiveProcessorMask                              );
    cout_name_val("DWORD     dwNumberOfProcessors       ", si.dwNumberOfProcessors                               );
    cout_name_val("DWORD     dwAllocationGranularity    ", int_to_hex(si.dwAllocationGranularity                ));
    cout_name_val("WORD      wProcessorLevel            ", si.wProcessorLevel                                    );
    cout_name_val("WORD      wProcessorRevision         ", si.wProcessorRevision                                 );
}

int main() {
    SYSTEM_INFO sysinf;
    GetSystemInfo(&sysinf);
    print_sysinf("sysinf", sysinf);

    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(NULL, &mbi, sysinf.dwPageSize);
    print_mbi("mbi", mbi);
}