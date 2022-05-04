#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "common.h"

int main() {
	std::cout << "Hello from client\n";

	auto chmod = S_IRUSR | S_IWUSR
	           | S_IRGRP | S_IWGRP
	           | S_IROTH | S_IWOTH;

	key_t myIpcKey = ftok(".", 'a');
	int messageQueueId = msgget(myIpcKey, IPC_CREAT | chmod);
	if (messageQueueId == -1) {
		// TODO: error handle
		return -1;
	}
	
	std::cout << "please input 5 integers:\n";
	int arr[5];
	for (int i = 1; i <= 5; i++) {
		std::cout << "\tinteger #" << i << ":";
		std::cin >> arr[i - 1];
	}

	Message message { 0, *arr };
	int rec = msgsnd(
		messageQueueId,
		(struct msgbuf*) (&message),
		sizeof(message.array),
		0
	);
}