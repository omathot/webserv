#include "../lib/includes/webserv.h"
#include <fstream>
#include <locale>
#include <unistd.h>
// #include "Server.h"
#include "Parser.h"
#include "fcntl.h"
#include <cstdio>
#include <iostream>
// # include "Parser.h"
#include "running_servers.h"
#include <filesystem>
#include <sstream>
#include <string>
// #include "Server.h"
// # include "Parser.h"
#include <chrono>
#include <time.h>
#include "Request.h"

#include <unistd.h>


UserRequestInfo extract_from_buffer(std::string buffer);
std::string get_error_response(int code, method_type method_type, server *server);
std::string make_autoindex_body(std::string root, std::string path, std::string cur_url);
std::string make_header_response(int code_num, method_type method_type, std::string surplus, size_t size);
std::string identifyContentType(std::string s);
std::string methodTypeToString(method_type t);
void trim_spaces_semi(std::string &str);


int match_against_config_domains(running_server* server, UserRequestInfo req) {
    // doesn't feel too right
    for (size_t i = 0; i < server->subdomain.size(); i++) {
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

method_type is_method_ask(UserRequestInfo &user_request) {
    for (int method = GET; method <= HEADER; method++) {
        if (user_request.methods_asked[static_cast<method_type>(method)] == true)
            return (static_cast<method_type>(method));
    }
    return (GET);
}

int is_method_allowed(UserRequestInfo &user_request, method_path_option &cur_path) {
    for (int method = GET; method <= HEADER; method++) {
        std::cout << cur_path.method_type_allowed[static_cast<method_type>(method)] << std::endl;
    }
    for (int method = GET; method <= HEADER; method++) {
        if (user_request.methods_asked[static_cast<method_type>(method)] == true) {
            if (cur_path.method_type_allowed[static_cast<method_type>(method)]) {
                // method allowed
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

// std::string make_header_response(int status_code, int content_type, int content_lenght) {
//     std::string header = "HTTP/1.1";
//     if (status_code == 200) {
//         header.append(" 200 OK\r\n");
//     }
//     else {
//         header.append(" " + std::to_string(status_code) +" OK\r\n");
//         // return (get_error_response(status_code));
//     }
//     if (content_type == 0) {
//         header.append("Content-Type: text/html\r\n");
//     }
//     std::stringstream buffer;
//     buffer << "Content-Length: " << content_lenght << "\r\n\r\n";
//     header.append(buffer.str());
//     return header;
// }

std::string handle_single_connetion(UserRequestInfo &user_request, method_path_option &cur_path, std::string &root, std::string html_page) {
    std::string response;
    std::string path = root + "/" + html_page; 
    std::fstream index(path);
    // std::cout <<"|" <<path<<"|" << std::endl;
    std::string content_responce;
    std::string line;
    if (!index.is_open()) {
        //maybe need to give him server later
        response = get_error_response(404, GET, nullptr);
        return response;
    }
    while (std::getline(index, line)) {
        content_responce.append(line);
    }
    response = make_header_response(200, GET, html_page, content_responce.size());
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
        //maybe need to give him server later
        response = get_error_response(404, GET, nullptr);
        return response;
    }
    while (std::getline(index, line)) {
        content_responce.append(line);
    }
    response = make_header_response(200, GET, html_page, content_responce.size());
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
    if (sur_plus.empty())
        return false;
    sur_plus.pop_back();
    if (identifyContentType(sur_plus) != "error")
        return true;
    return false;
}

void handle_get_request(int client_fd, server &server, UserRequestInfo &user_request) {
    std::string response;
    config_path_search config_parsed = match_against_config_path(server, user_request);
    int config_path_index = config_parsed.config_index;
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
        response = get_error_response(404, GET, &server);
    }
    // never happens now I think;
    else if (config_path_index == -2) {
        if (server.root.empty()) {
            response = get_error_response(404, GET, &server);
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
            response = get_error_response(401, GET, &server);
        } else if (!server.index.empty() && config_parsed.surplus.empty())
            response = handle_single_connetion(user_request, 
                    server.loc_method[config_path_index],
                    server.root,
                    server.index);
        else if (!server.loc_method[config_path_index].index.empty() && config_parsed.surplus.empty())
            response = handle_single_connetion(user_request, 
                server.loc_method[config_path_index],
                server.root,
                server.loc_method[config_path_index].index);
        else if (is_surplus_valid_file(config_parsed.surplus, server.root)) {
            config_parsed.surplus.pop_back();
            std::string path = server.root + config_parsed.surplus;
            std::ifstream f(path);
            std::string line;
            std::string body;
            if (f.good()) {
                while (std::getline(f, line)) {
                    body.append(line);
                }
                response = make_header_response(200, GET, config_parsed.surplus, body.size());
                response.append(body);
            }
            else 
                response = get_error_response(404, GET, &server);
        }
        else if (server.loc_method[config_path_index].autoindex) {
            // std::cout << server.loc_method[config_path_index].autoindex << "  "<< server.name << std::endl;
            std::string cur_url = std::to_string(server.port) + "/" + server.name + "/" + server.loc_method[config_path_index].path;
            std::string temp = make_autoindex_body(server.root, config_parsed.surplus, cur_url);
            response = make_header_response(200, GET, "autoindex.html", temp.size());
            response.append(temp);
        } else 
            response = get_error_response(403, GET, &server);
    }
    // std::cout << response << "|" << std::endl;
    send(client_fd, response.data(), response.size(), 0);
    std::cout << "\n\n\n";
}

// void handle_post_request(int client_fd, server &server, UserRequestInfo &user_request) {
//     // size_t delemite_content_type = user_request.body.find("Content-Type");
//     // size_t delemite_filename = user_request.body.find("filename");
//     // std::cout << user_request.body << std::endl << std::endl;
//     std::string response;
//     // std::cout << delemite_filename << " = delemite_filename | delemite_content_type = " << delemite_content_type << " npos = " << std::string::npos << std::endl;
//     if (user_request.header_content["Content-Disposition"].empty()) {
//         std::string temp = "<h1>Post resquest Denied</h1>";
//         response = make_header_response(403, POST, "post.html", temp.size());
//         response.append(temp);
//     } else {
//         size_t delemite_filename = user_request.header_content["Content-Disposition"].find("filename");
//         size_t file_name_del = user_request.header_content["Content-Disposition"].find("\"", delemite_filename) + 1;
//         size_t file_name_del_end = user_request.header_content["Content-Disposition"].find("\"", file_name_del);
//         std::string filename = user_request.header_content["Content-Disposition"].substr(file_name_del, file_name_del_end);
        
//         // size_t start_of_file_content = user_request.body.find("\n", delemite_content_type) + 1;
//         std::string file_content = user_request.body;
//         std::cout << server.root + "download/" + filename;
//         std::fstream download(server.root + "download/" + filename);
//         download << file_content;
//         std::string body = "<h1>Post resquest fuffiled</h1>";
//         // response = make_header_response(201, POST, "post.html", body.size());
//         // response.append(body);
//         // std::cout << response << std::endl;
//         response = R"(HTTP/1.1 201 Created
// Content-Type: application/json
// Content-Length: 128
// Location: https://api.example.com/files/newfile.txt
// Date: Sun, 29 Sep 2024 12:00:00 GMT

// {
//   "id": "file123",
//   "filename": "newfile.txt",
//   "size": 1024,
//   "created_at": "2024-09-29T12:00:00Z",
//   "message": "File created successfully"
// })";
//     }
//     send(client_fd, response.data(), response.size(), 0);
// }

size_t find_cgi_path(server &server) {
    for (size_t i = 0; i < server.loc_method.size(); i++)
    {
        if (!server.loc_method[i].cgi_execute.empty() && !server.loc_method[i].cgi_path.empty())
            return i;
    }
    return -1;
}
void free_argv(char ** argv) {
    size_t i = 0;
    while (argv[i] != nullptr) {
        delete argv[i];
        i++;
    }
    delete [] argv;
}

char** make_arg_cgi(const std::string& cgi_asked, const std::string& root, 
                    const std::string& our_cgi_path, const std::string& cgi_path) {
    size_t end_script_name = cgi_asked.find('?');
    std::string path_to_script = root + our_cgi_path.substr(1) + "/" + 
                                 (end_script_name != std::string::npos ? 
                                  cgi_asked.substr(0, end_script_name) : cgi_asked);

    // std::vector<std::string> args;
    // if (end_script_name != std::string::npos && end_script_name + 1 < cgi_asked.size()) {
    //     std::string query_string = cgi_asked.substr(end_script_name + 1);
    //     args = my_strsplit(query_string, '&');
    // }
    // char** argv = new char*[3 + args.size()];

    // Allocate argv
    char** argv = new char*[3];
    
    // Set interpreter path
    argv[0] = new char[cgi_path.size() + 1];
    std::strcpy(argv[0], cgi_path.c_str());

    // Set script path
    argv[1] = new char[path_to_script.size() + 1];
    std::strcpy(argv[1], path_to_script.c_str());

    // Null-terminate the array
    argv[2] = nullptr;

    return argv;
}

// std::string &get_content_type_from_header_request(std::map<std::string, std::string> &header_content) {
//     std::string key_finding = "Content-Type";
//     for (size_t i = 0; i < header_content.size(); i++) {
//         if (!header_content[key_finding].empty())
//             return (header_content[key_finding])
//     }
    
// }

std::map<std::string, std::string> make_env(server &server, char** exe_arg, UserRequestInfo &user_request) {
    std::map<std::string, std::string> env;
    env["SERVER_SOFTWARE"] = "webserver/1.1";
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["REDIRECT_STATUS"] = "1";
    env["SERVER_PROTOCOL"] = "HTTP/1.1";
    env["SERVER_PORT"] = std::to_string(server.port);
    env["REQUEST_METHOD"] = methodTypeToString(is_method_ask(user_request));
    env["PATH_INFO"] = exe_arg[1];
    env["PATH_TRANSLATED"] = exe_arg[1];
    
    size_t end_script_name = user_request.subdomains.back().find('?');
    env["QUERY_STRING"] = user_request.subdomains.back().substr(end_script_name + 1);
    // env[""] = ;
    if (user_request.body.size() > 0)
        env["CONTENT_LENGTH"] = std::to_string(user_request.body.size());
    if (!user_request.header_content["Content-Type"].empty()) {
        env["CONTENT_TYPE"] = user_request.header_content["Content-Type"];
        perror(env["CONTENT_TYPE"].c_str());
        trim_spaces_semi(env["CONTENT_TYPE"]);
    }
    return env;
}

char **map_to_array(std::map<std::string, std::string> map_env) {
    char **array;
    array = new char*[map_env.size() + 1];
    size_t i = 0;
    for (const auto &it : map_env) {
        std::string all_arg = it.first + "=" + it.second;
        array[i] = new char [all_arg.size() + 1];
        std::strcpy(array[i], all_arg.c_str());
        i++;
    }
    array[i] = nullptr;
    return (array);
}

method_type get_method_type (UserRequestInfo &user_request) {
    method_type cur_method;
    if (user_request.methods_asked[POST])
        cur_method = POST;
    else if (user_request.methods_asked[DELETE])
        cur_method = DELETE;
    else
        cur_method = GET;
    return (cur_method);
}


void handle_cgi_request(int client_fd, server &server, UserRequestInfo &user_request) {
    size_t index_config_cgi = find_cgi_path(server);
    std::string response;
    std::string body;
    if (user_request.body.find("DELETE") != std::string::npos) {
        user_request.methods_asked[POST] = false;
        user_request.methods_asked[DELETE] = true;
    }

    method_type cur_method = get_method_type(user_request);
    if (index_config_cgi == -1) {
        response = get_error_response(404, cur_method, &server);
        send(client_fd, response.data(), response.size(), 0);
        return;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        response = get_error_response(500, cur_method, &server);
        send(client_fd, response.data(), response.size(), 0);
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        response = get_error_response(500, cur_method, &server);
        send(client_fd, response.data(), response.size(), 0);
        return;
    }

    if (pid == 0) {  // Child process
        if (dup2(pipefd[1], STDOUT_FILENO) == -1 || dup2(pipefd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(pipefd[1]);
        close(pipefd[0]);

        // Set up environment variables
        // setenv("QUERY_STRING", user_request.subdomains.back().substr(end_script_name + 1).c_str(), 1);
        // setenv("REQUEST_METHOD", "GET", 1);
        // setenv("CONTENT_TYPE", "text/html; charset=UTF-8", 1);
        // setenv("CONTENT_LENGTH", std::to_string(user_request.content_length).c_str(), 1);

        char ** argv = make_arg_cgi(user_request.subdomains.back(), server.root,
                                    server.loc_method[index_config_cgi].path, 
                                    server.loc_method[index_config_cgi].cgi_path);

        // Debug: Print argv
        for (int i = 0; argv[i] != nullptr; ++i) {
            std::cerr << "argv[" << i << "]: " << argv[i] << std::endl;
        }
        // char **env = nullptr;
        char **env = map_to_array(
            make_env(server, argv, user_request)
        );
        for (int i = 0; env[i] != nullptr; ++i) {
            std::cerr << "argv[" << i << "]: " << env[i] <<"|" << std::endl;
        }
        execve(server.loc_method[index_config_cgi].cgi_path.c_str(), argv, env);
        
        // If execve returns, it's an error
        // free_argv(argv);
        // free_argv(env);
        exit(1);
    } else {  // Parent process
        write(pipefd[1], user_request.body.c_str(), user_request.body.size());
        close(pipefd[1]);
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            char buffer[1024];
            ssize_t count;

            while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
                buffer[count] = '\0';
                body += buffer;
            }

            close(pipefd[0]);
            close(pipefd[1]);
            response = make_header_response(200, is_method_ask(user_request), "python.html", body.size());
            response.append(body);
        } else {
            close(pipefd[0]);
            close(pipefd[1]);
            response = get_error_response(500, cur_method, &server);
        }
    }

    send(client_fd, response.data(), response.size(), 0);
}

// void handle_cgi_request(int client_fd, server &server, UserRequestInfo &user_request) {
//     size_t index_config_cgi = find_cgi_path(server);
//     std::string response;
//     std::string body;
//     if (index_config_cgi == -1) {
//         // no python gin in here
//         response = get_error_response(459);
//     } else {
//         std::cout << "started forking" << std::endl;
//         int pipefd[2];
//         if (pipe(pipefd) == -1) {
//             perror("pipe");
//             response = get_error_response(422);
//             send(client_fd, response.data(), response.size(), 0);
//         }
//         int pid = fork(); 
//         if (pid == -1) {
//             response = get_error_response(459);
//         } else {
//             if (pid == 0) {
//                 close(pipefd[0]);
//                 if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
//                     perror("dup2");
//                     response = get_error_response(422);
//                     send(client_fd, response.data(), response.size(), 0);
//                 }
//                 char ** argv = make_arg_cgi(user_request.subdomains.back(), server.root,
//                     server.loc_method[index_config_cgi].path, server.loc_method[index_config_cgi].cgi_path);
                
                
//                 // size_t end_script_name = user_request.subdomains.back().rfind('?');
//                 // std::string path_to_script = server.root + server.loc_method[index_config_cgi].path.substr(1)
//                 //     + "/" + user_request.subdomains.back().substr(0 , end_script_name);
//                 // char * const argv[] = {const_cast<char*>(server.loc_method[index_config_cgi].cgi_path.c_str()),
//                 //         const_cast<char*>(path_to_script.c_str()), NULL};
//                 int i = 0;
//                 while (argv[i] != nullptr)
//                 {
//                     std::cout << argv[i] << std::endl;
//                     i++;
//                 }
                
//                 execve(server.loc_method[index_config_cgi].cgi_path.c_str(), argv, NULL);
//                 free_argv(argv);
//             } else {
//                 close(pipefd[1]);
//                 char buffer[1024];
//                 ssize_t count;
//                 std::string output;
//                 while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
//                     buffer[count] = '\0';
//                     body += buffer;
//                 }
//                 close(pipefd[0]);
//                 wait(nullptr);
//             }
//             response = make_header_response(200, GET, "python.html", body.size());
//             response.append(body);
//             // waitpid(pid);
//         }
//     }
//     send(client_fd, response.data(), response.size(), 0);

// }
void handle_del_request(int client_fd, server &server, UserRequestInfo &user_request) {
    std::string response;
    size_t start_idx = user_request.body.rfind('=');
    std::string filename = user_request.body.substr(start_idx + 1);
    std::string pa = server.root + "Downloads/" + filename;
    pa.erase(remove_if(pa.begin(), pa.end(), isspace), pa.end());
    std::cout << "---PA: " << pa << "---" << std::endl;
    std::ifstream f(pa);
    if (f.good() && !filename.empty()) {
        std::remove(pa.c_str());
        std::string body = "<h1>Delete resquest Successful</h1>";
        response = make_header_response(200, DELETE, "del.html", body.size());
        response.append(body);
    }
    else {
        std::string temp = "<h1>Delete resquest Denied</h1>";
        response = make_header_response(404, DELETE, "del.html", temp.size());
        response.append(temp);
    }
    send(client_fd, response.data(), response.size(), 0);
}


bool end_with_py(std::string input) {
    std::cout << "test2" << std::endl;
    if (input.empty())
        return false;
    size_t find_arg = input.find("?");
    std::string str;
    if (find_arg != std::string::npos) {
        str = input.substr(0, find_arg);
    } else {
        str = input;
    }
    if (str.back() == '/')
        str.pop_back();
    if (str.back() == 'y') {
        str.pop_back();
        if (str.back() == 'p') {
            str.pop_back();
            if (str.back() == '.')
                str.pop_back();
                return true;
        }
    }
    return false;
}

void handle_connection(int client_fd, running_server* server) {
    // char buffer[BUFFER_SIZE] = "\0";
    // size_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
    // // read(new_socket, buffer, BUFFER_SIZE);
    UserRequestInfo user_request;

    char buffer[BUFFER_SIZE] = "\0";
    // memset(buffer, 0, BUFFER_SIZE);
    // * check body size| compare  with buffer size (MAX BUFFER)
    std::string full_buffer;
    ssize_t bytes_read;
    do {
        bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
        if (bytes_read < 0) {
            throw std::runtime_error("recv error: " + std::string(strerror(errno)));
        } else if (bytes_read > 0) {
            full_buffer.append(buffer, bytes_read); // Append only the bytes read
        }
    } while (bytes_read == BUFFER_SIZE);

    // ssize_t bytes_read = BUFFER_SIZE;
    // while (bytes_read == BUFFER_SIZE)
    // {
    //     bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
    //     full_buffer.append(buffer);
    // }
    std::cout << "user request is =\n|" << full_buffer << "|\n";
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
    user_request = extract_from_buffer(full_buffer);

    int config_server_index = match_against_config_domains(server, user_request);
    if (config_server_index == -1) {
        std::cout << "match_against_config_domains failed 3\n";
        method_type cur_method = get_method_type(user_request);
        std::string response = get_error_response(404, cur_method, nullptr);
        send(client_fd, response.data(), response.size(), 0);
        return ;
    }
    std::cout << config_server_index << std::endl;
    // std::cout << "|" << user_request.subdomains[config_server_index] << "|" << std::endl;

    std::cout << "eaeogiuheuogtheougheoeuah" << config_server_index <<  std::endl;
    // std::cout << user_request.subdomains[config_server_index].empty() << std::endl;
    // std::cout << "|" << user_request.subdomains.size() << "|" << std::endl;
    // end_with_py(user_request.subdomains[config_server_index]);
    std::cout << "eaeogiuheuogtheougheoeuah" << std::endl;

    if (!user_request.subdomains.empty() &&  end_with_py(user_request.subdomains[config_server_index])) {
        handle_cgi_request(client_fd, server->subdomain[config_server_index], user_request);
    }
    else if (user_request.methods_asked[GET]) {
        std::cout << "------ GET go-----" << std::endl;
        handle_get_request(client_fd, server->subdomain[config_server_index], user_request);
    }
    else if (user_request.methods_asked[POST] && user_request.body.find("DELETE") != std::string::npos) {
        handle_del_request(client_fd, server->subdomain[config_server_index], user_request);
    } else {
        std::cout << "Unknown method sent\n";
        method_type cur_method = get_method_type(user_request);
        std::string response = get_error_response(405, cur_method, nullptr);
        std::cout << response << std::endl;
        send(client_fd, response.data(), response.size(), 0);
        // return ;
    }
    

    // std::cout << bytes_read << " = bytes_read |" << buffer << "| = buffer and BUFFER_SIZE = " << BUFFER_SIZE << std::endl; 
    // if (bytes_read < BUFFER_SIZE) {
    //     std::cout << "did go in if \n";
    //     const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: 13\r\n\r\n<h1>PPOOPP</h1>";
    //     send(client_fd, response, strlen(response), 0);
    // }

    close(client_fd);
}


UserRequestInfo extract_from_buffer(char *buffer);

std::string match_code(int code_n) {
    std::string code;
    switch (code_n) {
        case 200:
            code = std::to_string(code_n) + " Ok\r\n";
            break ;
        case 201:
            code = std::to_string(code_n) + " Created\r\n";
            break ;
        case 202:
            code = std::to_string(code_n) + " Accepted\r\n";
            break ;
        case 302:
            code = std::to_string(code_n) + " Found\r\n";
            break ;
        case 400:
            code = std::to_string(code_n) + " Bad Request\r\n";
            break ;
        case 401:
            code = std::to_string(code_n) + " Unauthorized\r\n";
            break ;
        case 403:
            code = std::to_string(code_n) + " Forbidden\r\n";
            break ;
        case 404:
            code = std::to_string(code_n) + " Not Found\r\n";
            break ;
        case 405:
            code = std::to_string(code_n) + " Method Not Allowed\r\n";
            break ;
        case 408:
            code = std::to_string(code_n) + " Request Timeout\r\n";
            break ;
        case 411:
            code = std::to_string(code_n) + " Length Required\r\n";
            break ;
        case 415:
            code = std::to_string(code_n) + " Unsupported Media Type\r\n";
            break ;
        case 418:
            code = std::to_string(code_n) + " I'm a Teapot\r\n";
            break ;
        case 429:
            code = std::to_string(code_n) + " Too Many Requests\r\n";
            break ;
        case 500:
            code = std::to_string(code_n) + " Internal Server Error\r\n";
            break ;
        default:
            code = std::to_string(code_n) + " Ok\r\n";
            break ;
    }
    return code;
}

std::string make_header_response(int code_n, method_type method_type, std::string surplus, size_t size) {
    Request req = Request(code_n, surplus, method_type);
    if (method_type == POST)
        std::cout << "---MAKING POST HEADER---" << std::endl;
    else if (method_type == DELETE)
        std::cout << "---MAKING DELETE HEADER---" << std::endl;
    else
        std::cout << "---MAKING GET HEADER---" << std::endl;
    char date_buff[20];
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss; 

    std::string code = match_code(code_n);
    std::string method = req.getMethod() + "\r\n";;
    strftime(date_buff, 20, "%d-%m-%Y %H:%M:%S", localtime(&now_c));
    std::string date = date_buff;
    std::string content_type = req.getContentType() + "\r\n";
    std::string message = req.getMessage();
    oss << "HTTP/1.1 ";
    oss << code;
    oss << "Date : ";
    oss << date << "\n"; // buffer does not have \n at the end, added here
    oss << "Content-type: ";
    oss << content_type;
    oss << "Content-length: ";
    oss << size << "\n";
    oss << "Connection close" << "\r\n";
    oss << "\r\n";
    std::cout << "---HEADER PRINTOUT HERE---" << std::endl;
    std::cout << "(" << oss.str() << ")";
    std::cout << "--- ---" << std::endl;
    return (oss.str());

}






// CGI ISSUES, MOVED TO BOTTOM
// char ** make_arg_cgi(std::string cgi_asked, std::string root, std::string our_cgi_path, std::string cgi_path) {
//     size_t end_script_name = cgi_asked.rfind('?');
//     char **argv;
//     std::string path_to_script = root + our_cgi_path.substr(1)
//         + "/" + cgi_asked.substr(0 , end_script_name);
//     if (end_script_name + 1 == cgi_asked.size()) {
//         argv = new char *[2];
//         argv[0] = new char [cgi_path.size() + 1];
//         argv[1] = new char [path_to_script.size() + 1];
//         std::strcpy(argv[0], cgi_path.c_str());
//         std::strcpy(argv[1], path_to_script.c_str());
//         // argv[0] = cgi_path.data();
//         // argv[1] = path_to_script.data();
//         argv[2] = nullptr;
//         // argv = {(),
//         //         (path_to_script.c_str()), NULL};
//     } else {
//         std::string args = cgi_asked.substr(end_script_name + 1);
//         // std::istringstream iss(args);        
//         std::vector<std::string> args_split = my_strsplit(args, '&');
//         argv = new char *[2 + args_split.size()];
//         argv[0] = new char [cgi_path.size() + 1];
//         argv[1] = new char [path_to_script.size() + 1];
//         std::strcpy(argv[0], cgi_path.c_str());
//         std::strcpy(argv[1], path_to_script.c_str());
//         size_t i;
//         for (i = 0; i < args_split.size(); i++) {
//             // std::cout << args_split[i] << std::endl;
//             argv[2 + i] = new char [args_split[i].size() + 1];
//             std::strcpy(argv[2 + i], args_split[i].c_str());
//         }
//         argv[3 + i] = NULL;
//     }
//     return argv;
// }
