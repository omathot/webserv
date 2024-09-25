#include "../lib/includes/webserv.h"
#include <unistd.h>
// #include "Server.h"
#include "fcntl.h"
// # include "Parser.h"
#include "running_servers.h"
#include <filesystem>

UserRequestInfo extract_from_buffer(char *buffer);
std::string get_error_response(int code);


int match_against_config_domains(running_server* server, UserRequestInfo req) {
    // doesn't feel too right
    for (size_t i = 0; i < server->subdomain.size(); i++) {
        std::cout << server->subdomain[i].name << std::endl; 
        if (server->subdomain[i].name.size() > req.domain.size() - 1
            && server->subdomain[i].name.size() < req.domain.size() + 3) {
            if (server->subdomain[i].name.find(req.domain) != std::string::npos)
                return (i);
        }
    }
    return (-1);
}



int    match_against_config_path(server &server, UserRequestInfo req) {
    size_t cur_size_read = 0;
    size_t total_size_read = 0;
    if (req.subdomains.empty()) {
        req.subdomains.push_back("/");
        // return (-2);
    }
    // for (auto &it : req.subdomains)
    // {
    //     std::cout << it << "/";
    // }
    std::cout << std::endl;
    for (size_t i = 0; i < server.loc_method.size(); i++)
    {
        cur_size_read = 0;
        total_size_read = 0;
        for (size_t j = 0; j < req.subdomains.size(); j++) {
            cur_size_read = server.loc_method[i].path.find(req.subdomains[j]);
            if (cur_size_read == std::string::npos && total_size_read + 1 != cur_size_read) {
                total_size_read = 0;
                break ; // super break
            }
            // std::cout << total_size_read <<  " == total_size_read,  cur_size_read == " << cur_size_read << ", req.subdomains[j].size() == " << req.subdomains[j].size() << "\n"; 
            total_size_read = cur_size_read + req.subdomains[j].size();
        }
        // std::cout << server.loc_method[i].path << " = path, subdomains =  " << req.subdomains[0] << "\n";  
        // std::cout << total_size_read << " = total_size_read, req.subdomains.size() = " << server.loc_method[i].path.size() << "\n"; 
        // if (total_size_read == 24) {
        //     std::cout << std::endl << server.loc_method[i].path << std::endl;
        //     for (size_t j = 0; j < req.subdomains.size(); j++) {
        //         std::cout << req.subdomains[j];
        //     }
        //     std::cout << std::endl;
        // }
        // if (total_size_read == server.loc_method[i].path.size() - 1 && server.loc_method[i].path[server.loc_method[i].path.size() - 1] == '/')
        //     total_size_read++;
        if (total_size_read == server.loc_method[i].path.size())
            return (i);
    }
    return (-1);
}

int is_method_allowed(UserRequestInfo &user_request, method_path_option &cur_path) {
    for (int method = GET; method <= HEADER; method++) {
        if (user_request.methods_asked[static_cast<method_type>(method)] == true) {
            if (cur_path.method_type_allowed[static_cast<method_type>(method)] == false) {
                // error (method) not allowed
                return (1);
            }
            else {
                // method allowed
                return (0);
            }
        }
    }
    // error method asked don't exist
    return (-1);
}

std::string make_header_responce(int status_code, int content_type, int content_lenght) {
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
    response = make_header_responce(200, 0, content_responce.size());
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
    response = make_header_responce(200, 0, content_responce.size());
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
    // response = make_header_responce(301, 0, temp.size());
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

void handle_get_request(int client_fd, server &server, UserRequestInfo &user_request) {
    std::string response;
    std::cout << "match_against_config_path\n";
    int config_path_index = match_against_config_path(server, user_request);
    if (config_path_index == -1) {
        std::cout << "match_against_config_path failed 1\n";
        std::string temp = check_server_root_files(server, user_request);
        if (temp == "NULL")
            response = get_error_response(903);
        else {
            std::cout << temp << std::endl;
            response = handle_single_connection_no_subdomain(user_request,
                    server.root, temp);
        }
    }
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
        if (is_method_allowed(user_request, server.loc_method[config_path_index]) != 0) {
            std::cout << "match_against_config_domains failed 2" << std::endl;
            std::string response = get_error_response(366);
        }
        else if (!server.loc_method[config_path_index].redirection.empty) {
            response = handle_single_redirection(
                    server.loc_method[config_path_index].redirection.retValue,
                    user_request,
                    server.loc_method[config_path_index],
                    server.loc_method[config_path_index].redirection.path);
        } else if (!server.index.empty())
            response = handle_single_connetion(user_request, 
                    server.loc_method[config_path_index],
                    server.root,
                    server.index);
        else if (!server.redirect.empty)
            response = handle_single_redirection(
                    server.redirect.retValue,
                    user_request, 
                    server.loc_method[config_path_index],
                    server.redirect.path);
        else
            response = get_error_response(698);
    }
    std::cout << response << "|" << std::endl;
    send(client_fd, response.data(), response.size(), 0);
    std::cout << "\n\n\n";
}

void handle_post_request(int client_fd, server &server, UserRequestInfo &user_request) {
    std::string temp = "<h1>Post resquest Denied</h1>";
    std::string response = make_header_responce(403, 0, temp.size());
    response.append(temp);
    send(client_fd, response.data(), response.size(), 0);
}

void handle_del_request(int client_fd, server &server, UserRequestInfo &user_request) {
    std::string temp = "<h1>Delete resquest Denied</h1>";
    std::string response = make_header_responce(403, 0, temp.size());
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
    std::cout << "buffer size is = " << bytes_read << std::endl;
    user_request = extract_from_buffer(buffer);
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
