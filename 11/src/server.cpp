#include "common.h"

int main() {
	std::cout << "server started\n";
	SOCKET_INIT
	BIND_CONNECT(bind, "binding a name to a socket", "bound a name to a socket")

	ASSERT( listen(socketFileDescriptor, 20)
	      , "listening for socket connections"
	      , "listening for socket connections"
	      )

	socklen_t socketLength = sizeof(socketAddress);
	result = accept(SOCK_FD_AND_ADDR, &socketLength);
	ASSERT( result
	      , "accepting a connection on a socket"
	      , "accepted a connection on socket"
	      )

	char* buf = new char[256];
	ASSERT(	recv(result, buf, 256, 0)
	      , "receiving message from socket"
	      , std::string("received message from socket: \"") + buf + "\""
	      )

	ASSERT( shutdown(socketFileDescriptor, SHUT_RDWR)
	      , "closing socket"
	      , "socket shutdown"
	      )
	return 0;
}