#pragma once
# include "Socket.h"
// #include "Parser.h"

struct running_server {
    ServerSocket *_socket;
    std::vector<server> subdomain;
    int socke_size;
};

class RunningServers {
public:
	int _nfds;
	std::vector<struct pollfd> _track_fds;
	std::map<int, running_server *> _servers;

	RunningServers();
	~RunningServers();

	void	push_and_update(int fd);
	void	remove_fd(int fd);
	pollfd* get_track_fds_array(); // New method to get raw pointer

};