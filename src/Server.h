#ifndef SERVER_H
# define SERVER_H

# include "webserv.h"

class Server {

private:
	std::map<long, std::string>	_requests;
	sockaddr	_addr;
	long		_fd;

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