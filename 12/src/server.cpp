#include <iostream>

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
int thread_flag;
int i;
int s;
int rc;
void* buf1;

void service() {
	const int kBufferSize = 1024;
	char buffer[kBufferSize];
	std::cout << "receiving data\n";
	// TODO: also need to check for -1
	while (recv(conn, buffer, kBufferSize, 0) != 0) {
		std::cout << buffer << "\n";
		send(conn, buffer, kBufferSize, 0);
	}
	close(conn);
	std::cout << "done\n";
}

void* thread_func(void* thread_arg) {
	while (1)	{
		pthread_mutex_lock(&thread_flag_mutex);
		while (!thread_flag) {
			pthread_cond_wait(&thread_flag_cv, &thread_flag_mutex);
			thread_flag = 0;
			pthread_mutex_lock(&thread_busy_mutex);
			free_thr--;
			pthread_mutex_unlock(&thread_busy_mutex);
			pthread_mutex_unlock(&thread_flag_mutex);
			service();
			pthread_mutex_lock(&thread_busy_mutex);
			free_thr++;
			pthread_mutex_unlock(&thread_busy_mutex);
		}
	}
}

int main() {
	sockaddr address;
	size_t addrL = sizeof(address);
	addrinfo hints;
	addrinfo* addr;
	sockaddr_in clt;
	pthread_t id1;
	pthread_t id2;
	pthread_t id3;

	pthread_mutex_init(&thread_flag_mutex, NULL);
	pthread_mutex_init(&thread_busy_mutex, NULL);
	pthread_cond_init(&thread_flag_cv, NULL);
	thread_flag = 0;
	pthread_create(&id1, NULL, thread_func, NULL);
	pthread_create(&id2, NULL, thread_func, NULL);
	pthread_create(&id3, NULL, thread_func, NULL);
	free_thr = 3;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
	if ((rc = getaddrinfo(NULL, "1234", &hints, &addr))) {
		std::cout << "Сбой поиска имени хоста\n";// TODO: translate
		return -1;
	}
	
	if ((s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol)) < 0) {
		std::cout << "Ошибка при создании сокета\n";// TODO: translate
		return -1;
	}
	
	i = 1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i));
	if (bind(s, addr->ai_addr, addr->ai_addrlen)) {
		std::cout << "Ошибка при связывании сокета\n";// TODO: translate
		return -1;
	}
	
	freeaddrinfo(addr);
	if (listen(s, 5)) {
		std::cout << "Ошибка listen\n";// TODO: translate
		return -1;
	}
	
	std::cout << "---cервер в ожидании\n";// TODO: translate
	while (true)	{
		bzero(&clt, sizeof(clt));
		conn = accept( s
		             , reinterpret_cast<sockaddr*>(&address)
		             , reinterpret_cast<socklen_t*>(&addrL)
		             );
		if (conn < 0) {
			return -1;
		}
	
		if (free_thr <= 0) {
			send(conn, buf1, sizeof(buf1), 0);
		}	else {
			thread_flag = 1;
		}
	}
	return 0;
}