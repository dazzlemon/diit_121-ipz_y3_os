#include <iostream>

#include <termios.h>

int main() {
	termios* termios_p = new termios;
	int fd = 0;// 0 - stdin

	if (tcgetattr(fd, termios_p) == -1) {
		std::cout << "error while getting stdin parameters, errno: "
			<< errno << '\n';
		return -1;
	}

	if (tcsetattr(fd, 0, termios_p) == -1) {
		std::cout << "error while setting stdin parameters, errno: "
			<< errno << '\n';
		return -1;
	}
}