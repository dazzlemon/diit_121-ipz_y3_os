#include "common.h"

int main() {
	std::cout << "client started\n";
	SOCKET_INIT
	BIND_CONNECT( connect
	            , "initiating a connection on a socket"
	            , "initiated socket connection"
	            )

	std::cout << "please input message: ";
	std::string string;
	std::getline(std::cin, string);

	ASSERT(	send( socketFileDescriptor
	            , reinterpret_cast<const void*>(string.c_str())
	            , string.length()
	            , 0
	            )
	      , "sending message on a socket"
	      , std::string("sent message to socket: \"") + string + "\""
	      )

	ASSERT(	close(socketFileDescriptor)
	      , "closing socket"
	      , "socket closed"
	      )
	return 0;
}