#include "../lib/includes/webserv.h"
#include <unistd.h>
// #include "Server.h"
#include "fcntl.h"
// # include "Parser.h"
#include "running_servers.h"
#include <filesystem>
#include <sstream>
#include <string>
// #include "Server.h"
// # include "Parser.h"
#include <chrono>
#include "Request.h"

#include <unistd.h>


UserRequestInfo extract_from_buffer(char *buffer);
std::string get_error_response(int code);
std::string make_autoindex_body(std::string root, std::string path, std::string cur_url);
std::string make_header_response(int code_num, method_type method_type, std::string surplus, size_t size);
std::string identifyContentType(std::string s);


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
    std::cout << "test\n";
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
    std::cout << "test\n";

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
        response = get_error_response(684);
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
        response = get_error_response(685);
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
    if (sur_plus.empty())
        return false;
    sur_plus.pop_back();
    std::cout << sur_plus << "|\n";
    if (identifyContentType(sur_plus) != "error")
        return true;
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
                response = get_error_response(708);
        }
        else if (server.loc_method[config_path_index].autoindex) {
            // std::cout << server.loc_method[config_path_index].autoindex << "  "<< server.name << std::endl;
            std::string cur_url = std::to_string(server.port) + "/" + server.name + "/" + server.loc_method[config_path_index].path;
            std::string temp = make_autoindex_body(server.root, config_parsed.surplus, cur_url);
            response = make_header_response(200, GET, "autoindex.html", temp.size());
            response.append(temp);
        } else 
            response = get_error_response(698);
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

    std::vector<std::string> args;
    if (end_script_name != std::string::npos && end_script_name + 1 < cgi_asked.size()) {
        std::string query_string = cgi_asked.substr(end_script_name + 1);
        args = my_strsplit(query_string, '&');
    }

    // Allocate argv
    char** argv = new char*[3 + args.size()];
    
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

std::map<std::string, std::string> make_env(server &server, char** exe_arg, UserRequestInfo &user_request) {
    std::map<std::string, std::string> env;
    env["SERVER_SOFTWARE"] = "webserver/1.1";
    env["GATEWAY_INTERFACE"] = "CGI/1.1";
    env["REDIRECT_STATUS"] = "1";
    // env["SERVER_PROTOCOL"] = ;
    env["SERVER_PORT"] = std::to_string(server.port);
    env["REQUEST_METHOD"] = "webserver/1.1";
    env["PATH_INFO"] = exe_arg[1];
    env["PATH_TRANSLATED"] = exe_arg[1];
    
    size_t end_script_name = user_request.subdomains.back().find('?');
    env["QUERY_STRING"] = user_request.subdomains.back().substr(end_script_name + 1);
    // env[""] = ;
    if (user_request.body.size() > 0)
    env["CONTENT_LENGTH"] = std::to_string(user_request.body.size());
    // if (user_request.header_content.find("content_type"))
    //     env["CONTENT_TYPE"] = ;
    return env;
}


void handle_cgi_request(int client_fd, server &server, UserRequestInfo &user_request) {
    size_t index_config_cgi = find_cgi_path(server);
    std::string response;
    std::string body;

    if (index_config_cgi == -1) {
        response = get_error_response(459);
        send(client_fd, response.data(), response.size(), 0);
        return;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        response = get_error_response(422);
        send(client_fd, response.data(), response.size(), 0);
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        response = get_error_response(459);
        send(client_fd, response.data(), response.size(), 0);
        return;
    }

    if (pid == 0) {  // Child process
        close(pipefd[0]);
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(1);
        }
        close(pipefd[1]);

        // Set up environment variables
        setenv("QUERY_STRING", user_request.subdomains.back().substr(end_script_name + 1).c_str(), 1);
        setenv("REQUEST_METHOD", "GET", 1);
        setenv("CONTENT_TYPE", "text/html; charset=UTF-8", 1);
        // setenv("CONTENT_LENGTH", std::to_string(user_request.content_length).c_str(), 1);

        char ** argv = make_arg_cgi(user_request.subdomains.back(), server.root,
                                    server.loc_method[index_config_cgi].path, 
                                    server.loc_method[index_config_cgi].cgi_path);

        // Debug: Print argv
        for (int i = 0; argv[i] != nullptr; ++i) {
            std::cerr << "argv[" << i << "]: " << argv[i] << std::endl;
        }

        execve(server.loc_method[index_config_cgi].cgi_path.c_str(), argv, NULL);
        
        // If execve returns, it's an error
        perror("execve");
        exit(1);
    } else {  // Parent process
        close(pipefd[1]);
        char buffer[1024];
        ssize_t count;

        while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[count] = '\0';
            body += buffer;
        }

        close(pipefd[0]);
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            response = make_header_response(200, GET, "python.html", body.size());
            response.append(body);
        } else {
            response = get_error_response(500);
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
    std::string temp = "<h1>Delete resquest Denied</h1>";
    std::string response = make_header_response(403, DELETE, "del.html", temp.size());
    response.append(temp);
    send(client_fd, response.data(), response.size(), 0);
}


bool end_with_py(std::string input, UserRequestInfo &user_request) {

    size_t find_arg = input.find("?");
    if (find_arg == std::string::npos)
        return false;
    std::string str = input.substr(0, find_arg);
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
    if (end_with_py(user_request.subdomains[config_server_index], user_request)) {
        handle_cgi_request(client_fd, server->subdomain[config_server_index], user_request);
    }
    else if (user_request.methods_asked[GET]) {
        handle_get_request(client_fd, server->subdomain[config_server_index], user_request);
    }
    else if (user_request.methods_asked[POST] && user_request.body.find("DELETE") != std::string::npos) {
            handle_del_request(client_fd, server->subdomain[config_server_index], user_request);
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


UserRequestInfo extract_from_buffer(char *buffer);
std::string get_error_response(int code);

std::string make_header_response(int code_num, method_type method_type, std::string surplus, size_t size) {
    std::cout << "---MAKING HEADER RESPONSE---" << std::endl;
    Request req = Request(code_num, surplus, method_type);
    // std::cout << "just made req" << std::endl;
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    std::ostringstream oss; 

    std::string code;
    // std::cout << "Making code" << std::endl;
    // if (method_type == POST)
    //     code = std::to_string(code_num) + " Created\r\n";
    // else
        code = std::to_string(code_num) + " Ok\r\n";

    // std::cout << "Making method" << std::endl;
    std::string method = req.getMethod() + "\r\n";;
    
    // std::cout << "Making date" << std::endl;
    std::string date = std::to_string(now_c) + "\r\n";

    // std::cout << "making content_type" << std::endl;
    std::string content_type = req.getContentType() + "\r\n";
    std::string message = req.getMessage();
    oss << "HTTP/1.1 ";
    // add if statement to decide if code Ok or Err
    oss << code;

    // oss << date;
    oss << "Content-Length: ";
    oss << content_type;
    oss << "Connection close" << "\r\n";
    oss << "\r\n";

    // req.setResponse(oss.str());
    // std::cout << "(" << oss.str() << ")";
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