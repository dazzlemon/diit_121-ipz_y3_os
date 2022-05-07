#include "common.h"

int main() {
	std::cout << "server started\n";

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
	auto result = bind(
		socketFileDescriptor,
		(sockaddr*)&socketAddress,// TODO: make c++ cast
		sizeof(socketAddress)
	);
	if (result == -1) {
		std::cout << "error while binding a name to a socket, errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "bound a name to a socket\n";

	if (listen(socketFileDescriptor, 20) == -1) {
		std::cout << "error while listening for socket connections, errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "listening for socket connections\n";

	socklen_t socketLength = sizeof(socketAddress);
	if (
		accept(socketFileDescriptor, (sockaddr*)&socketAddress, &socketLength) == -1
	) {
		std::cout << "error while accepting a connection on a socket, errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "accepted a connection on socket\n";

	if (shutdown(socketFileDescriptor, SHUT_RDWR)) {
		std::cout << "error while closing socket, errno: " << errno << '\n';
		return -1;
	}
	std::cout << "socket shutdown\n";

	return 0;
}