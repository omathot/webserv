#include <iostream>
#include "../lib/includes/webserv.h"
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Server.h"

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
        int socke_size = sizeof(in_making->_socket);
        if ((in_making->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        int temp = bind(in_making->fd, (struct sockaddr*)&((*in_making)._socket), socke_size);
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

// GET, POST, DELETE, (HEADER)
// use poll, epoll if need separate thread to monitor
int main() {
    std::map<int, running_serveurs *>  config_content = read_config();
    for (auto it = config_content.begin(); it != config_content.end(); it++) {
        std::cout << it->first << ": with fd: " << it->second->fd << " has server: ";
        for (size_t i = 0; i < it->second->mini_server.size(); i++) {
            std::cout << it->second->mini_server[i].name << " = name,  ";
        }
        std::cout << std::endl;
    }
    // int epoll_fd = epoll_create(config_content.size());
    // {
    //     // epoll_ctl(epoll_fd, EPOLL_CTL_ADD, it->second->fd, NULL);
    // }



    for (std::map<int, running_serveurs *>::iterator it = config_content.begin(); it != config_content.end(); it++)
    {
        std::cout << it->first << " : \n";
        std::cout << it->second->fd << " -> " << &it->second->mini_server; 
        long server_fd, new_socket;
        char buffer[BUFFER_SIZE] = {0};
        const char *hello_message = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "<h1>PPOOPP</h1>";
        int addr_len = sizeof(it->second->_socket);
        if (it != config_content.begin()) {
            std::cout << "inside if trying to connect" << std::endl;
            while (true) {
                if ((new_socket = accept(it->second->fd, (struct sockaddr *)&it->second->_socket, (socklen_t*)&addr_len)) < 0) {
                    perror("accept");
                    close(it->second->fd);
                    exit(EXIT_FAILURE);
                }

                read(new_socket, buffer, BUFFER_SIZE);
                std::cout << "Received request:\n" << buffer << std::endl;
                
                send(new_socket, hello_message, strlen(hello_message), 0);
                std::cout << "Hello message sent\n";
                
                close(new_socket);
            }
        }
    }
    
    return 0;
}
