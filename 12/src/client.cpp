#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>

#include "common.h"

int main() {	
	HINTS(AI_ADDRCONFIG)
	GETADDRINFO("127.0.0.1", NULL)
	auto addrInf = reinterpret_cast<sockaddr_in*>(addr->ai_addr);
	SOCKET(addrInf->sin_family)

	addrInf->sin_port = htons(1234);
	BIND_CONNECT( connect
	            , (sockaddr*) addrInf
	            , "initiating a connection on a socket"
	            , "initiated socket connection"
	            )
	freeaddrinfo(addr);
	
	std::cout << "please input message: ";
	std::string string;
	std::getline(std::cin, string);
	if (string.length() > 255) {
		std::cout << "error: message too big to send\n";
		return -1;
	}
	
	if (send( socketFileDescriptor
	        , reinterpret_cast<const void*>(string.c_str())
	        , string.length()
	        , 0
	        ) == -1
	) {
		std::cout << "error while sending message to socket, errno: " << errno
		          << '\n';
		return -1;
	}
	std::cout << "sent message to socket: \"" << string << "\"\n";

	char buf[1024];
	if (recv(socketFileDescriptor, buf, sizeof(buf), 0) == -1) {
		std::cout << "Ошибка при получении ответа\n";// TODO: translate
		return -1;
	}
	std::cout << "received an answer from server: \"" << buf << "\"\n";
	
	close(socketFileDescriptor);
	return 0;
}