#include <iostream>
#include "../lib/includes/webserv.h"
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Server.h"
#include "fcntl.h"

const int PORT = 8080;
const int BACKLOG = 10;
const int BUFFER_SIZE = 1024;

Parse *make_parse(std::ifstream &fileToRead);
void print_parse(Parse *my_parse, int num_tab);
void    free_parse(Parse *my_parse);
std::ostream& operator<<(std::ostream& o, const std::vector<server>* to_printf);

std::map<int, running_serveurs *> read_config() {
    std::ifstream inputFile("src/config/default.conf");
    if (!inputFile.is_open()) {
        std::cerr << "Error opening config file" << std::endl;
        exit(1);
    }
    std::vector<server> *servers = make_all_server(inputFile);
    std::vector<int> ports;
    std::map<int, running_serveurs *> running;
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
        running_serveurs *in_making = new running_serveurs;
        in_making->_socket.sin_family = AF_INET;
        in_making->_socket.sin_port = htons(ports[x]);
        in_making->_socket.sin_addr.s_addr = INADDR_ANY;
        in_making->socke_size = sizeof(in_making->_socket);
        
        if ((in_making->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        int opt = 1;
        if (setsockopt(in_making->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            std::cerr << "Failed to set socket options" << std::endl;
            close(in_making->fd);
            exit(EXIT_FAILURE);
        }
        //fcntl(info->fd, F_SETFL, O_NONBLOCK);

        // std::cout << ports[x];
        int temp = bind(in_making->fd, (struct sockaddr*)&((*in_making)._socket), in_making->socke_size);
        if (temp < 0) {
            perror("bind failed");
            close(in_making->fd);
            exit(EXIT_FAILURE);
        }
        std::cout << "test\n";
        if (listen(in_making->fd, BACKLOG) < 0) {
            perror("listen");
            close(in_making->fd);
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= servers->size(); i++) {
            if ((*servers)[i].port == ports[x]) {
                in_making->mini_server.push_back((*servers)[i]);
            }
        }
        running[ports[x]] = in_making;
    }
    inputFile.close();
    return (running);
}

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


void simple_responce(long new_socket, running_serveurs *info) {
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
    // std:: cout << buffer << std::endl;
    // std::cout << "Received request:\n" << buffer << std::endl;
    // temp_received = Request_convert(buffer);
    // std::cout << temp_received;
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

bool is_open_socket(int i, std::map<int, running_serveurs *> config_content) {
    for (auto it = config_content.begin(); it != config_content.end(); it++) {
        if (i == it->second->fd)
            return true;
    }
    return false;
}


running_serveurs *which_open_socket(int i, std::map<int, running_serveurs *> config_content) {
    for (auto it = config_content.begin(); it != config_content.end(); it++) {
        if (i == it->second->fd)
            return it->second;
    }
    return nullptr;
}

// GET, POST, DELETE, (HEADER)
// use poll, epoll if need separate thread to monitor
int main() {
    // * get config 
    std::map<int, running_serveurs *>  config_content = read_config();
    for (auto it = config_content.begin(); it != config_content.end(); it++) {
        std::cout << it->first << ": with fd: " << it->second->fd << " has server: ";
        for (size_t i = 0; i < it->second->mini_server.size(); i++) {
            std::cout << it->second->mini_server[i].name << " = name,  ";
        }
        std::cout << std::endl;
    }


    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    fd_set read_fds, master_fds;
    int fd_max, new_fd;
    char buffer[BUFFER_SIZE];



    FD_ZERO(&master_fds);
    FD_SET(config_content[8080]->fd, &master_fds);
    FD_SET(config_content[8070]->fd, &master_fds);
    fd_max = std::max(config_content[8080]->fd, config_content[8070]->fd);;

    std::cout << "Server listening on port 8080..." << std::endl;

    while (true) {
        read_fds = master_fds;
        if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select failed");
            return 1;
        }

        for (int i = 0; i <= fd_max; ++i) {
            if (FD_ISSET(i, &read_fds)) {
                if (is_open_socket(i, config_content)) {
                    // New connection
                    new_fd = accept(i, (struct sockaddr *)&client_addr, &client_addr_len);
                    if (new_fd < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("accept failed");
                        }
                    } else {
                        // Set new socket to non-blocking
                        setNonBlocking(new_fd);
                        FD_SET(new_fd, &master_fds);
                        if (new_fd > fd_max) {
                            fd_max = new_fd;
                        }
                        simple_responce(new_fd, which_open_socket(i, config_content));
                        std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << std::endl;
                    }
                } else {
                    // Handle data from a client
                    int nbytes = read(i, buffer, BUFFER_SIZE);
                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            std::cout << "Socket " << i << " closed connection" << std::endl;
                        } else {
                            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                                perror("read failed");
                            }
                        }
                        close(i);
                        FD_CLR(i, &master_fds);
                    } else {
                        buffer[nbytes] = '\0';
                        std::cout << "Received: " << buffer << std::endl;
                        int sent = send(i, buffer, nbytes, 0);
                        if (sent < 0) {
                            if (errno != EWOULDBLOCK && errno != EAGAIN) {
                                perror("send failed");
                                close(i);
                                FD_CLR(i, &master_fds);
                            }
                        }
                    }
                }
            }
        }
    }
    
    return 0;
}

