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

std::string read_config() {
    std::ifstream inputFile("src/config/default.conf");
    if (!inputFile.is_open()) {
        std::cerr << "Error opening config file" << std::endl;
        exit(1);
    }
    // Parse *sam_truc = make_parse(inputFile);
    // print_parse(sam_truc, 1);
    // free_parse(sam_truc);
    std::vector<server> *servers = make_all_server(inputFile);
    // std::cout << servers;
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
        in_making->_socket.sin_family = htons(ports[x]);
        in_making->_socket.sin_addr.s_addr = INADDR_ANY;
        int temp = bind(in_making->fd, (struct sockaddr*)&(*in_making)._socket, sizeof(in_making->_socket));
        // if (temp < 0) {
        //     perror("bind failed");
        //     close(in_making->fd);
        //     exit(EXIT_FAILURE);
        // }
        if ((in_making->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket failed");
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
    for (std::map<int, running_serveurs *>::iterator it = running.begin(); it != running.end(); it++)
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
        if (it == running.end())
            while (true) {
                if ((new_socket = accept(it->second->fd, (struct sockaddr *)&it->second->_socket, (socklen_t*)&it->second->_socket)) < 0) {
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

    

    std::string line;
    std::string content;
    while (getline(inputFile, line)) {
        content += line;
    }
    inputFile.close();
    return (content);
}

// GET, POST, DELETE, (HEADER)
// use poll, epoll if need separate thread to monitor
int main() {
    std::string config_content = read_config();
    // std::cout << config_content << std::endl;
    // pause();
    // Server server = Server();
    long server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    const char *hello_message = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "<h1>Hello</h1>";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to the port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        read(new_socket, buffer, BUFFER_SIZE);
        std::cout << "Received request:\n" << buffer << std::endl;
        
        send(new_socket, hello_message, strlen(hello_message), 0);
        std::cout << "Hello message sent\n";
        
        close(new_socket);
    }

    close(server_fd);
    return 0;
}
