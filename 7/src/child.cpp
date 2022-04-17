#include <iostream>
#include <optional>
#include <cstring>

#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define MESSAGE_LENGTH 256

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

int main(int argc, char* argv[]) {
	/**
	 * argv[1] - fileName
	 * argv[2] - text(len <= MESSAGE_LENGTH)
	 */

	// verify argc
	if (argc != 3) {
		std::cout << "child process: Wrong amount of arguments, expected:\n"
		          << "argv[1] - fileName\n"
							<< "argv[2] - text(len <= MESSAGE_LENGTH)\n";
		return -1;
	}

	// verify message length
	auto textlen = strlen(argv[2]);
	if (textlen > MESSAGE_LENGTH) {
		std::cout << "child process: Wrong length of second argument \"text\":"
		          	<< textlen << '\n'
		          << "Should be <=MESSAGE_LENGTH\n";
		return -1;
	}

	auto res = writeToFile(
		argv[1], static_cast<void*>(argv[2]), strlen(argv[2])
	);
	if (res.has_value()) {
		std::cout << "child process: " << res.value() << " returned -1\n"
		          << "errno: " << errno << '\n';
		return -1;
	}

	// signal to parent process
	if (kill(getppid(), SIGUSR1) != 0) {
		std::cout << "child process: kill error\n";
	}
}