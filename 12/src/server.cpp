#include <iostream>
#include <algorithm>

#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>

#include "common.h"

int conn = 0;// TODO
pthread_mutex_t thread_flag_mutex;
pthread_mutex_t thread_busy_mutex;
pthread_cond_t thread_flag_cv;
int free_thr;
bool thread_flag;
int socketFileDescriptor;
const int kBufferSize = 1024;
char buffer[kBufferSize];

void service() {
	std::cout << "service started\n";
	bool continue_ = true;
	do {
		switch (recv(conn, buffer, kBufferSize, 0)) {
			case -1: std::cout << "error while receiving message from socket " << conn
			                   << ", errno: " << errno << '\n';
			case 0:
				continue_ = false;
				break;
			default:
				std::string string(buffer);
				std::sort(string.begin(), string.end());
				send(conn, string.c_str(), string.length() + 1, 0);
				break;
		}
	} while (continue_);
	close(conn);
	std::cout << "service done\n";
}

#define WITH_MUTEX(mutex, action) \
	pthread_mutex_lock(mutex); \
	action \
	pthread_mutex_unlock(mutex); \

void* thread_func(void* thread_arg) {
	while (true)	{
		pthread_mutex_lock(&thread_flag_mutex);
		while (!thread_flag) {
			pthread_cond_wait(&thread_flag_cv, &thread_flag_mutex);
			thread_flag = false;

			WITH_MUTEX(&thread_busy_mutex, free_thr--;)			
			
			pthread_mutex_unlock(&thread_flag_mutex);
			service();

			WITH_MUTEX(&thread_busy_mutex, free_thr++;)
		}
	}
}

#define CREATE_THREAD(name) \
	pthread_t name; \
	pthread_create(&name, NULL, thread_func, NULL); \

#define ASSERT(flag, errorWhile, done) \
	if (flag == -1) { \
		std::cout << "error while " << errorWhile << ", errno: " << errno << '\n'; \
		return -1; \
	} \
	std::cout << done << '\n';

int main() {
	pthread_mutex_init(&thread_flag_mutex, NULL);
	pthread_mutex_init(&thread_busy_mutex, NULL);
	pthread_cond_init( &thread_flag_cv,    NULL);
	thread_flag = false;

	CREATE_THREAD(id1)
	CREATE_THREAD(id2)
	CREATE_THREAD(id3)
	free_thr = 3;

	HINTS(AI_PASSIVE | AI_ADDRCONFIG)

	int result = getaddrinfo(NULL, "1234", &hints, &addr);
	if (result != 0) {
		std::cout << "Error translating network address, error code" << result
		          << ", errno: " << errno << '\n';
		return -1;
	}
	
	socketFileDescriptor =
		socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (socketFileDescriptor < 0) {
		std::cout << "Error while creating endpoint, errno: " << errno << "\n";
		return -1;
	}
	
	int optval = 1;// allow reuse of local addresses
	setsockopt(
		socketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (bind(socketFileDescriptor, addr->ai_addr, addr->ai_addrlen)) {
		std::cout << "Error binding socket, errno" << errno << "\n";
		return -1;
	}
	freeaddrinfo(addr);
	
	ASSERT( listen(socketFileDescriptor, 5)
	      , "listening for socket connections"
	      , "listening for socket connections"
	      )

	sockaddr socketAddress;
	socklen_t socketLength = sizeof(socketAddress);
	while (true)	{
		conn = accept( socketFileDescriptor
		             , reinterpret_cast<sockaddr*>(&socketAddress)
		             , (&socketLength)
		             );
		ASSERT( conn
		      , "accepting a connection on a socket"
		      , "accepted a connection on socket"
		      )

		if (free_thr <= 0) {
			send(conn, buffer, kBufferSize, 0);
			std::cout << "sent answer\n";
		}	else {
			thread_flag = true;
			pthread_cond_signal(&thread_flag_cv);
		}
	}
}