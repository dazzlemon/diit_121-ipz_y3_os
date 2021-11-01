#include <windows.h>
#include <iostream>

#define ITERS 10

void reader(HANDLE mutex, HANDLE file) {
    for (int i = 0; i < ITERS; i++) {
        WaitForSingleObject(mutex, INFINITE);
        std::cout << "reader" << std::endl; 
        ReadFile(file, NULL, 0, NULL, NULL);
        ReleaseMutex(mutex);
    }
}

void writer(HANDLE mutex, HANDLE file) {
    for (int i = 0; i < ITERS; i++) {
        WaitForSingleObject(mutex, INFINITE);
        std::cout << "writer" << std::endl; 
        WriteFile(file, NULL, 0, NULL, NULL);
        ReleaseMutex(mutex);
    }
}

int main(int argc, char* argv[]) {
    /**
     * argv[1] - filename
     * argv[2] - process type: reader or writer
     * argv[3] - mutexname
     * 
     * if no arguments 2 & 3 provided then proccess type is infered as main
     */
    if (argc == 2) {// main
        HANDLE mutex = CreateMutex(NULL, false, "mutex");

        std::string filename = argv[1];
        std::string command_line_reader = std::string(argv[0]) + " " + argv[1] + " reader mutex";
        std::string command_line_writer = std::string(argv[0]) + " " + argv[1] + " writer mutex";

        auto si = STARTUPINFO();
        PROCESS_INFORMATION pi;

        CreateProcess(
            /*lpApplicationName   */ NULL,
            /*lpCommandLine       */ strdup(command_line_reader.c_str()),
            /*lpProcessAttributes */ NULL,
            /*lpThreadAttributes  */ NULL,
            /*bInheritHandles     */ false,
            /*dwCreationFlags     */ NULL,
            /*lpEnvironment       */ NULL,
            /*lpCurrentDirectory  */ NULL,
            /*lpStartupInfo       */ &si,
            /*lpProcessInformation*/ &pi);
        CreateProcess(
            /*lpApplicationName   */ NULL,
            /*lpCommandLine       */ strdup(command_line_writer.c_str()),
            /*lpProcessAttributes */ NULL,
            /*lpThreadAttributes  */ NULL,
            /*bInheritHandles     */ false,
            /*dwCreationFlags     */ NULL,
            /*lpEnvironment       */ NULL,
            /*lpCurrentDirectory  */ NULL,
            /*lpStartupInfo       */ &si,
            /*lpProcessInformation*/ &pi);
    } else if (argc == 4) {// reader or writer
        auto filename     = argv[1];
        auto process_type = argv[2];
        auto mutexname    = argv[3];

        HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, false, mutexname);
        HANDLE file  = CreateFile(filename, GENERIC_READ | GENERIC_WRITE, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (std::string(process_type) == "reader") {
            reader(mutex, file);
        } else if (std::string(process_type) == "writer") {
            writer(mutex, file);
        } else {
            std::cout << "wrong process type" << "\'" << process_type << "\'" << std::endl;
        }
    } else {
        std::cout << "wrong amount of arguments" << std::endl;
    }
}