#pragma once

#ifndef PARSER_H
# define PARSER_H
#include "../lib/includes/webserv.h"
# include "Socket.h"


struct method_path_option {
    public:
        std::string path;
        std::string alias;
        bool autoindex;
        std::map<method_type, bool> method_type_allowed;
};

struct server
{
    // std::map<long, std::string>    requests;
    // sockaddr_in    addr;
    // long        fd;
    // long        newSocket;
    // int            addrLen;
    int port;
    std::string name;
    std::string root;
    std::string uploads_dir;
    bool autoindex;
    std::map<int, std::string> error_pages;
    std::string index;
    std::string access_log;
    std::string error_log;
    std::vector<method_path_option> loc_method;
    int _fd;
    sockaddr_in _socket;
};

struct Parse
{
    std::map<std::string, std::string> basic;
    std::vector<Parse *> servers;
    std::string loc_name;
};


struct running_serveurs
{
    ServerSocket *_socket;
    // long fd;
    // sockaddr_in _socket;
    std::vector<server> mini_server;
    int socke_size;
};


// std::vector<server> *make_all_server(std::ifstream &fileToRead);

#endif
