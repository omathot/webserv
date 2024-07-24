#include "Webserver.hpp"
#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

//#define BUFFER_SIZE 1024
#define PORT 8080
#define BACKLOG 10

int main() 
{
    long server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    //char buffer[BUFFER_SIZE] = {0};
    const char *hello_message = "data/www/index.html";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to the port 8080
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //Assigns the specified address to the socket.
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    //the socket is waiting for the connection
    if (listen(server_fd, BACKLOG) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) 
    {
        //connection creation with the socket. 
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) 
        {
            perror("accept");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        //open the file
        std::ifstream file(hello_message);
        if (!file.is_open()) {
            perror("file open failed");
            close(new_socket);
            continue;
        }

        // read the file ("file") and save it in file_content
        std::stringstream file_content;
        //read(new_socket, buffer, BUFFER_SIZE);
        //std::cout << "Received request:\n" << buffer << std::endl;
        file_content << file.rdbuf();
        //converting the stringstream in string
        std::string file_string = file_content.str();
        file.close();

        //creating the HTTP answer
        std::stringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << file_string.length() << "\r\n";
        response << "\r\n";
        response << file_string;
        std::string response_str = response.str();

        // send the file to the client using the socket
        send(new_socket, response_str.c_str(), response_str.length(), 0);
        close(new_socket);
    }
    close(server_fd);
    return 0;
}
