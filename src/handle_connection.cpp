#include "../lib/includes/webserv.h"
#include <unistd.h>
// #include "Server.h"
#include "fcntl.h"
// # include "Parser.h"
#include "running_servers.h"
#include <filesystem>

UserRequestInfo extract_from_buffer(char *buffer);
std::string get_error_response(int code);
std::string make_autoindex_body(std::string root, std::string path, std::string cur_url);


int match_against_config_domains(running_server* server, UserRequestInfo req) {
    // doesn't feel too right
    for (size_t i = 0; i < server->subdomain.size(); i++) {
        std::cout << server->subdomain[i].name << "|" << (req.domain) << std::endl; 
        // if (server->subdomain[i].name.size() > req.domain.size() - 1
        //     && server->subdomain[i].name.size() < req.domain.size() + 3) {
        if (server->subdomain[i].name.find(req.domain) != std::string::npos)
            return (i);
        // }
    }
    return (-1);
}

struct config_path_search {
    int config_index;
    std::string surplus;
};

config_path_search    match_against_config_path(server &server, UserRequestInfo &req) {
    size_t cur_size_read = 0;
    size_t total_size_read = 0;
    config_path_search to_return;
    if (req.subdomains.empty()) {
        for (size_t i = 0; i < server.loc_method.size(); i++) {
            std::cout << server.loc_method[i].path << "|\n";
            // I hate that it works but well
            if (server.loc_method[i].path == "/ ") {
                to_return.config_index = i;
                return (to_return);
            }
        }
        to_return.config_index = -1;
        return (to_return);
    }
    size_t j;
    for (size_t i = 0; i < server.loc_method.size(); i++) {

        cur_size_read = 0;
        total_size_read = 0;
        std::vector<std::string> split_loc_meth = my_strsplit(server.loc_method[i].path, '/');
        for (j = 0; j < req.subdomains.size() && j < split_loc_meth.size(); j++) {
            std::cout << req.subdomains[j] << "|" << split_loc_meth[j] << "|\n"; 
            if (req.subdomains[j].compare(split_loc_meth[j]) != 0)
                break ;
        }
        if (j == split_loc_meth.size()) {
            if (j < req.subdomains.size()) {
                for (size_t j_temp = j; j_temp < req.subdomains.size(); j_temp++) {
                    to_return.surplus.append(req.subdomains[j_temp]);
                    to_return.surplus.append("/");
                }
            }
            to_return.config_index = i;
            return (to_return);
        }
    }
    for (j = 0; j < server.loc_method.size(); j++) {
        std::cout << server.loc_method[j].path << "|\n";
        // I hate that it works but well
        if (server.loc_method[j].path == "/ ") {
            break ;
        }
    }
    if (j != server.loc_method.size()) {
        for (size_t j_temp = 0; j_temp < req.subdomains.size(); j_temp++) {
            to_return.surplus.append(req.subdomains[j_temp]);
            to_return.surplus.append("/");
        }
        to_return.config_index = j;
        return (to_return);
    }
    to_return.config_index = -1;
    return (to_return);





    // all after this is the old version
    for (size_t i = 0; i < server.loc_method.size(); i++)
    {
        cur_size_read = 0;
        total_size_read = 0;
        for (j = 0; j < req.subdomains.size(); j++) {
            cur_size_read = server.loc_method[i].path.find(req.subdomains[j]);
            if (cur_size_read == std::string::npos) {
                break ; // super break
            }
            total_size_read = cur_size_read + req.subdomains[j].size();
        }
        if (total_size_read == server.loc_method[i].path.size()) {
            std::cout << j << " = j ; req.subdomains.size= " << req.subdomains.size() << std::endl;
            std::cout << req.subdomains[0];
            if (j < req.subdomains.size()) {
                for (size_t j_temp = j; j_temp < req.subdomains.size(); j_temp++) {
                    to_return.surplus.append(req.subdomains[j_temp]);
                    to_return.surplus.append("/");
                }
            }
            to_return.config_index = i;
            return (to_return);
        }
    }
    // ! this so that he check the root after trying everything else (need to be better later)
    req.subdomains.insert(req.subdomains.begin(), "/");
    for (size_t i = 0; i < server.loc_method.size(); i++)
    {
        cur_size_read = 0;
        total_size_read = 0;
        for (j = 0; j < req.subdomains.size(); j++) {
            cur_size_read = server.loc_method[i].path.find(req.subdomains[j]);
            if (cur_size_read == std::string::npos) {
                break ; // super break
            }
            total_size_read = cur_size_read + req.subdomains[j].size();
        }
        if (total_size_read == server.loc_method[i].path.size()) {
            std::cout << j << " = j ; req.subdomains.size= " << req.subdomains.size() << std::endl;
            std::cout << req.subdomains[0];
            if (j < req.subdomains.size()) {
                for (size_t j_temp = j; j_temp < req.subdomains.size(); j_temp++) {
                    to_return.surplus.append(req.subdomains[j_temp]);
                    to_return.surplus.append("/");
                }
            }
            to_return.config_index = i;
            return (to_return);
        }
    }
    to_return.config_index = -1;
    return (to_return);
}

