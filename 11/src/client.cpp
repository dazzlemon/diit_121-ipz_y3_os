#include <optional>

#include "common.h"

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
	if (write(fileDescriptor, str, size) == -1) {
		return "write";
	}
	if (close(fileDescriptor) == -1) {
		return "close";
	}
	return {};
}

int main() {
	std::cout << "client started\n";

	int socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
	if (socketFileDescriptor == -1) {
		std::cout << "error while creating endpoint for communication, errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "created endpoint for communication\n";

	sockaddr_in socketAddress {
		.sin_family = AF_INET,
		.sin_port = htons(8080),
		.sin_addr = { .s_addr = inet_addr("127.0.0.1") }
	};
	auto result = connect(
		socketFileDescriptor,
		(sockaddr*)&socketAddress,// TODO: make c++ cast
		sizeof(socketAddress)
	);
	if (result == -1) {
		std::cout << "error while initiating a connection on a socket, errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "initiated socket connection\n";

	char filename[] = "test.txt";// TODO
	char message[] = "aboba";// TODO

	// auto res = writeToFile(
	// 	filename, static_cast<void*>(message), strlen(message)
	// );
	// if (res.has_value()) {
	// 	std::cout << res.value() << " returned -1, errno: " << errno << '\n';
	// 	return -1;
	// }
	// std::cout << "wrote to file\n";

	if (send(socketFileDescriptor, message, strlen(message), 0) == -1) {
		std::cout << "error while sending message on a socket, errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "sent message to socket\n";

	if (close(socketFileDescriptor)) {
		std::cout << "error while closing socket, errno: " << errno << '\n';
		return -1;
	}
	std::cout << "close socket\n";

	return 0;
}