#include <windows.h>
#include <string>
#include <stdexcept>

#define RETURN_CASE(arg) case arg: return #arg

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
        RETURN_CASE(PAGE_EXECUTE);
        RETURN_CASE(PAGE_EXECUTE_READ);
        RETURN_CASE(PAGE_EXECUTE_READWRITE);
        RETURN_CASE(PAGE_EXECUTE_WRITECOPY);
        RETURN_CASE(PAGE_NOACCESS);
        RETURN_CASE(PAGE_READONLY);
        RETURN_CASE(PAGE_READWRITE);
        RETURN_CASE(PAGE_WRITECOPY);
        case 0x40000000: return "PAGE_TARAGETS_INVALID or PAGE_TARGETS_NO_UPDATE";
        case 0         : return "no access";
    }
    throw new std::invalid_argument(mp + " is wrong value for memory protection");
}

std::string page_type_to_string(const DWORD& type) {
    switch (type) {
        RETURN_CASE(MEM_IMAGE);
        RETURN_CASE(MEM_MAPPED);
        RETURN_CASE(MEM_PRIVATE);
        case 0: return "no access";// my assumption
    }
    throw new std::invalid_argument(type + " is wrong value for page type");
}

std::string mbi_state_to_string(const DWORD& state) {
    switch (state) {
        RETURN_CASE(MEM_COMMIT);
        RETURN_CASE(MEM_FREE);
        RETURN_CASE(MEM_RESERVE);
    }
    throw new std::invalid_argument(state + " is wrong value for state");
}