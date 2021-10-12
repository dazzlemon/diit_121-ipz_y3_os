#include <iostream>

#include "winapi_def_objects_str.h"
#include "util.h"

void print_mbi(const std::string name, const MEMORY_BASIC_INFORMATION& mbi) {
    std::cout << name << " = " << std::endl;
    cout_name_val("PVOID  BaseAddress      ", ptr_to_str(mbi.BaseAddress                       ));
    cout_name_val("PVOID  AllocationBase   ", ptr_to_str(mbi.AllocationBase                    ));
    cout_name_val("DWORD  AllocationProtect", memory_protection_to_string(mbi.AllocationProtect));
    cout_name_val("SIZE_T RegionSize       ", int_to_hex(mbi.RegionSize                        ));
    cout_name_val("DWORD  State            ", mbi_state_to_string(mbi.State                    ));
    cout_name_val("DWORD  Protect          ", memory_protection_to_string(mbi.Protect          ));
    cout_name_val("DWORD  Type             ", page_type_to_string(mbi.Type                     ));
}

/**
 * obsolete members skipped
 */
void print_sysinf(const std::string name, const SYSTEM_INFO& si) {
    std::cout << name << " = " << std::endl;
    cout_name_val("WORD      wProcessorArchitecture     ",
        sysinfo_proc_arch_to_string(si.wProcessorArchitecture                                       ));
    cout_name_val("DWORD     dwPageSize                 ", int_to_hex(si.dwPageSize                 ));
    cout_name_val("LPVOID    lpMinimumApplicationAddress", ptr_to_str(si.lpMinimumApplicationAddress));
    cout_name_val("LPVOID    lpMaximumApplicationAddress", ptr_to_str(si.lpMaximumApplicationAddress));
    cout_name_val("DWORD_PTR dwActiveProcessorMask      ", si.dwActiveProcessorMask                  );
    cout_name_val("DWORD     dwNumberOfProcessors       ", si.dwNumberOfProcessors                   );
    cout_name_val("DWORD     dwAllocationGranularity    ", int_to_hex(si.dwAllocationGranularity    ));
    cout_name_val("WORD      wProcessorLevel            ", si.wProcessorLevel                        );
    cout_name_val("WORD      wProcessorRevision         ", si.wProcessorRevision                     );
}