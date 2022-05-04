#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#include "common.h"

int main() {
	std::cout << "Hello from server\n";
}