// * random example
// int main() {
//     int server_socket1, server_socket2;
//     struct sockaddr_in server_addr1, server_addr2, client_addr;
//     socklen_t client_addr_len = sizeof(client_addr);
//     fd_set read_fds, master_fds;
//     int fd_max, new_fd;
//     char buffer[BUFFER_SIZE];

//     // Initialize server socket 1 for port 8080
//     server_socket1 = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_socket1 < 0) {
//         perror("socket failed for port 8080");
//         return 1;
//     }
//     setNonBlocking(server_socket1);

//     // Set up server address structure for port 8080
//     server_addr1.sin_family = AF_INET;
//     server_addr1.sin_addr.s_addr = INADDR_ANY;
//     server_addr1.sin_port = htons(8080);

//     if (bind(server_socket1, (struct sockaddr *)&server_addr1, sizeof(server_addr1)) < 0) {
//         perror("bind failed for port 8080");
//         return 1;
//     }

//     if (listen(server_socket1, 5) < 0) {
//         perror("listen failed for port 8080");
//         return 1;
//     }

//     // Initialize server socket 2 for port 8070
//     server_socket2 = socket(AF_INET, SOCK_STREAM, 0);
//     if (server_socket2 < 0) {
//         perror("socket failed for port 8070");
//         return 1;
//     }
//     setNonBlocking(server_socket2);

//     // Set up server address structure for port 8070
//     server_addr2.sin_family = AF_INET;
//     server_addr2.sin_addr.s_addr = INADDR_ANY;
//     server_addr2.sin_port = htons(8070);

//     if (bind(server_socket2, (struct sockaddr *)&server_addr2, sizeof(server_addr2)) < 0) {
//         perror("bind failed for port 8070");
//         return 1;
//     }

//     if (listen(server_socket2, 5) < 0) {
//         perror("listen failed for port 8070");
//         return 1;
//     }

//     FD_ZERO(&master_fds);
//     FD_SET(server_socket1, &master_fds);
//     FD_SET(server_socket2, &master_fds);
//     fd_max = std::max(server_socket1, server_socket2);

//     std::cout << "Server listening on ports 8080 and 8070..." << std::endl;

//     while (true) {
//         read_fds = master_fds;
//         if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) < 0) {
//             perror("select failed");
//             return 1;
//         }

//         for (int i = 0; i <= fd_max; ++i) {
//             if (FD_ISSET(i, &read_fds)) {
//                 if (i == server_socket1 || i == server_socket2) {
//                     // New connection
//                     int server_socket = (i == server_socket1) ? server_socket1 : server_socket2;
//                     new_fd = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
//                     if (new_fd < 0) {
//                         if (errno != EWOULDBLOCK) {
//                             perror("accept failed");
//                         }
//                     } else {
//                         // Set new socket to non-blocking
//                         setNonBlocking(new_fd);
//                         FD_SET(new_fd, &master_fds);
//                         if (new_fd > fd_max) {
//                             fd_max = new_fd;
//                         }
//                         std::cout << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port) << " on port " << (i == server_socket1 ? 8080 : 8070) << std::endl;
//                     }
//                 } else {
//                     // Handle data from a client
//                     int nbytes = read(i, buffer, BUFFER_SIZE);
//                     if (nbytes <= 0) {
//                         if (nbytes == 0) {
//                             std::cout << "Socket " << i << " closed connection" << std::endl;
//                         } else {
//                             if (errno != EWOULDBLOCK && errno != EAGAIN) {
//                                 perror("read failed");
//                             }
//                         }
//                         close(i);
//                         FD_CLR(i, &master_fds);
//                     } else {
//                         buffer[nbytes] = '\0';
//                         std::cout << "Received from socket " << i << ": " << buffer << std::endl;
//                         int sent = send(i, buffer, nbytes, 0);
//                         if (sent < 0) {
//                             if (errno != EWOULDBLOCK && errno != EAGAIN) {
//                                 perror("send failed");
//                                 close(i);
//                                 FD_CLR(i, &master_fds);
//                             }
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     close(server_socket1);
//     close(server_socket2);
//     return 0;
// }