#include <iostream>
#include <sstream>
#include <string>

#include <ftw.h>

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

char mode_tToChar(mode_t p) {
	switch (p && S_IFMT) {
		case S_IFREG:  return 'f';
		case S_IFDIR:  return 'd';
		case S_IFIFO:  return 'p';
		case S_IFSOCK: return 's';
		case S_IFCHR:  return 'c';
		case S_IFBLK:  return 'b';
		case S_IFLNK:  return 'l';
		default:       return ' '; 
	}
}

int minsize = 0;
int traverseFile(
	const char* fpath, const struct stat* sb, int tflag, struct FTW* ftwbuf
) {
	if (sb->st_size > minsize) {
		std::cout << mode_tToString(sb->st_mode)
							<< ' ' << mode_tToChar(sb->st_mode)
							<< ' ' << (std::string(fpath) == "." ? fpath : fpath + 2)
							<< '\n';
	}
	return 0;
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

	size_t idx;
	std::string minsizeStr(argv[2]);
	try {
		minsize = std::stoi(minsizeStr, &idx, 10);
	} catch (std::invalid_argument& e) {
		std::cout << "second parameter is not a number\n";
		return -1;
	} catch (std::out_of_range& e) {
		std::cout << "size out of range\n";
		return -1;
	}

	if (minsizeStr.length() != idx) {
		std::cout << "second parameter is not a number\n";
		return -1;
	}

	if (nftw(argv[1], traverseFile, 20, 0) == -1) {
    std::cout << "error while traversing file tree, errno: " << errno << '\n';
    return -1;
  }
	return 0;
}