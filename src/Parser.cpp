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

const std::string& Parser::getServerName()
{
    return (this->m_serverName);
}

const std::string& Parser::getLocalHost()
{
    return (this->m_localHost);
}

const std::string& Parser::getRoot()
{
    return (this->m_root);
}

const std::string& Parser::getMainPage()
{
    return (this->m_mainPage);
}

void  Parser::setServerName(const std::string &t_serverName)
{
    this->m_serverName = t_serverName;
}
void  Parser::setLocalHost(const std::string &t_localHost)
{
    this->m_localHost = t_localHost;
}
void  Parser::setRoot(const std::string &t_root)
{
    this->m_root = t_root;
}
void  Parser::setMainPage(const std::string &t_mainPage)
{
    this->m_mainPage = t_mainPage;
}

void Parser::webserver(char **argv, Parser &parser)
{
    std::ifstream fileToRead(argv[1]); //read from the text file
    std::string  buffer;

    if (!fileToRead.good()) //if the reading fails
    {
        std::cerr << "Error: Unable to open the input file." << std::endl;
        return;
    }
    howManyServer(buffer);
    while (std::getline(fileToRead, buffer, ' ')) //read all the file lines and write into buffer.
    {
        if (buffer.compare("server_name") == 0)
        {
            if (fileToRead >> buffer) // read the next word after "server_name"
            {
                parser.setServerName(buffer);
                std::cout << "This is the server_name: " << parser.getServerName() << std::endl;
            }
        }
        else if (buffer.compare("listen") == 0)
        {
            if (fileToRead >> buffer) // read the next word after "server_name"
            {
                parser.setLocalHost(buffer);
                std::cout << "This is the local_host: " << parser.getLocalHost() << std::endl;
            }
        }
        else if (buffer.compare("root") == 0)
        {
            if (fileToRead >> buffer)
            {
                parser.setRoot(buffer);
                std::cout << "This is the root: " << parser.getRoot() << std::endl;
            }
        }
    }
    
    fileToRead.close();
}

void    howManyServer(std::ifstream &fileToRead)
{
    std::string buffer;
    size_t count = 0;

    while (std::getline(fileToRead, buffer, ' '))
    {
        if (buffer.compare("server") == 0)
            count++;
    }
    std::cout << "This is count: " << count << std::endl;
}