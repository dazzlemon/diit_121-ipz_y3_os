#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>

int main() {	
	addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_ADDRCONFIG;
	addrinfo* addr;
	int result = getaddrinfo("127.0.0.1", NULL, &hints, &addr);
	if (result) {
		std::cout << "Сбой поиска имени хоста\n";// TODO: translate
		return -1;
	}
	
	auto addrInf = reinterpret_cast<sockaddr_in*>(addr->ai_addr);
	int socketFileDescriptor =
		socket(addrInf->sin_family, addr->ai_socktype, addr->ai_protocol);
	if (socketFileDescriptor < 0) {
		std::cout << "Ошибка при создании сокета\n";// TODO: translate
		return -1;
	}
	
	addrInf->sin_port = htons(1234);
	if (connect(socketFileDescriptor, (sockaddr*) addrInf, addr->ai_addrlen)) {
		std::cout << "Ошибка при подключении\n";// TODO: translate
		return -1;
	}
	
	freeaddrinfo(addr);
	std::cout << "---сообщение серверу\n";// TODO: translate
	char buf[1024];
	if (read(0, buf, sizeof(buf)) == -1) {
		std::cout << "error while reading, errno: " << errno << '\n';
		return -1;
	} 

	if (send(socketFileDescriptor, buf, sizeof(buf), 0) == -1) {
		std::cout << "error while sending message to socket, errno: " << errno
		          << '\n';
		return -1;
	}

	if (recv(socketFileDescriptor, buf, sizeof(buf), 0) < 0) {
		std::cout << "Ошибка при получении ответа\n";// TODO: translate
		return -1;
	}
	
	std::cout << "---получен ответ от сервера" << buf << '\n';// TODO: translate
	close(socketFileDescriptor);
	return 0;
}