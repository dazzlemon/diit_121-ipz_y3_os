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
std::optional<std::string> writeToFile(
	const char* fileName, void* str, size_t size
) {
	auto chmod = S_IRUSR | S_IWUSR
	           | S_IRGRP | S_IWGRP
						 | S_IROTH | S_IWOTH;
	int fileDescriptor = creat(fileName, chmod);
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

int main() {
	char* text = "aboba";
	writeToFile(
		"test.txt", static_cast<void*>(text), sizeof(text) / sizeof(*text)
	);
	int error = kill(getppid(), SIGUSR1);
}