#include <iostream>
#include <optional>
#include <cstring>

#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MESSAGE_LENGTH 256
char* fileName;

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
	char buffer[MESSAGE_LENGTH];
	auto res = readFromFile(
		fileName, static_cast<void*>(buffer), sizeof(buffer) / sizeof(*buffer)
	);
	if (res.has_value()) {
		std::cout << "parent process: " << res.value() << " returned -1\n";
		return;
	}
	std::cout << "parent process: read '" << buffer << "'\n";
}

int main(int argc, char* argv[]) {
	/**
	 * argv[1] - fileName
	 * argv[2] - text(len <= MESSAGE_LENGTH)
	 */

	// verify argc
	if (argc != 3) {
		std::cout << "parent process: Wrong amount of arguments, expected:\n"
		          << "argv[1] - fileName\n"
							<< "argv[2] - text(len <= MESSAGE_LENGTH)\n";
		return -1;
	}

	// verify message length
	auto textlen = strlen(argv[2]);
	if (textlen > MESSAGE_LENGTH) {
		std::cout << "parent process: Wrong length of second argument \"text\":"
		          	<< textlen << '\n'
		          << "Should be <=MESSAGE_LENGTH\n";
		return -1;
	}

	fileName = argv[1];
	signal(SIGUSR1, signalHandler);

	// create child process
	int processId = fork();
	if (processId == -1) {
		std::cout << "parent process: there was an error creating child process\n";
		return -1;
	}

	// child process
	if (processId == 0) {
		char* argv_[]{"bin/child", argv[1], argv[2], NULL};
		if (execvp("bin/child", argv_) == -1) {
			std::cout << "child process: execvp error\n";
			return -1;
		}
	}

	// parent process
	int status;
	if (waitpid(processId, &status, 0) == -1) {
		std::cout << "parent process: waitpid error\n";
		return -1;
	}

	return 0;
}