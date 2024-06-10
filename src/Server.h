#ifndef SERVER_H
# define SERVER_H

# include "webserv.h"

class Server {

private:
	sockaddr	_addr;
	long		_fd;
	std::string	_request;

public:
	Server(/* args */);
	~Server();

	Server	&operator=(const Server &src);

	

};

#endif