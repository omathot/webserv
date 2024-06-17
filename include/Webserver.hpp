#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <sys/socket.h>
#include <vector>
#include <map>

#define CYAN	"\033[36m"
#define GREEN	"\033[32m"
#define MAGENTA "\033[35m"
#define RED     "\033[31m"
#define YELLOW	"\033[33m"
#define RESET	"\033[0m"

struct Parser
{
private:
    std::string m_serverName; 
    std::string m_localHost;
    std::string m_root;
    std::string m_mainPage;
    std::map<std::string, std::string> first_draft;

public:
    Parser();
    Parser(const Parser &obj);
    Parser& operator=(const Parser& obj);
    ~Parser();

    //get functions //
    const std::string &getServerName();
    const std::string &getLocalHost();
    const std::string &getRoot();
    const std::string &getMainPage();

    //set functions //
    void  setServerName(const std::string &t_serverName);
    void  setLocalHost(const std::string &t_localHost);
    void  setRoot(const std::string &t_root);
    void  setMainPage(const std::string &t_mainPage);

    //parser functions //
    void searchValue(std::string& fileToRead); //, std::string valueToParse, std::string buffer);
    void webserver(char **arguments);
    void keyAnalisize();
};

void    howManyServer(std::string &fileToRead);