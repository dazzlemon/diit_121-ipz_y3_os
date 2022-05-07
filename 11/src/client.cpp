#include "common.h"

int main() {
	std::cout << "client started\n";
	SOCKET_FILE_DESCRIPTOR
	SOCKET_ADDRESS;

	auto result = connect(BIND_CONNECT_ARGS);
	if (result == -1) {
		std::cout << "error while initiating a connection on a socket, errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "initiated socket connection\n";

	char message[256];
	std::cout << "please input message: ";
	std::string string;
	std::getline(std::cin, string);
	if (string.length() > 255) {
		std::cout << "error: message too big to send\n";
		return -1;
	}
	std::copy(string.begin(), string.end(), message);
	message[string.length()] = '\0';

	if (send(socketFileDescriptor, message, strlen(message), 0) == -1) {
		std::cout << "error while sending message on a socket, errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "sent message to socket: \"" << message << "\"\n";

	if (close(socketFileDescriptor)) {
		std::cout << "error while closing socket, errno: " << errno << '\n';
		return -1;
	}
	std::cout << "close socket\n";

	return 0;
}