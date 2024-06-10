#include "Server.h"

Server::Server(/* args */) {

}

Server::Server(const Server &src) {
	*this = src;
}

Server::~Server() {

}

Server	&Server::operator=(const Server &src) {
	if (this != &src) {
		this->_addr = src._addr;
		this->_fd = src._fd;
		this->_requests = src._requests;
	}
	return (*this);
}


// Getters n Setters
void	Server::setFd(long fd) {
	this->_fd = fd;
}

long Server::getFd(void) {
	return (this->_fd);
}

void	Server::setAddr(sockaddr addr) {
	this->_addr = addr;
}

sockaddr Server::getAddr(void) {
	return (this->_addr);
}