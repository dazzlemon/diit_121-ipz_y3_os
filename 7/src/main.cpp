#include <iostream>
#include <optional>

#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

std::optional<std::string> readFromFile(
	const char* fileName, void* str, size_t size
) {
	int fileDescriptor = open(fileName, O_RDWR);
	if (fileDescriptor == -1) {
		return "creat";
	}
	if (read(fileDescriptor, str, size) == -1) {
		return "read";
	}
	if (close(fileDescriptor) == -1) {
		return "close";
	}
	return {};
}

void signalHandler(int) {
	char buffer[256];
	readFromFile(
		"test.txt", static_cast<void*>(buffer), sizeof(buffer) / sizeof(*buffer)
	);
	std::cout << buffer << '\n';
}

int main() {
	signal(SIGUSR1, signalHandler);
	int processId = fork();
	if (processId == -1) {
		// error
	} else if (processId == 0) {// child process
		if (execvp("bin/child", NULL) == -1) {
			// TODO
			std::cout << "execvp error\n";
		}
	} else {
		int status;
		if (waitpid(processId, &status, 0) == -1) {
			// error
		}
	}
}