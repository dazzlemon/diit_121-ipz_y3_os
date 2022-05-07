#include <iostream>

#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#define SOCKET_FILE_DESCRIPTOR \
	int socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0); \
	if (socketFileDescriptor == -1) { \
		std::cout << "error while creating endpoint for communication, errno: " \
		          << errno << '\n'; \
		return -1; \
	} \
	std::cout << "created endpoint for communication\n";

#define SOCKET_ADDRESS sockaddr_in socketAddress { \
		.sin_family = AF_INET, \
		.sin_port = htons(8080), \
		.sin_addr = { .s_addr = inet_addr("127.0.0.1") } \
	}

#define BIND_CONNECT_ARGS \
	socketFileDescriptor, \
	(sockaddr*)&socketAddress, \
	sizeof(socketAddress)
	// TODO: make c++ cast
	// TODO: add BIND_CONNECT macro