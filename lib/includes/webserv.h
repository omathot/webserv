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
# include "../../src/Parser.h"
// # include <sys/epoll.h>



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

struct user_request_info {
	public :
	std::string domain;
	std::map<method_type, bool> methods_asked;
};


# define DEFAULT_CONFIG ".src/config/default.conf"

#endif
