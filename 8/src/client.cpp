#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "common.h"

int main() {
	std::cout << "Client started\n";

	key_t myIpcKey = ftok(".", 'a');
	if (myIpcKey == -1) {
		std::cout << "error while getting ipc key (ftok), errno: " << errno << '\n';
		return -1;
	}
	std::cout << "ipc key: " << myIpcKey << '\n';

	auto chmod = S_IRUSR | S_IWUSR
	           | S_IRGRP | S_IWGRP
	           | S_IROTH | S_IWOTH;
	int messageQueueId = msgget(myIpcKey, IPC_CREAT | chmod);
	if (messageQueueId == -1) {
		std::cout << "error while getting message queue id (msgget), errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "message queue id: " << messageQueueId << '\n';
	
	Message message;
	message.type = 1;

	std::cout << "please input 5 integers:\n";
	for (int i = 1; i <= 5; i++) {
		std::cout << "\tinteger #" << i << ": ";
		std::cin >> message.array[i - 1];
	}

	int result = msgsnd(
		messageQueueId,
		(struct msgbuf*) (&message),
		sizeof(message.array),
		0
	);
	if (result == -1) {
		std::cout << "error while sending message (msgsnd), errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "sent message:";
	for (int i = 1; i <= 5; i++) {
		std::cout << ' ' << message.array[i - 1];
	}
	std::cout << '\n';
}