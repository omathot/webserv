#include "Socket.h"
#include "unistd.h"

const int BACKLOG = 10;

ServerSocket::ServerSocket(int domain, int service, int protocol, int port, u_long interface) {
	// establish connection
	_sock_fd = socket(domain, service, protocol);
	if (this->_sock_fd < 0) {
		perror("failed _sock_fd");
		exit(EXIT_FAILURE);
	}


	// define address structure
	this->_address.sin_family = domain;
	this->_address.sin_addr.s_addr = interface;
	this->_address.sin_port = htons(port);


	// serverside, so bind() (connect for client)
	if (bind(_sock_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
        perror("bind failed");
        close(_sock_fd);
        exit(EXIT_FAILURE);
    }

	// listen on bound socket
	if (listen(_sock_fd, BACKLOG) < 0) {
        perror("listen");
        close(_sock_fd);
        exit(EXIT_FAILURE);
    }
}

ServerSocket::~ServerSocket() {}

int	ServerSocket::getSocketFd() {
	return (this->_sock_fd);
}

const struct sockaddr_in &ServerSocket::getAddr() const {
	return (this->_address);
}