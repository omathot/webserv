#include "../lib/includes/webserv.h"
#include <cstring>
#include <unistd.h>
// #include "Server.h"
#include "fcntl.h"
// # include "Parser.h"
#include "running_servers.h"

const int PORT = 8080;
const int BUFFER_SIZE = 1024;
Parse *make_parse(std::ifstream &fileToRead);
void print_parse(Parse *my_parse, int num_tab);
void    free_parse(Parse *my_parse);
std::ostream& operator<<(std::ostream& o, const std::vector<server>* to_printf);
std::vector<server> *make_all_server(std::ifstream &fileToRead);
user_request_info parse_user_buffer(char *buffer);

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
    // by_line[0].replace('\r', ' ');
    // by_line[0].replace('\t', ' ');
    // std::vector<std::string> first_line = my_strsplit(by_line[0], ' ');
    std::map<REQUEST_INFO, std::string> to_return;

    // if (first_line[2].size() > 2)
    //     first_line[2].pop_back();
    // to_return[Method] = first_line[0];
    // to_return[Domane_Name] = first_line[1];
    // to_return[Transfer_Protocol] = first_line[2];

    // std::cout << "size of lines is " << by_line.size() << std::endl;
    for (int i = 3; i < (int)Priority; i++) {
        // std::cout << i << std::endl;
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
    std::cout << temp_received;
}


void simple_responce(long new_socket, running_server *info) {
    long server_fd;
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
    int addr_len = sizeof(info->_socket);
    std::map<REQUEST_INFO, std::string> temp_received;

    read(new_socket, buffer, BUFFER_SIZE);
    send(new_socket, hello_message, strlen(hello_message), 0);
    std::cout << "Hello message sent\n";    
}

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

