#include "common.h"

#include <cstring>
#include <string>

int main() {
	std::cout << "Client started\n";
	MY_IPC_KEY

	auto chmod = S_IRUSR | S_IWUSR
	           | S_IRGRP | S_IWGRP
	           | S_IROTH | S_IWOTH;
	int messageQueueId = msgget(myIpcKey, IPC_CREAT | chmod);
	CHECK_MESSAGE_QUEUE_ID
	
	Message message;
	message.type = 1;
	std::cout << "please input message:\n";
	std::string string;
	std::getline(std::cin, string);
	if (string.length() > 255) {
		std::cout << "error: message too big to send\n";
		return -1;
	}
	std::copy(string.begin(), string.end(), message.string);
	message.string[string.length()] = '\0';

	int result = msgsnd(
		messageQueueId,
		(struct msgbuf*) (&message),
		sizeof(message.string),
		0
	);
	CHECK_RESULT("sending", "msgsnd")
	std::cout << "sent message: \"" << message.string << "\"\n";
}