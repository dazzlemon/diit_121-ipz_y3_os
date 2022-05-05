#include <iostream>
#include <vector>
#include <sstream>
#include <deque>

#include <dirent.h>
#include <sys/stat.h>

#include <iostream>

// #define ENABLE_DEBUG

namespace debugging {
#ifdef ENABLE_DEBUG
	constexpr bool debug = true;
#else
	constexpr bool debug = false;
#endif
	template <typename... Args>
	void print(const char* file, int line, Args... args) {
			(std::clog << "[" << file << ":" << line << "] "
								<< ... << args) << std::endl;
	}
}

#define DEBUG(...)                                            \
	do {                                                        \
			if (debugging::debug) {                                 \
					debugging::print(__FILE__, __LINE__, __VA_ARGS__);  \
			}                                                       \
	} while (0)

char filetypeToChar(unsigned char d_type) {
	switch (d_type) {
		case DT_UNKNOWN: return 'u';
		case DT_REG:     return 'f';
		case DT_DIR:     return 'd';
		case DT_FIFO:    return 'p';
		case DT_SOCK:    return 's';
		case DT_CHR:     return 'c';
		case DT_BLK:     return 'b';
		case DT_LNK:     return 'l';
		// mostly bsd, but supported in XFS and CODA with kernel codes
		case DT_WHT:     return 'w';
		default:         return ' '; 
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

struct fileInfo {
	std::string permissions;
	char        type;
	std::string name;
};

std::string fileInfoVectorToString(std::vector<fileInfo> files) {
	std::stringstream stringstream;
	for (auto file : files) {
		stringstream << file.permissions
			           << ' ' << file.type
								 << ' ' << file.name
								 << '\n';
	}
	return stringstream.str();
}

/**
 * argv[1] - directory name
 * argv[2] - file size (bytes)
 * searches directory recursively
 * For all files larger than argv[2] (bytes) prints the following information:
 *   filename
 *   filetype (in the same form that command find accepts:
 * 	   b - block (buffered) special
 *     c - character (unbuffered) special
 *     d - directory
 *     p - named pipe
 *     f - regular file
 *     l - symbolic link
 *     s - socket
 * 
 *     u - unknown filetype (not accepted by find obviously)
 *     w - bsd whiteout (not accepted by find too),
 *       can be supported by XFS or CODA on linux with proper kernel
 *   )
 *   permissions (mode)
 * 
 * one file per line in next format: permissions filetype filename
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

	std::deque<std::string> directoryNames;
	directoryNames.push_back(argv[1]);

	std::vector<fileInfo> files;

	do {// for all directories
		bool continue_ = true;
		
		DIR* directory = opendir(directoryNames[0].c_str());
		if (!directory) {
			std::cout << "error: can't open directory \"" << argv[1] << "\": "
								<< opendirErrnoToString() << '\n';
			return -1;
		}
		DEBUG((std::string("directory ") + directoryNames[0] + " opened"));

		int oldErrno;
		do {// for all files
			oldErrno = errno;
			dirent* directoryEntry = readdir(directory);
			if (directoryEntry) {
				struct stat statbuf;
				std::string filenameRelativeToPwd = directoryNames[0] + "/" +
					directoryEntry->d_name;
				if (stat(filenameRelativeToPwd.c_str(), &statbuf) == -1) {
					std::cout << "error while trying to get info about file permissions "
					          << "for file " << directoryEntry->d_name
										<< ", errno: " << errno << '\n';
					return -1;
				}
				if (std::string(directoryEntry->d_name) != "." &&
				    std::string(directoryEntry->d_name) != ".."
				) {
					if (directoryEntry->d_type == DT_DIR) {
						directoryNames.push_back(filenameRelativeToPwd);
						DEBUG((
							std::string("directory ") + filenameRelativeToPwd +
							" added to directories list"
						));
					}
					files.push_back({
						mode_tToString(statbuf.st_mode),
						filetypeToChar(directoryEntry->d_type),
						filenameRelativeToPwd
					});
					DEBUG((
						std::string("file ") + filenameRelativeToPwd + " added to file list"
					));
				}
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
		closedir(directory);// TODO: check err
		DEBUG((std::string("directory ") + directoryNames[0] + " closed"));
		directoryNames.pop_front();
	} while (!directoryNames.empty());
	

	std::cout << fileInfoVectorToString(files);
}