#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <cstring>
#include <netdb.h>

int main() {
	sockaddr_in* addrInf;
	int s;
	int rc;
	addrinfo hints;
	addrinfo* addr;
	char buf[1024];
	
	memset(&hints , 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_ADDRCONFIG;
	rc = getaddrinfo("127.0.0.1", NULL, &hints, &addr);
	if (rc) {
		std::cout << "Сбой поиска имени хоста\n";// TODO: translate
		return 1;
	}
	
	addrInf=(sockaddr_in*) addr->ai_addr;
	s = socket(addrInf->sin_family,addr->ai_socktype,addr->ai_protocol);
	if (s < 0) {
		std::cout << "Ошибка при создании сокета\n";// TODO: translate
		return 1;
	}
	
	addrInf->sin_port = htons(1234);
	if (connect(s, (sockaddr*) addrInf, addr->ai_addrlen)) {
		std::cout << "Ошибка при подключении\n";// TODO: translate
		return 1;
	}
	
	freeaddrinfo(addr);
	std::cout << "---сообщение серверу\n";// TODO: translate
	read(0, buf, sizeof(buf));
	send(s, buf, sizeof(buf), 0);
	if (recv(s,buf,sizeof(buf),0)<0) {
		std::cout << "Ошибка при получении ответа\n";// TODO: translate
		return 1;
	}
	
	std::cout << "---получен ответ от сервера\n%s",buf;// TODO: translate
	close(s);
	return 0;
}