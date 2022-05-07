#include "common.h"

int main() {
	std::cout << "client started\n";
	SOCKET_INIT
	BIND_CONNECT( connect
	            , "initiating a connection on a socket"
	            , "initiated socket connection"
	            )

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

	ASSERT(	send(socketFileDescriptor, message, strlen(message), 0)
	      , "sending message on a socket"
	      , std::string("sent message to socket: \"") + message + "\""
	      )

	ASSERT(	close(socketFileDescriptor)
	      , "closing socket"
	      , "socket closed"
	      )
	return 0;
}