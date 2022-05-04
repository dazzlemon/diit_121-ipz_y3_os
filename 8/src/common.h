#include <iostream>

#include <sys/stat.h>
#include <sys/msg.h>

#define MY_IPC_KEY \
	key_t myIpcKey = ftok(".", 'a'); \
	if (myIpcKey == -1) { \
		std::cout << "error while getting ipc key (ftok), errno: " \
							<< errno << '\n'; \
		return -1; \
	} \
	std::cout << "ipc key: " << myIpcKey << '\n';

#define CHECK_MESSAGE_QUEUE_ID \
	if (messageQueueId == -1) { \
		std::cout << "error while getting message queue id (msgget), errno: " \
		          << errno << '\n'; \
		return -1; \
	} \
	std::cout << "message queue id: " << messageQueueId << '\n';

#define CHECK_RESULT(action, functionName) \
	if (result == -1) { \
		std::cout << "error while " << action << " message (" \
		          << functionName << "), errno: " << errno << '\n'; \
		return -1; \
	}

struct Message {
	long type;
	char string[256];
};