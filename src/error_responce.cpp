# include "../lib/includes/webserv.h"


void responce_error(int code) {
    std::cout << code <<  " = code | responce_error\n";
}


void error_response(int client_fd) {
    // * make it actualy response base on curent situation 
    std::cout << "did go in error response \n";
    const char* response = "HTTP/1.1 404 KO\r\nContent-Type: text/html\r\nContent-Length: 13\r\n\r\n<h1>ERROR CONFIG_SERVER_INDEX RETURNED </h1>";
    send(client_fd, response, strlen(response), 0);
    std::cout << "finished error response \n";
    close(client_fd);
    std::cout << "finish closed fd \n";
}
