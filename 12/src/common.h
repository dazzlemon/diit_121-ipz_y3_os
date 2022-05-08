#define HINTS(flags) \
	addrinfo hints { \
		.ai_flags = flags, \
		.ai_socktype = SOCK_STREAM \
	}; \
	addrinfo* addr;