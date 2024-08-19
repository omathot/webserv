#include "../lib/includes/webserv.h"
#include <cstring>
#include <unistd.h>
// #include "Server.h"
#include "Socket.h"

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

Parse *make_parse(std::ifstream &fileToRead);
void print_parse(Parse *my_parse, int num_tab);
void    free_parse(Parse *my_parse);
std::ostream& operator<<(std::ostream& o, const std::vector<server>* to_printf);

// std::string read_config() {
//     std::ifstream inputFile("src/config/default.conf");
//     if (!inputFile.is_open()) {
//         std::cerr << "Error opening config file" << std::endl;
//         exit(1);
//     }
//     // std::vector<server> *servers = make_all_server(inputFile);
//     std::vector<int> ports;
//     for (int x = 0; x < servers->size(); x++) {
//         bool is_in = false;
//         for (size_t i = 0; i < ports.size(); i++)
//         {
//             if (ports[i] == (*servers)[x].port)
//                 is_in = true;
//         }
//         if (!is_in)
//             ports.push_back((*servers)[x].port);
//     }
//     for (int i = 0; i <= servers->size(); i++) {
//         (*servers)[i]._socket.sin_family = AF_INET;
//         (*servers)[i]._socket.sin_addr.s_addr = INADDR_ANY;
//         (*servers)[i]._socket.sin_port = htons(ports[i]);
//         std::cout << ports[i] << std::endl;
//         if (((*servers)[i]._fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
//             perror("socket failed");
//             exit(EXIT_FAILURE);
//         }
//         std::cout << "server " << i << "'s fd = " << (*servers)[i]._fd << std::endl;
//         std::cout << "server " << i << "'s _socket = " << (*servers)[i]._socket.sin_addr.s_addr << std::endl;
//         std::cout << "first checkpoint" << std::endl;
//         if (bind((*servers)[i]._fd, (struct sockaddr *)&(*servers)[i]._socket, sizeof((*servers)[i]._socket)) < 0) {
//             perror("bind failed");
//             close((*servers)[i]._fd);
//             exit(EXIT_FAILURE);
//         }
//         std::cout << "second checkpoint" << std::endl;
//         if (listen((*servers)[i]._fd, BACKLOG) < 0) {
//             perror("listen");
//             close((*servers)[i]._fd);
//             exit(EXIT_FAILURE);
//         }
//         std::cout << "finished server " << i << std::endl;
//     }


//     std::string line;
//     std::string content;
//     while (getline(inputFile, line)) {
//         content += line;
//     }
//     inputFile.close();
//     return (content);
// }

// GET, POST, DELETE, (HEADER)
// use poll, epoll if need separate thread to monitor
int main() {
    // std::string config_content = read_config();
	ServerSocket	own_socket(AF_INET, SOCK_STREAM, 0, PORT, INADDR_ANY);
    long new_socket;
    char buffer[BUFFER_SIZE] = {0};
    const char *hello_message = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "<h1>Hello</h1>";

    std::cout << "Server listening on port " << PORT << std::endl;
	u_long addrlen = sizeof(own_socket.getAddr());
    while (true) {
        if ((new_socket = accept(own_socket.getSocketFd(), (struct sockaddr *)&own_socket.getAddr(), (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            close(own_socket.getSocketFd());
            exit(EXIT_FAILURE);
        }

        read(new_socket, buffer, BUFFER_SIZE);
        std::cout << "Received request:\n" << buffer << std::endl;
        
        send(new_socket, hello_message, strlen(hello_message), 0);
        std::cout << "Hello message sent\n";
        
        close(new_socket);
    }

    close(own_socket.getSocketFd());
    return (0);
}
