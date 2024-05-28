#ifndef SERVER_H
# define SERVER_H

# include <sys/socket.h>
# include <iostream>

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