#ifndef SERVER_H
# define SERVER_H

#include "../lib/includes/webserv.h"

class Server {

private:
	std::map<long, std::string>	_requests;
	sockaddr_in	_addr;
	long		_fd;
	long		_newSocket;
	int			_addrLen;

public:
	Server();
	Server(const Server &src);
	~Server();

	Server	&operator=(const Server &src);

	void		setFd(long fd);
	long		getFd(void);
	void		setAddr(sockaddr addr);
	sockaddr	getAddr(void);

};

#endif