// GET, POST, DELETE, (HEADER)
// use poll, epoll if need separate thread to monitor
int main() {
    // * get config 
    RunningServers active_servers;
    for (auto it = active_servers._servers.begin(); it != active_servers._servers.end(); it++) {
        std::cout << it->first << ": with fd: " << it->second->_socket->getSocketFd() << " has server: ";
        for (size_t i = 0; i < it->second->subdomain.size(); i++) {
            std::cout << it->second->subdomain[i].name << " = name,  ";
        }
        std::cout << std::endl;
    }


    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    fd_set read_fds, master_fds;
    int fd_max, new_fd, fd_min;
    char buffer[BUFFER_SIZE];
	user_request_info user_info;



    FD_ZERO(&master_fds);
	std::cout << "---MANUAL DEBUG HERE ---" << std::endl;
    FD_SET(active_servers._servers[8080]->_socket->getSocketFd(), &master_fds);
    FD_SET(active_servers._servers[8070]->_socket->getSocketFd(), &master_fds);
    fd_max = std::max(active_servers._servers[8080]->_socket->getSocketFd(), active_servers._servers[8070]->_socket->getSocketFd());
    fd_max = std::max(active_servers._servers[8080]->_socket->getSocketFd(), active_servers._servers[8070]->_socket->getSocketFd());
    fd_min = std::min(active_servers._servers[8080]->_socket->getSocketFd(), active_servers._servers[8070]->_socket->getSocketFd());
	std::cout << "8080 = " << active_servers._servers[8080]->_socket->getSocketFd() << std::endl;
	std::cout << "8070 = " << active_servers._servers[8070]->_socket->getSocketFd() << std::endl;

    std::cout << "Server listening on port 8080..." << std::endl;
	
	int poll_ret;
	int event_index;
    while (true) {
        read_fds = master_fds;
		poll_ret = poll(active_servers._track_fds.data(), active_servers._nfds, 5000);
        if (poll_ret < 0) {
            perror("poll() failed");
            return (1);
        }
		event_index = 0;
        for (int i = 0; i < poll_ret; i++) {
			if (FD_ISSET(active_servers._track_fds[event_index].fd, &read_fds)) {
				while (active_servers._track_fds[event_index].revents == 0) {
					event_index++;
				}
				if (active_servers._track_fds[event_index].revents == POLLIN) {
					new_fd = accept(active_servers._track_fds[event_index].fd, (struct sockaddr *)&client_addr, &client_addr_len);
					if (new_fd < 0) {
						if (errno != EWOULDBLOCK) {
							perror("accept failed");
						}
					} else {
						// Set new socket to non-blocking
						setNonBlocking(new_fd);
						FD_SET(new_fd, &master_fds);
						if (new_fd > fd_max) {
							std::cout << "pushing and updating" << std::endl;
							pause();
							active_servers.push_and_update(new_fd);
						}
						std::cout << "-----UNORTHODOX BUFFER PRINT HERE -----\n" << buffer << std::endl;
						simple_responce(new_fd, which_open_socket(i, active_servers._servers));
						std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;						
					}
				}
				active_servers._track_fds[event_index].revents = 0;
				
			}









        //     if (FD_ISSET(i, &read_fds)) {

        //         // maybe redent
		// 		std::cout << "i = " << i << std::endl;
		// 		new_fd = accept(i, (struct sockaddr *)&client_addr, &client_addr_len);

        //         if (is_open_socket(i, active_servers._servers)) {
        //             // New connection
		// 			// int nbytes = read(i, buffer, BUFFER_SIZE);
		// 			// if (nbytes <= 0) {
		// 			// 	std::cout << "read failed, no user info" << std::endl;
        //             //     if (nbytes == 0) {
        //             //         std::cout << "Socket " << i << " closed connection" << std::endl;
        //             //     } else {
        //             //         if (errno != EWOULDBLOCK && errno != EAGAIN) {
        //             //             perror("read failed");
        //             //         }
        //             //     }
        //             //     close(i);
        //             //     FD_CLR(i, &master_fds);
		// 			// } else {
		// 				// buffer[nbytes] = '\0';
		// 			if (new_fd < 0) {
		// 				if (errno != EWOULDBLOCK) {
		// 					perror("accept failed");
		// 				}
		// 			} else {
		// 				// Set new socket to non-blocking
		// 				setNonBlocking(new_fd);
		// 				FD_SET(new_fd, &master_fds);
		// 				if (new_fd > fd_max) {
		// 					active_servers.push_and_update(new_fd);
		// 				}
		// 				std::cout << "-----UNORTHODOX BUFFER PRINT HERE -----\n" << buffer << std::endl;
		// 				simple_responce(new_fd, which_open_socket(i, active_servers._servers));
		// 				std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
		// 			}
		// 			// }
        //         }
        //         else {
        //             // Handle data from a client
        //             int nbytes = read(i, buffer, BUFFER_SIZE);
        //             if (nbytes <= 0) {
        //                 if (nbytes == 0) {
        //                     std::cout << "Socket " << i << " closed connection" << std::endl;
        //                 } else {
        //                     if (errno != EWOULDBLOCK && errno != EAGAIN) {
        //                         perror("read failed");
        //                     }
        //                 }
        //                 close(i);
        //                 FD_CLR(i, &master_fds);
        //             } else {
        //             	std::cout << "are you ever here?\n";
        //                 buffer[nbytes] = '\0';
		// 				user_info = parse_user_buffer(buffer);
		// 				// if (user_info.domain == "poop.com") {

		// 				// } else {

		// 				// }
        //                 std::cout << "Received: " << buffer << std::endl;
        //                 int sent = send(i, buffer, nbytes, 0);
        //                 if (sent < 0) {
        //                     if (errno != EWOULDBLOCK && errno != EAGAIN) {
        //                         perror("send failed");
        //                         close(i);
        //                         FD_CLR(i, &master_fds);
        //                     }
        //                 }
        //             }
        //         }


        //     }
        }
    }
    
    return 0;
}
