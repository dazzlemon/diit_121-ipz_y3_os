#include <windows.h>
#include <iostream>

void reader(char mutex_name[], char filename[]) {
    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, false, mutex_name);

    do {
        WaitForSingleObject(mutex, INFINITE);
        std::cout << "reader" << std::endl; 
        // reading data
        ReleaseMutex(mutex);
    } while (true);
}

void writer(char mutex_name[], char filename[]) {
    HANDLE mutex = OpenMutex(MUTEX_ALL_ACCESS, false, mutex_name);

    do {
        WaitForSingleObject(mutex, INFINITE);
        std::cout << "writer" << std::endl; 
        // writing data
        ReleaseMutex(mutex);
    } while (true);
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

        // std::cout << command_line_reader << std::endl;
        // std::cout << command_line_writer << std::endl;

        auto si = STARTUPINFO();

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
            /*lpProcessInformation*/ NULL);
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
            /*lpProcessInformation*/ NULL);
    } else if (argc == 4) {// reader or writer
        auto filename     = argv[1];
        auto process_type = argv[2];
        auto mutexname    = argv[3];

        std::cout << argv[0] << " "
                  << argv[1] << " "
                  << argv[2] << " "
                  << argv[3] << " ";

        if (process_type == "reader") {
            reader(mutexname, filename);
        } else if (process_type == "writer") {
            writer(mutexname, filename);
        } else {
            std::cout << "wrong process type" << std::endl;
        }
    } else {
        std::cout << "wrong amount of arguments" << std::endl;
    }
}