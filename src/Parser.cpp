#include "../include/Webserver.hpp"

Parser::Parser() : m_serverName("default"), m_localHost("default"), m_root("default"), m_mainPage("default")
{
    std::cout <<"Default Parser constructor called." << std::endl;
}

Parser::Parser(const Parser& obj)
{
    std::cout << "Copy Parser constructor called." << std::endl;

    *this = obj;
}

Parser& Parser::operator=(const Parser& obj)
{
    if (this != &obj)
    {
        this->m_serverName = obj.m_serverName;
        this->m_localHost = obj.m_localHost;
        this->m_root = obj.m_root;
        this->m_mainPage = obj.m_mainPage;
    }
    std::cout << "Copy Parser assignment operator called." << std::endl;
    return (*this);
}

Parser::~Parser()
{
    std::cout << RED << "Destructor Parser called." << RESET << std::endl;
}

void Parser::webserver(char **argv)
{
    std::string  file = argv[1];
    std::string  buffer; 

    std::ifstream fileToRead(file); //read from the text file 
    if (!fileToRead) //if the reading fails
    {
        std::cerr << "Error: Unable to open the input file." << std::endl;
        return;
    }
    while (getline(fileToRead, buffer)) //read all the file lines and write into buffer.
    {
        std::cout << buffer << std::endl;
    }
    fileToRead.close();
}