int is_method_allowed(UserRequestInfo &user_request, method_path_option &cur_path) {
    for (int method = GET; method <= HEADER; method++) {
        std::cout << cur_path.method_type_allowed[static_cast<method_type>(method)] << std::endl;
    }
    for (int method = GET; method <= HEADER; method++) {
        if (user_request.methods_asked[static_cast<method_type>(method)] == true) {
            if (cur_path.method_type_allowed[static_cast<method_type>(method)]) {
                // method allowed
                std::cout << (method) << std::endl;
                return (0);
            }
            else {
                // error (method) not allowed
                return (1);
            }
        }
    }
    // error method asked don't exist
    return (-1);
}

std::string make_header_response(int status_code, int content_type, int content_lenght) {
    std::string header = "HTTP/1.1";
    if (status_code == 200) {
        header.append(" 200 OK\r\n");
    }
    else {
        header.append(" " + std::to_string(status_code) +" OK\r\n");
        // return (get_error_response(status_code));
    }
    if (content_type == 0) {
        header.append("Content-Type: text/html\r\n");
    }
    std::stringstream buffer;
    buffer << "Content-Length: " << content_lenght << "\r\n\r\n";
    header.append(buffer.str());
    return header;
}

std::string handle_single_connetion(UserRequestInfo &user_request, method_path_option &cur_path, std::string &root, std::string html_page) {
    std::string response;
    std::string path = root + "/" + html_page; 
    std::fstream index(path);
    // std::cout <<"|" <<path<<"|" << std::endl;
    std::string content_responce;
    std::string line;
    if (!index.is_open()) {
        response = get_error_response(684);
        return response;
    }
    while (std::getline(index, line)) {
        content_responce.append(line);
    }
    response = make_header_response(200, 0, content_responce.size());
    response.append(content_responce);
    return response;
}

std::string handle_single_connection_no_subdomain(UserRequestInfo &user_request, std::string &root, std::string html_page) {
    std::string response;
    std::cout <<"|" << html_page<<"| root before" << std::endl;
    std::string path = root + "/" + html_page; 
    std::fstream index(path);
    std::cout <<"|" <<path<<"|" << std::endl;
    std::string content_responce;
    std::string line;
    if (!index.is_open()) {
        response = get_error_response(685);
        return response;
    }
    while (std::getline(index, line)) {
        content_responce.append(line);
    }
    response = make_header_response(200, 0, content_responce.size());
    response.append(content_responce);
    return response;
}

std::string handle_single_redirection(int ret_val, UserRequestInfo &user_request, method_path_option &cur_path, std::string &redirection) {
    std::string response;
    // if (is_method_allowed(user_request, cur_path) != 0) {
    //     response = get_error_response(366);
    //     return response;
    // }
    std::string temp = "HTTP/1.1 ";
    temp.append(std::to_string(ret_val));
    temp.append(" Moved Permanently\r\nLocation: ");
    // 301 "Moved Permanently\r\nLocation: ";
    temp.append(redirection);
    // temp.append("\r\n");
    std::cout << temp << "|\n";
    // response = make_header_response(301, 0, temp.size());
    // response.append(temp);
    return (temp);
}


std::vector<std::string> get_all_server_files(std::string root) {
    std::vector<std::string> vec;
    for (const auto & entry : std::filesystem::directory_iterator(root)) {
        // std::cout << entry.path() << std::endl;
        vec.push_back(entry.path().filename().string());
    }
    return vec;
}

std::string check_server_root_files(server &server, UserRequestInfo &user_request) {
    std::vector<std::string> vec = get_all_server_files(server.root);
    for (auto &file: vec) {
        if (file == user_request.subdomains.back()) {
            return file;
            // if (server.root.back() == '/')
            //     return (server.root + file);
            // else 
            //     return (server.root + "/" + file);
        }
    }
    return ("NULL");
}

bool is_surplus_valid_file(std::string sur_plus, std::string root) {
    return false;
}

