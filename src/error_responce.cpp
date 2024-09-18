# include "../lib/includes/webserv.h"


void responce_error(int code) {
    std::cout << code <<  " = code | responce_error\n";
}

std::string get_error_responce(int code) {
    std::stringstream buffer;
    buffer << "<h1>ERROR CONFIG_SERVER_INDEX RETURNED: " << code << " </h1>";
    std::string message = buffer.str();
    buffer << "HTTP/1.1 404 KO\r\nContent-Type: text/html\r\nContent-Length: " << message.size() << "\r\n\r\n" << message;
    std::string response = buffer.str();
    return (response);
}

void error_response(int client_fd) {
    // * make it actualy response base on curent situation 
    std::cout << "did go in error response \n";
    std::string response = get_error_responce(404);
    send(client_fd, response.data(), response.size(), 0);
    std::cout << "finished error response \n";
    close(client_fd);
    std::cout << "finish closed fd \n";
}
