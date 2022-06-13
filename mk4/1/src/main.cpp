#include <iostream>
#include <termios.h>

#define tcsetattr_(term) tcsetattr(0, 0, &term);

int main() {
	std::cout << "Enter password: ";

	termios term;
	tcgetattr(0, &term);

	term.c_lflag &= ~ECHO;
	tcsetattr_(term)

	std::string passwd;
	std::getline(std::cin, passwd);

	term.c_lflag |= ECHO;
	tcsetattr_(term)

	std::cout << "\nYour password is: " << passwd << '\n';
}
