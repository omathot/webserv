#include "Socket.h"
#include "unistd.h"

void setNonBlocking(int sockfd);

const int BACKLOG = 10;

ServerSocket::ServerSocket(int domain, int service, int protocol, int port, u_long interface) {
	// establish connection
	std::cout << "domain: " << domain << ", service: " << service << ", protocol: " << protocol << ", port: " << port << ", interface: " << interface << std::endl;
	_sock_fd = socket(domain, service, protocol);
	if (this->_sock_fd < 0) {
		perror("failed _sock_fd");
		exit(EXIT_FAILURE);
	}

	// define address structure
	this->_address.sin_family = domain;
	this->_address.sin_addr.s_addr = interface;
	this->_address.sin_port = htons(port);

	// socket option recomentation of MAX
    int opt = 1;
    if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "Failed to set socket options" << std::endl;
        close(_sock_fd);
        exit(EXIT_FAILURE);
    }

	
	// serverside, so bind() (connect for client)
	std::cout << "sock_fd: " << _sock_fd << ", address: " << &_address << ", sizeof address: " << sizeof(_address) << std::endl;
	if (bind(_sock_fd, (struct sockaddr *)&_address, sizeof(_address)) < 0) {
        perror("bind failed");
        close(_sock_fd);
        exit(EXIT_FAILURE);
    }

	setNonBlocking(_sock_fd);

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