void handle_get_request(int client_fd, server &server, UserRequestInfo &user_request) {
    std::string response;
    std::cout << "match_against_config_path\n";
    config_path_search config_parsed = match_against_config_path(server, user_request);
    int config_path_index = config_parsed.config_index;
    std::cout << "\n\n" << config_parsed.surplus <<"\n\n";
    if (config_path_index == -1) {
        std::cout << "match_against_config_path failed 1\n";
        // std::string temp = check_server_root_files(server, user_request);
        // if (temp == "NULL")
        //     response = get_error_response(903);
        // else {
        //     std::cout << temp << std::endl;
        //     response = handle_single_connection_no_subdomain(user_request,
        //             server.root, temp);
        // }
        response = get_error_response(903);
    }
    // never happens now I think;
    else if (config_path_index == -2) {
        if (server.root.empty()) {
            response = get_error_response(973);
        } 
        else if (!server.redirect.empty)
            response = handle_single_redirection(
                    server.redirect.retValue,
                    user_request, 
                    server.loc_method[config_path_index],
                    server.redirect.path);
        else {
            response = handle_single_connection_no_subdomain(user_request,
                    server.root, server.index);
            // send(client_fd, response.data(), response.size(), 0);
        }
    }
    else if (!server.loc_method[config_path_index].path.empty()) {
        // std::cout << "path in sub is |" << server->subdomain[config_server_index].loc_method[config_path_index].path << "|\n";  
        // const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 13\r\n\r\n<h1>Google.com</h1>";
        if (!server.loc_method[config_path_index].redirection.empty) {
            response = handle_single_redirection(
                    server.loc_method[config_path_index].redirection.retValue,
                    user_request,
                    server.loc_method[config_path_index],
                    server.loc_method[config_path_index].redirection.path);
        } else if (!server.redirect.empty)
            response = handle_single_redirection(
                    server.redirect.retValue,
                    user_request, 
                    server.loc_method[config_path_index],
                    server.redirect.path);
        else if (is_method_allowed(user_request, server.loc_method[config_path_index]) != 0) {
            std::cout << "match_against_config_domains failed 2" << std::endl;
            std::string response = get_error_response(366);
        } else if (!server.index.empty())
            response = handle_single_connetion(user_request, 
                    server.loc_method[config_path_index],
                    server.root,
                    server.index);
        else if (is_surplus_valid_file(config_parsed.surplus, server.root)) {
            response = get_error_response(708);
        }
        else if (server.loc_method[config_path_index].autoindex) {
            std::cout << server.loc_method[config_path_index].autoindex << "  "<< server.name << std::endl;
            std::string cur_url = std::to_string(server.port) + "/" + server.name + "/" + server.loc_method[config_path_index].path;
            std::string temp = make_autoindex_body(server.root, config_parsed.surplus, cur_url);
            response = make_header_response(200, 0, temp.size());
            response.append(temp);
        } else 
            response = get_error_response(698);
    }
    // std::cout << response << "|" << std::endl;
    send(client_fd, response.data(), response.size(), 0);
    std::cout << "\n\n\n";
}

void handle_post_request(int client_fd, server &server, UserRequestInfo &user_request) {
    std::string temp = "<h1>Post resquest Denied</h1>";
    std::string response = make_header_response(403, 0, temp.size());
    response.append(temp);
    send(client_fd, response.data(), response.size(), 0);
}

void handle_del_request(int client_fd, server &server, UserRequestInfo &user_request) {
    std::string temp = "<h1>Delete resquest Denied</h1>";
    std::string response = make_header_response(403, 0, temp.size());
    response.append(temp);
    send(client_fd, response.data(), response.size(), 0);
}

void handle_connection(int client_fd, running_server* server) {
    // char buffer[BUFFER_SIZE] = "\0";
    // size_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
    // // read(new_socket, buffer, BUFFER_SIZE);
    UserRequestInfo user_request;

    char buffer[BUFFER_SIZE] = "\0";
    // memset(buffer, 0, BUFFER_SIZE);
    // * check body size| compare  with buffer size (MAX BUFFER)
    std::cout << client_fd << std::endl;
    ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
    buffer[bytes_read] = '\0';

    if (bytes_read < 0) {
        perror("recv");
        // close(client_fd);
        return;
    }
    if (bytes_read == 0) {
        std::cout << "empty buffer" << std::endl;
        // close(client_fd);
        return;
    }
    std::cout << "buffer is = (" << buffer << ")" << std::endl;
    user_request = extract_from_buffer(buffer);
    // for (auto temp : user_request.subdomains)
    //     std::cout << temp << std::endl;
    int config_server_index = match_against_config_domains(server, user_request);
    if (config_server_index == -1) {
        std::cout << "match_against_config_domains failed 3\n";
        std::string response = get_error_response(943);
        send(client_fd, response.data(), response.size(), 0);
        return ;
    }
    if (user_request.methods_asked[GET]) {
        handle_get_request(client_fd, server->subdomain[config_server_index], user_request);
    }
    else if (user_request.methods_asked[POST]) {
        if (user_request.body.find("DELETE") != std::string::npos) {
            handle_del_request(client_fd, server->subdomain[config_server_index], user_request);
        }
        else
            handle_post_request(client_fd, server->subdomain[config_server_index], user_request);
    } else {
        std::cout << "Unknown method sent\n";
        std::string response = get_error_response(353);
        send(client_fd, response.data(), response.size(), 0);
        return ;
    }
    
    // ! the random segfault happens afater this point 

    // std::cout << bytes_read << " = bytes_read |" << buffer << "| = buffer and BUFFER_SIZE = " << BUFFER_SIZE << std::endl; 
    // if (bytes_read < BUFFER_SIZE) {
    //     std::cout << "did go in if \n";
    //     const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 13\r\n\r\n<h1>PPOOPP</h1>";
    //     send(client_fd, response, strlen(response), 0);
    // }

    close(client_fd);
}
