#include <iostream>

#include <dirent.h>

/**
 * argv[1] - directory name
 * argv[2] - file size (bytes)
 * searches directory recursively
 * For all files larger than argv[2] (bytes) prints the following information:
 *   filename
 *   filetype (regular or directory)
 *   permissions (mode)
 */
int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cout << "error: wrong amount of arguments provided, "
		          << "expected 2 arguments, but got " << argc - 1 << '\n';
		return 0;
	}

	if (argv[1] == "") {
		std::cout << "error: directory name is empty string\n";
		return -1;
	}

	DIR* dir = opendir(argv[1]);
	if (!dir) {
		std::cout << "error: can't open directory \"" << argv[1] << "\": ";
		switch (errno) {
			case EACCES:
				std::cout << "permission denied";
				break;
			case EMFILE:
				std::cout << "too many file descriptors in use by process";
				break;
			case ENFILE:
				std::cout << "too many files are currently opened in the system";
				break;
			case ENOENT:
				std::cout << "directory does not exist";
				break;
			case ENOMEM:
				std::cout << "insufficient memory to complete the operation";
				break;
			case ENOTDIR:
				std::cout << "not a directory";
				break;
		}
		std::cout << '\n';
		return -1;
	}
	std::cout << "sucessfuly opened \"" << argv[1] << "\"\n";
}