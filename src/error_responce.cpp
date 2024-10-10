# include "../lib/includes/webserv.h"   

std::string make_header_response(int code_n, method_type method_type, std::string surplus, size_t size);

void response_error(int code) {
    std::cout << code <<  " = code | responce_error\n";
}

std::string make_default_error_page(int code) {
    std::string body = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>)\n";
    body.append(std::to_string(code));
    body.append(" Not Found</title>\n<style>\nbody {\n\nfont-family: Arial, sans-serif;\nbackground-color: #f4f4f4;");
    body.append("color: #333;\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 50px;\nmargin: 0;\n}\np {\n    font-size: 20px;\n}\na {\n    text-decoration: none;\n    color: #007bff;\n}\n");
    body.append("a:hover {\n           text-decoration: underline;\n       }\n   </style>\n</head>\n<body>\n   <h1>\n");    
        
    body.append(std::to_string(code));
    body.append("- Page Not Found</h1>\n<p>Sorry, the page you are looking for does not exist.</p>\n");
    body.append("<p><a href=\"/\">Go back to the homepage</a></p>\n</body>\n</html>");
    // std::cout << body;
    return body;
}

std::string get_error_response(int code, method_type method_type, server *server) {
    // std::stringstream buffer;
    std::string body;
    if (server != nullptr && !server->error_pages.empty() && !server->error_pages[code].empty())
        std::string path = server->root + server->error_pages[code];
        std::fstream p;
        p.open(path.c_str());
        body = server->error_pages[code];
    else {
        body = make_default_error_page(code);
    }
    std::string header = make_header_response(code, method_type, "error.html", body.size());
    header.append(body);
    return (header);
}

// void error_response(int client_fd) {
//     // * make it actualy response base on curent situation 
//     std::cout << "did go in error response \n";
//     std::string response = get_error_response(404);
//     send(client_fd, response.data(), response.size(), 0);
//     std::cout << "finished error response \n";
//     close(client_fd);
//     std::cout << "finish closed fd \n";
// }
