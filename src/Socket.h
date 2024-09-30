#pragma once

#ifndef SOCKET_H
# define SOCKET_H

#include "../lib/includes/webserv.h"

class ServerSocket {
private:
	int _sock_fd;
	struct sockaddr_in _address;

public:
	ServerSocket(int domain, int service, int protocol, int port, u_long interface);
	~ServerSocket();

	int	getSocketFd();
	const struct sockaddr_in	&getAddr() const;
};


#endif