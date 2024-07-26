#include <iostream>
#include "../lib/includes/webserv.h"
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "Server.h"
#include "fcntl.h"

const int BUFFER_SIZE = 1024;
const int BACKLOG = 10;

// not allow with forks
int child_server(running_serveurs *info) {
    long server_fd, new_socket;
    char buffer[BUFFER_SIZE] = {0};
    const char *hello_message = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Content-Length: 13\r\n"
    "\r\n"
    "<h1>PPOOPP</h1>";
    int addr_len = sizeof(info->_socket);
    std::cout << "started port acceptiong " << info->mini_server[0].port;
    if ((info->fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    int opt = 1;
    if (setsockopt(info->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "Failed to set socket options" << std::endl;
        close(info->fd);
        exit(EXIT_FAILURE);
    }
    //fcntl(info->fd, F_SETFL, O_NONBLOCK);

    // std::cout << ports[x];
    int temp = bind(info->fd, (struct sockaddr*)&((*info)._socket), info->socke_size);
    if (temp < 0) {
        perror("bind failed");
        close(info->fd);
        exit(EXIT_FAILURE);
    }
    std::cout << "test\n";
    if (listen(info->fd, BACKLOG) < 0) {
        perror("listen");
        close(info->fd);
        exit(EXIT_FAILURE);
    }
    std::map<REQUEST_INFO, std::string> temp_received;
    while (true) {
        if ((new_socket = accept(info->fd, (struct sockaddr *)&info->_socket, (socklen_t*)&addr_len)) < 0) {
            perror("accept");
            close(info->fd);
            exit(EXIT_FAILURE);
        }
        std::cout << "did it\n\n";
        read(new_socket, buffer, BUFFER_SIZE);
        // std::cout << "Received request:\n" << buffer << std::endl;
        // temp_received = Request_convert(buffer);
        // std::cout << temp_received;
        send(new_socket, hello_message, strlen(hello_message), 0);
        std::cout << "Hello message sent\n";
        
        close(new_socket);
    }
    exit(0);
}

void fork_main() {
    std::vector<pid_t> working_pid; 
    for (std::map<int, running_serveurs *>::iterator it = config_content.begin(); it != config_content.end(); it++)
    {
        pid_t c_pid = fork(); 
    
        if (c_pid == -1) { 
            perror("fork"); 
            exit(EXIT_FAILURE); 
        } 
        else if (c_pid > 0) { 
            //  wait(nullptr); 
            std::cout << "printed from parent process " << getpid() 
                << std::endl;
            working_pid.push_back(c_pid);
        } 
        else {
            child_server(it->second);
            std::cout << "printed from child process " << getpid() 
                << std::endl; 
        } 
        std::cout << "inside if trying to connect" << std::endl;
    }
    while (!working_pid.empty())
    {
        wait(nullptr);
        working_pid.pop_back();
    }
    for (std::map<int, running_serveurs *>::iterator it = config_content.begin(); it != config_content.end(); it++)
    {
        close(it->second->fd);
    }
}
