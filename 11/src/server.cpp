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

	SOCKET_ADDRESS;
	auto result = bind(BIND_CONNECT_ARGS);
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
	result =
		accept(socketFileDescriptor, (sockaddr*)&socketAddress, &socketLength);
	if (result == -1) {
		std::cout << "error while accepting a connection on a socket, errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "accepted a connection on socket\n";

	char* buf = new char[256];
	if (recv(result, buf, 256, 0) == -1) {
		std::cout << "error while receiving message from socket, errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "received message from socket: \"" << buf << "\"\n";

	if (shutdown(socketFileDescriptor, SHUT_RDWR)) {
		std::cout << "error while closing socket, errno: " << errno << '\n';
		return -1;
	}
	std::cout << "socket shutdown\n";

	return 0;
}