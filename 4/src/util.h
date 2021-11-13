#pragma once

#include <string>
#include <windows.h>

char* cli(std::string execname, std::string filename, std::string process_type) {
    return strdup((execname
        + " " + filename
        + " " + process_type).c_str());
}

PROCESS_INFORMATION CreateProcess_(char* cl) {
    auto si = STARTUPINFOA();
    PROCESS_INFORMATION pi;
    CreateProcessA(
        /*lpApplicationName   */ NULL,
        /*lpCommandLine       */ strdup(cl),
        /*lpProcessAttributes */ NULL,
        /*lpThreadAttributes  */ NULL,
        /*bInheritHandles     */ false,
        /*dwCreationFlags     */ NULL,
        /*lpEnvironment       */ NULL,
        /*lpCurrentDirectory  */ NULL,
        /*lpStartupInfo       */ &si,
        /*lpProcessInformation*/ &pi
    );
    return pi;
}