#include "Server.hpp"

Server::Server(/* args */) {

}

Server::~Server() {

}

Server	&Server::operator=(const Server &src) {
	if (this != &src) {
		this->_addr = src._addr;
		this->_fd = src._fd;
		this->_request = src._request;
	}
	return (*this);
}