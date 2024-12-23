# include "../lib/includes/webserv.h"   

std::string make_header_response(int code_n, method_type method_type, std::string surplus, size_t size);
std::string match_code(int code_n);

void response_error(int code) {
    std::cout << code <<  " = code | responce_error\n";
}

std::string make_default_error_page(int code) {
    std::string body = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n<title>)\n";
    body.append(std::to_string(code));
    body.append(" </title>\n<style>\nbody {\n\nfont-family: Arial, sans-serif;\nbackground-color: #f4f4f4;");
    body.append("color: #333;\ntext-align: center;\npadding: 50px;\n}\nh1 {\nfont-size: 50px;\nmargin: 0;\n}\np {\n    font-size: 20px;\n}\na {\n    text-decoration: none;\n    color: #007bff;\n}\n");
    body.append("a:hover {\n           text-decoration: underline;\n       }\n   </style>\n</head>\n<body>\n   <h1>\n");    
    
    body.append(match_code(code));
    body.append("\n</body>\n</html>");
    return body;
}

std::string get_error_response(int code, method_type method_type, server *server) {
    // std::stringstream buffer;
    std::string body;
    if (server != nullptr && !server->error_pages.empty() && !server->error_pages[code].empty()) {
        std::string path = server->root + server->error_pages[code];
        std::fstream p;
        std::string line;
        p.open(path.c_str());
        if (!p.is_open()) {
            body = make_default_error_page(code);
        }
        else {
            while (std::getline(p, line)) {
                body.append(line);
            }
        }
    }
    else {
        body = make_default_error_page(code);
    }
    std::string header = make_header_response(code, method_type, "error.html", body.size());
    header.append(body);
    return (header);
}

