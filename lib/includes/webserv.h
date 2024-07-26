#ifndef WEBSERV_H
# define WEBSERV_H

// CPP Include
# include <iostream>
# include <iomanip>
# include <sstream>
# include <fstream>
# include <string>
# include <limits>
# include <cstdio>

// CPP Containers
# include <map>
# include <set>
# include <vector>
# include <algorithm>
# include <iterator>
# include <list>
# include <utility>

// C System
# include <sys/types.h>
# include <sys/wait.h>
# include <sys/stat.h>
# include <sys/time.h>

// C Network
# include <sys/socket.h>
# include <sys/select.h>
# include <arpa/inet.h>
# include <netinet/in.h>

# include <poll.h>
# include <sys/epoll.h>



enum method_type {
    GET,
    POST,
    DELETE,
    HEADER,
};

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

std::vector<server> *make_all_server(std::ifstream &fileToRead);

struct running_serveurs
{
    long fd;
    sockaddr_in _socket;
    std::vector<server> mini_server;
    int socke_size;
};


std::vector<std::string> my_strsplit(std::string src, char delemiter);

enum REQUEST_INFO {
Method,
Domane_Name,
Transfer_Protocol,
Host,
User_Agent,
Accept,
Accept_Language,
Accept_Encoding,
Connection,
Upgrade_Insecure_Requests,
Sec_Fetch_Dest,
Sec_Fetch_Mode,
Sec_Fetch_Site,
Sec_Fetch_User,
Priority,
};


# define DEFAULT_CONFIG ".src/config/default.conf"

#endif