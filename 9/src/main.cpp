#include <iostream>

#include <dirent.h>

std::string filetypeToString(unsigned char d_type) {
	switch (d_type) {
		case DT_UNKNOWN: return "unknown";
		case DT_REG:     return "regular file";
		case DT_DIR:     return "directory";
		case DT_FIFO:    return "named pipe";
		case DT_SOCK:    return "local-domain socket";
		case DT_CHR:     return "character device";
		case DT_BLK:     return "block device";
		case DT_LNK:     return "symbolic link";
		// mostly bsd, but supported in XFS and CODA with kernel codes
		case DT_WHT:     return "whiteout";
		default:         return ""; 
	}
}

std::string opendirErrnoToString() {
	switch (errno) {
		// /var/db/sudo
		// find in your system
		// find / -type d ! -readable
		case EACCES:  return "permission denied";
		case EMFILE:  return "too many file descriptors in use by process";
		case ENFILE:  return "too many files are currently opened in the system";
		case ENOENT:  return "directory does not exist";
		case ENOMEM:  return "insufficient memory to complete the operation";
		case ENOTDIR: return "not a directory";
		default: return "";
	}
}

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

	DIR* directory = opendir(argv[1]);
	if (!directory) {
		std::cout << "error: can't open directory \"" << argv[1] << "\": "
		          << opendirErrnoToString() << '\n';
		return -1;
	}
	std::cout << "sucessfuly opened \"" << argv[1] << "\"\n";

	auto oldErrno = errno;
	dirent* directoryEntry;
	bool continue_ = true;
	do {
		directoryEntry = readdir(directory);
		if (directoryEntry) {
			std::cout << "file found:\n"
			          << "\tfilename: " << directoryEntry->d_name << '\n'
								<< "\tfiletype: "
								  << filetypeToString(directoryEntry->d_type) << '\n'
								<< "\tpermission: " << "TODO" << '\n'
								;
		} else {
			continue_ = false;
		}
	} while (continue_);
	if (errno != oldErrno) {
		// may happen if someone deleted directory unexpectedly or something
		// errno == EBADF // the only possible error
		std::cout << "error while getting next directory entry (readdir)\n";
		return -1;
	}
}