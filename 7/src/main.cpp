#include <iostream>

#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	auto chmod = S_IRUSR | S_IWUSR
	           | S_IRGRP | S_IWGRP
						 | S_IROTH | S_IWOTH;
	std::cout << std::oct << chmod << '\n';
}