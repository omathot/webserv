#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define CYAN	"\033[36m"
#define GREEN	"\033[32m"
#define MAGENTA "\033[35m"
#define RED     "\033[31m"
#define YELLOW	"\033[33m"
#define RESET	"\033[0m"

class Parser
{
private:
    

public:
    std::string m_serverName; 
    std::string m_localHost;
    std::string m_root;
    std::string m_mainPage;

    Parser();
    Parser(const Parser &obj);
    Parser& operator=(const Parser& obj);
    ~Parser();
    void webserver(char **arguments);
};





// class webserver
// {
// private:

// public:
//     webserver(/* args */);
//     ~webserver();
// };

// webserver::webserver(/* args */)
// {
// }

// webserver::~webserver()
// {
// }
