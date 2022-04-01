#include <iostream>
#include <optional>

#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

/**
 * returns name of the function that errored out
 */
std::optional<std::string> writeToFile(const char* fileName, void* str, size_t size) {
	int fileDescriptor = creat(fileName, 0666);
	if (fileDescriptor == -1) {
		return "creat";
	}
	if (write(fileDescriptor, str, sizeof(str)) == -1) {
		return "write";
	}
	if (close(fileDescriptor) == -1) {
		return "close";
	}
	return {};
}