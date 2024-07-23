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
    for (int i = 0; i <= servers->size(); i++) {
        (*servers)[i]._socket.sin_family = AF_INET;
        (*servers)[i]._socket.sin_addr.s_addr = INADDR_ANY;
        (*servers)[i]._socket.sin_port = htons((*servers)[i].port);

        if (((*servers)[i]._fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }
        std::cout << "first checkpoint" << std::endl;
        struct sockaddr_in *address = &(*servers)[i]._socket;
        if (bind((*servers)[i]._fd, (struct sockaddr *)&(*servers)[i]._socket, sizeof((*servers)[i]._socket)) < 0) {
            perror("bind failed");
            close((*servers)[i]._fd);
            exit(EXIT_FAILURE);
        }
        std::cout << "second checkpoint" << std::endl;
        if (listen((*servers)[i]._fd, BACKLOG) < 0) {
            perror("listen");
            close((*servers)[i]._fd);
            exit(EXIT_FAILURE);
        }
        std::cout << "done i++\n";
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
