#include "running_servers.h"

std::vector<server > *make_all_server(std::ifstream &fileToRead);
std::ostream& operator<<(std::ostream& o, const std::vector<server>* to_printf);
// const char *config_loc = "config/default.conf";
RunningServers::RunningServers(char *config_path) {
	this->MakeRunningServer(config_path);
}


RunningServers::RunningServers() {
  const char* temp = "config/ourDefault.conf";
  char* config_loc = new char[strlen(temp) + 1];
  strcpy(config_loc, temp);
	this->MakeRunningServer(config_loc);
	delete[] config_loc;
}

void RunningServers::MakeRunningServer(char *config_loc) {
    std::ifstream inputFile(config_loc);
    if (!inputFile.is_open()) {
        std::cerr << "Error opening config file" << std::endl;
        exit(1);
    }
    std::vector<server> *servers = make_all_server(inputFile);
    std::vector<int> ports;
    std::map<int, running_server *> running;
    for (size_t x = 0; x < servers->size(); x++) {
        bool is_in = false;
        for (size_t i = 0; i < ports.size(); i++)
        {
            if (ports[i] == (*servers)[x].port)
                is_in = true;
        }
        if (!is_in)
            ports.push_back((*servers)[x].port);
    }
    for (size_t x = 0; x < ports.size(); x++) {
        running_server *in_making = new running_server;
        in_making->_socket = new ServerSocket(AF_INET, SOCK_STREAM, 0, ports[x], INADDR_ANY);
        for (size_t i = 0; i <= servers->size(); i++) {
            if ((*servers)[i].port == ports[x]) {
                in_making->subdomain.push_back((*servers)[i]);
            }
        }
        running[ports[x]] = in_making;
    }
    inputFile.close();
    _nfds = servers->size();  // Assuming one fd per server
    _track_fds.reserve(_nfds);
    for (auto it = running.begin(); it != running.end(); it++) {
		push_and_update(it->second->_socket->getSocketFd());
        _fd_to_port[it->second->_socket->getSocketFd()] =it->first;
    }
	_servers = running;
    delete servers;
}



RunningServers::~RunningServers() {
  for (const auto &it : this->_servers) {
    delete it.second->_socket;
    delete it.second;
  }
}

void	RunningServers::push_and_update(int fd) {
	pollfd new_pollfd = {fd, POLLIN, 0};
	_track_fds.push_back(new_pollfd);
	_nfds = _track_fds.size();
}

void	RunningServers::remove_fd(int fd) {
	auto it = std::find_if(_track_fds.begin(), _track_fds.end(),
			[fd](const pollfd& pfd) { return pfd.fd == fd; });
	if (it != _track_fds.end()) {
		_track_fds.erase(it);
		_nfds = _track_fds.size();
	}
}


pollfd* RunningServers::get_track_fds_array() {
    return _track_fds.data();  // Returns a pointer to the underlying array
}
