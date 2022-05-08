#include <iostream>
#include <algorithm>

#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <cstring>

int conn = 0;// TODO
pthread_mutex_t thread_flag_mutex;
pthread_mutex_t thread_busy_mutex;
pthread_cond_t thread_flag_cv;
int free_thr;
bool thread_flag;
int socketFileDescriptor;
int rc;
const int kBufferSize = 1024;
char buffer[kBufferSize];

void service() {
	std::cout << "service started\n";
	// TODO: also need to check for -1
	while (recv(conn, buffer, kBufferSize, 0) != 0) {
		std::string string(buffer);
		std::sort(string.begin(), string.end());
		send(conn, string.c_str(), string.length(), 0);
		// send(conn, buffer, kBufferSize, 0);
	}
	close(conn);
	std::cout << "service done\n";
}

void* thread_func(void* thread_arg) {
	std::cout << "thread started\n";
	while (true)	{
		std::cout << "thread loop1\n";
		pthread_mutex_lock(&thread_flag_mutex);
		while (!thread_flag) {
			std::cout << "thread loop2\n";
			pthread_cond_wait(&thread_flag_cv, &thread_flag_mutex);
			thread_flag = false;
			std::cout << "thread flag set to false\n";
			
			pthread_mutex_lock(&thread_busy_mutex);
				free_thr--;
				std::cout << "thread being used set to false\n";
			pthread_mutex_unlock(&thread_busy_mutex);
			
			pthread_mutex_unlock(&thread_flag_mutex);
				service();
			pthread_mutex_lock(&thread_busy_mutex);
			
			free_thr++;
			std::cout << "thread freed\n";
			pthread_mutex_unlock(&thread_busy_mutex);
		}
	}
}

#define CREATE_THREAD(name) \
	pthread_t name; \
	pthread_create(&name, NULL, thread_func, NULL); \

int main() {
	pthread_mutex_init(&thread_flag_mutex, NULL);
	pthread_mutex_init(&thread_busy_mutex, NULL);
	pthread_cond_init( &thread_flag_cv,    NULL);
	thread_flag = false;

	CREATE_THREAD(id1)
	CREATE_THREAD(id2)
	CREATE_THREAD(id3)
	free_thr = 3;

	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	addrinfo* addr;
	rc = getaddrinfo(NULL, "1234", &hints, &addr);
	if (rc) {
		std::cout << "Сбой поиска имени хоста\n";// TODO: translate
		return -1;
	}
	
	socketFileDescriptor =
		socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
	if (socketFileDescriptor < 0) {
		std::cout << "Ошибка при создании сокета\n";// TODO: translate
		return -1;
	}
	
	int optval = 1;// allow reuse of local addresses
	setsockopt(
		socketFileDescriptor, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (bind(socketFileDescriptor, addr->ai_addr, addr->ai_addrlen)) {
		std::cout << "Ошибка при связывании сокета\n";// TODO: translate
		return -1;
	}
	
	freeaddrinfo(addr);
	if (listen(socketFileDescriptor, 5)) {
		std::cout << "Ошибка listen\n";// TODO: translate
		return -1;
	}
	std::cout << "---cервер в ожидании\n";// TODO: translate

	sockaddr socketAddress;
	socklen_t socketLength = sizeof(socketAddress);
	while (true)	{
		conn = accept( socketFileDescriptor
		             , reinterpret_cast<sockaddr*>(&socketAddress)
		             , (&socketLength)
		             );
		if (conn == -1) {
			std::cout << "error while accepting connection on socket, errno: "
			          << errno << '\n'; 
			return -1;
		}
		std::cout << "accepted connection on socket\n";

		if (free_thr <= 0) {
			send(conn, buffer, kBufferSize, 0);
			std::cout << "sent answer\n";
		}	else {
			thread_flag = true;
			pthread_cond_signal(&thread_flag_cv);
			std::cout << "no free threads\n";
		}
	}
	return 0;
}