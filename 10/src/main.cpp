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

	// // Change control symbols
	// termios_p->c_cc[VERASE] = VINTR;// VERASE = DEL
	// termios_p->c_cc[VINTR] = VERASE;// VINTR = INT

	// Change MIN, TIME in noncanonical mode
	termios_p->c_lflag &= ~ICANON;
	
	termios_p->c_cc[VMIN] = 5;
	termios_p->c_cc[VTIME] = 8;

	if (tcsetattr(fd, TCSANOW, termios_p) == -1) {
		std::cout << "error while setting stdin parameters, errno: "
			<< errno << '\n';
		return -1;
	}

	std::string string;
	while (true) {
		std::cin >> string;
		std::cout << "read \"" << string << "\"\n";
	}
}