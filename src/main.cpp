#include "../lib/includes/webserv.h"
#include <unistd.h>
// #include "Server.h"
#include "fcntl.h"
// # include "Parser.h"
#include "running_servers.h"
# include "Parser.h"

Parse *make_parse(std::ifstream &fileToRead);
void print_parse(Parse *my_parse, int num_tab);
void    free_parse(Parse *my_parse);
std::ostream& operator<<(std::ostream& o, const std::vector<server>* to_printf);
std::vector<server> *make_all_server(std::ifstream &fileToRead);
void handle_connection(int client_fd, running_server* server);

std::string getRequestInfoName(REQUEST_INFO info) {
    switch (info) {
        case Method:                    return "Method";
        case Domane_Name:               return "Domane_Name";
        case Transfer_Protocol:         return "Transfer_Protocol";
        case Host:                      return "Host";
        case User_Agent:                return "User_Agent";
        case Accept:                    return "Accept";
        case Accept_Language:           return "Accept_Language";
        case Accept_Encoding:           return "Accept_Encoding";
        case Connection:                return "Connection";
        case Upgrade_Insecure_Requests: return "Upgrade_Insecure_Requests";
        case Sec_Fetch_Dest:            return "Sec_Fetch_Dest";
        case Sec_Fetch_Mode:            return "Sec_Fetch_Mode";
        case Sec_Fetch_Site:            return "Sec_Fetch_Site";
        case Sec_Fetch_User:            return "Sec_Fetch_User";
        case Priority:                  return "Priority";
        default:                        return "Unknown";
    }
}


// ! first line 2 a un /r dedans
std::map<REQUEST_INFO, std::string> Request_convert(char buffer[BUFFER_SIZE]) {
    std::vector<std::string> by_line = my_strsplit(buffer, '\n');
    std::map<REQUEST_INFO, std::string> to_return;
    for (int i = 3; i < (int)Priority; i++) {
        size_t start = by_line[i - 2].find(':') + 2;
        to_return[(REQUEST_INFO)i] = by_line[i - 2].substr(start, by_line[i - 2].size() - start);
    }
    return (to_return);
}

std::ostream& operator<<(std::ostream& o, std::map<REQUEST_INFO, std::string> to_printf) {
    for (int i = 0; i < (int)Priority; i++) {
        o << getRequestInfoName((REQUEST_INFO)i) << " : " << to_printf[(REQUEST_INFO)i] << std::endl;
    }
    return o;
}

void tester_request_convert() {
    char buffer[BUFFER_SIZE] = "GET / HTTP/1.1\n"
"Host: localhost:8080\n"
"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:127.0) Gecko/20100101 Firefox/127.0\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8\n"
"Accept-Language: en-US,en;q=0.5\n"
"Accept-Encoding: gzip, deflate, br, zstd\n"
"Connection: keep-alive\n"
"Upgrade-Insecure-Requests: 1\n"
"Sec-Fetch-Dest: document\n"
"Sec-Fetch-Mode: navigate\n"
"Sec-Fetch-Site: cross-site\n"
"Priority: u=1\n";
    std::map<REQUEST_INFO, std::string> temp_received;
    temp_received = Request_convert(buffer);
}


void simple_response(long new_socket, running_server *info) {
    char buffer[BUFFER_SIZE] = {0};
    const char *hello_message = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "<h1>PPOOPP</h1>";
    if (info == nullptr) {
        std::perror("got a null as info\n");
    }
    std::map<REQUEST_INFO, std::string> temp_received;

    read(new_socket, buffer, BUFFER_SIZE);
    send(new_socket, hello_message, strlen(hello_message), 0);
}

// /tuct/aginore

// maybe usefull
void setNonBlocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl get");
        exit(EXIT_FAILURE);
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl set");
        exit(EXIT_FAILURE);
    }
}

bool is_open_socket(int i, std::map<int, running_server *> config_content) {
    for (auto it = config_content.begin(); it != config_content.end(); it++) {
        if (i == it->second->_socket->getSocketFd())
            return (true);
    }
    return (false);
}


running_server *which_open_socket(int i, std::map<int, running_server *> config_content) {
    for (auto it = config_content.begin(); it != config_content.end(); it++) {
        if (i == it->second->_socket->getSocketFd())
            return it->second;
    }
    return nullptr;
}


// void	print_server_info(RunningServers &active_servers) {
// 	for (auto it = active_servers._servers.begin(); it != active_servers._servers.end(); ++it) {
//         int port = it->first;
//         running_server* server = it->second;
//     }
// }

int main(int argc, char **argv) {
    if (argc > 2) {
        std::cout << "too many args" << std::endl;
        exit (1);
    }
    RunningServers *active_servers;
    if (argc == 2) {
        active_servers = new RunningServers(argv[1]);
    } else {
        active_servers = new RunningServers();
    }
    // print_server_info(*active_servers);
    while (true) {
        // poll returns the number of event that have changed
        int poll_ret = poll(active_servers->_track_fds.data(), active_servers->_track_fds.size(), 3000); // 3 second time out
        if (poll_ret < 0) {
            perror("poll() failed");
            return (1);
        }

        for (size_t i = 0; i < active_servers->_track_fds.size(); ++i) {
            if (active_servers->_track_fds[i].revents == 0) {
                continue ;
            }

            int fd = active_servers->_track_fds[i].fd;

            if (active_servers->_track_fds[i].revents & POLLIN) {
                struct sockaddr_in client_addr;
                socklen_t client_addr_len = sizeof(client_addr);
                int client_fd = accept(fd, (struct sockaddr*)&client_addr, &client_addr_len);
                
                if (client_fd >= 0) {
                    // setNonBlocking(client_fd);   // trying blocking writing
                    // simple_response(client_fd, active_servers._servers[fd]);
                    handle_connection(client_fd, active_servers->_servers[active_servers->_fd_to_port[fd]]);
                }
                std::cout << "true end of comunication" << std::endl;
            }

            if (active_servers->_track_fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                std::cerr << "Error condition on server socket " << fd << std::endl;
                // Handle error on server socket if necessary
            }

            active_servers->_track_fds[i].revents = 0;
        }
    }
    delete active_servers;
    return (0);
}
