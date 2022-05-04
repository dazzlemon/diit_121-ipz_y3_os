#include <iostream>
#include <ctime>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "common.h"

std::string time_tToString(time_t time) {
	tm *ltm = std::localtime(&time);
	char mbstr[100];
	std::strftime(mbstr, 100, "%d/%m/%Y %T", ltm);
	return mbstr;
}

int main() {
	std::cout << "Server started\n";

	key_t myIpcKey = ftok(".", 'a');
	if (myIpcKey == -1) {
		std::cout << "error while getting ipc key (ftok), errno: " << errno << '\n';
		return -1;
	}
	std::cout << "ipc key: " << myIpcKey << '\n';

	int messageQueueId = msgget(myIpcKey, 0);
	if (messageQueueId == -1) {
		std::cout << "error while getting message queue id (msgget), errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "message queue id: " << messageQueueId << '\n';

	Message message;
	int result = msgrcv(
		messageQueueId,
		(struct msgbuf*) (&message),
		sizeof(message.array),
		1,
		0
	);
	if (result == -1) {
		std::cout << "error while receiving message (msgrcv), errno: "
		          << errno << '\n';
		return -1;
	}
	std::cout << "received message(size = " << result << "):";
	for (int i = 1; i <= 5; i++) {
		std::cout << ' ' << message.array[i - 1];
	}
	std::cout << '\n';

	msqid_ds messageQueueState;
	msgctl(messageQueueId, IPC_STAT, &messageQueueState);
	std::cout << "message queue state:\n"
	          << "\townership and permissions:\n"
							<< "\t\tcreator user ID: "
								<< messageQueueState.msg_perm.cuid << '\n'
							<< "\t\tcreator group ID: "
								<< messageQueueState.msg_perm.cgid << '\n'
							<< "\t\towner user ID: "
								<< messageQueueState.msg_perm.uid << '\n'
							<< "\t\towner group ID: "
								<< messageQueueState.msg_perm.gid << '\n'
							<< "\t\tpermissions:" << messageQueueState.msg_perm.mode << '\n'
						<< "\ttime of last msgsnd: "
							<< time_tToString(messageQueueState.msg_stime) << '\n'
						<< "\ttime of last msgrcv: "
							<< time_tToString(messageQueueState.msg_rtime) << '\n'
						<< "\ttime of creation or last modification by msgctl: "
							<< time_tToString(messageQueueState.msg_ctime) << '\n'
						<< "\t# of bytes in queue: " << messageQueueState.msg_cbytes << '\n'
						<< "\t# of messages in queue: "
							<< messageQueueState.msg_qnum << '\n'
            << "\t maximum # of bytes in queue: "
							<< messageQueueState.msg_qbytes << '\n'
						<< "\t PID of last msgsnd: " << messageQueueState.msg_lspid << '\n'
						<< "\t PID of last msgrsv: " << messageQueueState.msg_lrpid << '\n';
}