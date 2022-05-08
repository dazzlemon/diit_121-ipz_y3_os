#define HINTS(flags) \
	addrinfo hints { \
		.ai_flags = flags, \
		.ai_socktype = SOCK_STREAM \
	}; \
	addrinfo* addr;

#define GETADDRINFO(ip, port) \
	int result = getaddrinfo(ip, port, &hints, &addr); \
	if (result != 0) { \
		std::cout << "Error translating network address, error code" << result \
		          << ", errno: " << errno << '\n'; \
		return -1; \
	}

#define SOCKET(family) \
	int socketFileDescriptor = \
		socket(family, addr->ai_socktype, addr->ai_protocol); \
	if (socketFileDescriptor < 0) { \
		std::cout << "Error while creating endpoint, errno: " << errno << "\n"; \
		return -1; \
	}