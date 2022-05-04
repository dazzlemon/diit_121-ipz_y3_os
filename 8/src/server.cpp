#include <ctime>

#include "common.h"

std::string time_tToString(time_t time) {
	tm *ltm = std::localtime(&time);
	char mbstr[100];
	std::strftime(mbstr, 100, "%d/%m/%Y %T", ltm);
	return mbstr;
}

std::string mode_tToString(mode_t p) {
	return std::string(
		      (p & S_IREAD)  != 0 ? "r" : "-")
			 + ((p & S_IWRITE) != 0 ? "w" : "-")
			 + ((p & S_IEXEC)  != 0 ? "x" : "-")
			 + ((p & S_IRGRP)  != 0 ? "r" : "-")
			 + ((p & S_IWGRP)  != 0 ? "w" : "-")
			 + ((p & S_IXGRP)  != 0 ? "x" : "-")
			 + ((p & S_IROTH)  != 0 ? "r" : "-")
			 + ((p & S_IWOTH)  != 0 ? "w" : "-")
			 + ((p & S_IXOTH)  != 0 ? "x" : "-")
			 ;
}

int main() {
	std::cout << "Server started\n";
	MY_IPC_KEY
	int messageQueueId = msgget(myIpcKey, 0);
	CHECK_MESSAGE_QUEUE_ID

	Message message;
	int result = msgrcv(
		messageQueueId,
		(struct msgbuf*) (&message),
		sizeof(message.string),
		1,
		0
	);
	CHECK_RESULT("receiving", "msgrcv")
	std::cout << "received message(size = " << result << "):"
	          << " \"" << message.string << "\"\n";

	msqid_ds messageQueueState;
	if (msgctl(messageQueueId, IPC_STAT, &messageQueueState) == -1) {
		std::cout << "error while getting message queue state (msgctl), errno:"
		          << errno << '\n';
		return -1;
	}
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
							<< "\t\tpermissions: "
								<< mode_tToString(messageQueueState.msg_perm.mode) << '\n'
						<< "\ttime of last msgsnd: "
							<< time_tToString(messageQueueState.msg_stime) << '\n'
						<< "\ttime of last msgrcv: "
							<< time_tToString(messageQueueState.msg_rtime) << '\n'
						<< "\ttime of creation or last modification by msgctl: "
							<< time_tToString(messageQueueState.msg_ctime) << '\n'
						<< "\t# of bytes in queue: " << messageQueueState.msg_cbytes << '\n'
						<< "\t# of messages in queue: "
							<< messageQueueState.msg_qnum << '\n'
            << "\tmaximum # of bytes in queue: "
							<< messageQueueState.msg_qbytes << '\n'
						<< "\tPID of last msgsnd: " << messageQueueState.msg_lspid << '\n'
						<< "\tPID of last msgrsv: " << messageQueueState.msg_lrpid << '\n';
}