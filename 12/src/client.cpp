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
	
	ASSERT(	send( socketFileDescriptor
	            , reinterpret_cast<const void*>(string.c_str())
	            , string.length()
	            , 0
	            )
	      , "sending message on a socket"
	      , std::string("sent message to socket: \"") + string + "\""
	      )

	char buf[MSG_SIZE];
	ASSERT(	recv(socketFileDescriptor, buf, MSG_SIZE, 0)
	      , "receiving message from socket"
	      , std::string("received message from socket: \"") + buf + "\""
	      )
	
	ASSERT(	close(socketFileDescriptor)
	      , "closing socket"
	      , "socket closed"
	      )
	return 0;
}