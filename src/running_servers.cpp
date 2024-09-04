#include "running_servers.h"

std::vector<server > *make_all_server(std::ifstream &fileToRead);
std::ostream& operator<<(std::ostream& o, const std::vector<server>* to_printf);

RunningServers::RunningServers() {
	std::ifstream inputFile("src/config/default.conf");
    if (!inputFile.is_open()) {
        std::cerr << "Error opening config file" << std::endl;
        exit(1);
    }
    std::vector<server> *servers = make_all_server(inputFile);
    std::vector<int> ports;
    std::map<int, running_server *> running;
    for (int x = 0; x < servers->size(); x++) {
        bool is_in = false;
        for (size_t i = 0; i < ports.size(); i++)
        {
            if (ports[i] == (*servers)[x].port)
                is_in = true;
        }
        if (!is_in)
            ports.push_back((*servers)[x].port);
    }
    for (int x = 0; x < ports.size(); x++) {
        running_server *in_making = new running_server;
        in_making->_socket = new ServerSocket(AF_INET, SOCK_STREAM, 0, ports[x], INADDR_ANY);

        for (int i = 0; i <= servers->size(); i++) {
            if ((*servers)[i].port == ports[x]) {
                in_making->subdomain.push_back((*servers)[i]);
            }
        }
        running[ports[x]] = in_making;
    }
    inputFile.close();
    _nfds = servers->size();  // Assuming one fd per server
    _track_fds.reserve(_nfds);
    int index = 0;
    for (auto it = running.begin(); it != running.end(); it++) {
		std::cout << it->first << "  " << it->second->_socket->getSocketFd() << std::endl;
		push_and_update(it->second->_socket->getSocketFd());
    }
	_servers = running;
	// std::cout << servers << std::endl;
	std::cout << "Hello" << std::endl;
}

RunningServers::~RunningServers() {

}

void RunningServers::push_and_update(int fd) {
	struct pollfd pfd;
	pfd.fd = fd;
	pfd.events = POLLIN;  // Assuming you want to check for read events
	pfd.revents = 0;
	_track_fds.push_back(pfd);
	_nfds++;
}

pollfd* RunningServers::get_track_fds_array() {
	pollfd* poll_events= new pollfd[_nfds];
    return _track_fds.data();  // Returns a pointer to the underlying array
}