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







bool is_all_space(std::string str) {
    int i = 0;
    while (str[i])
    {
        if (!isspace(str[i]))
            return false;
        i++;
    }
    return true;
}




struct Parse
{
    std::map<std::string, std::string> basic;
    std::vector<Parse *> servers;
};

std::size_t find_true_space(std::string line) {
    std::size_t i = 0;
    while (isspace(line[i]))
        i++;
    while (!isspace(line[i]))
        i++;
    return i;
}

Parse *make_parse(std::ifstream &fileToRead) {
    Parse *my_parse = new Parse;
    std::string buffer;
    std::string no_commants;
    std::string key_word;
    std::size_t space_loc;
    std::getline(fileToRead, buffer, '\n');
    while (buffer.find('}') == std::string::npos) //read all the file lines and write into buffer.
    {
        if (buffer[0] == '#') {

        }
        else if (buffer.find('#') != std::string::npos)
            no_commants = buffer.substr(0, buffer.find('#'));
        else 
            no_commants = buffer;
        if (no_commants.find('{') != std::string::npos) {
            (*my_parse).servers.push_back(make_parse(fileToRead));
        }
        else if (no_commants.find(';')!= std::string::npos) {
            space_loc = find_true_space(no_commants);
            key_word = no_commants.substr(0, space_loc);
            if ((*my_parse).basic.find(key_word) != (*my_parse).basic.end()) {
                (*my_parse).basic[key_word].append(no_commants.substr(space_loc, no_commants.length()));
            }
            else {
                (*my_parse).basic[key_word] = no_commants.substr(space_loc, no_commants.length());
            }
        } else if (no_commants.find('}')!= std::string::npos) {
            break ;
        }
        else if (!is_all_space(no_commants)) {
            std::cerr << no_commants << "    config wrong\n";
        }
        std::getline(fileToRead, buffer, '\n');
        if (fileToRead.eof())
            break ;
    }
    return my_parse;
}


void print_parse(Parse *my_parse, int num_tab) {
    for (auto it = (*my_parse).basic.cbegin(); it != (*my_parse).basic.cend(); it++) {
        for (int i = 0; i <=num_tab; i++) {
            std::cout << "  ";
        }
        
        std::cout << it->first << it->second << std::endl;
    }
    unsigned int vecSize = (*my_parse).servers.size();
    // run for loop from 0 to vecSize
    for(unsigned int i = 0; i < vecSize; i++)
    {
        print_parse((*my_parse).servers[i], num_tab + 1);
    }

    // for (Parse *i : (*my_parse).servers) {
    //     print_parse(i);
    // }
}

void    free_parse(Parse *my_parse) {
    unsigned int vecSize = (*my_parse).servers.size();
    // run for loop from 0 to vecSize
    for(unsigned int i = 0; i < vecSize; i++)
    {
        free_parse((*my_parse).servers[i]);
    }
    delete my_parse;
}




void Parser::webserver(char **argv)
{
    std::ifstream fileToRead(argv[1]); //read from the text file
    std::string  buffer;
    std::string no_commants;
    Parse *my_parse;
    if (!fileToRead.good()) //if the reading fails
    {
        std::cerr << "Error: Unable to open the input file." << std::endl;
        return;
    }
    my_parse = make_parse(fileToRead);
    // howManyServer(buffer);
    // while (std::getline(fileToRead, buffer, '\n')) //read all the file lines and write into buffer.
    // {
        // if (buffer.find('#') != std::string::npos)
        //     no_commants = buffer.substr(0, buffer.find('#'));
        // else 
        //     no_commants = buffer;
        // if (no_commants.find('{')!= std::string::npos) {
        //     //server / location
        // }
        // else if (no_commants.find(';')!= std::string::npos) {
        //     // add to map
        // }
        // else if (!is_all_space(no_commants)) {
        //     std::cerr << no_commants << "    config wrong\n";
        // }
        
    // }
    print_parse(my_parse, 0);
    free_parse(my_parse);
    
    fileToRead.close();
    return ;
}

void Parser::keyAnalisize() {

        // if (buffer.compare("server_name") == 0)
        // {
        //     if (fileToRead >> buffer) // read the next word after "server_name"
        //     {
        //         parser.setServerName(buffer);
        //         std::cout << "This is the server_name: " << parser.getServerName() << std::endl;
        //     }
        // }
        // else if (buffer.compare("listen") == 0)
        // {
        //     if (fileToRead >> buffer) // read the next word after "server_name"
        //     {
        //         parser.setLocalHost(buffer);
        //         std::cout << "This is the local_host: " << parser.getLocalHost() << std::endl;
        //     }
        // }
        // else if (buffer.compare("root") == 0)
        // {
        //     if (fileToRead >> buffer)
        //     {
        //         parser.setRoot(buffer);
        //         std::cout << "This is the root: " << parser.getRoot() << std::endl;
        //     }
        // }
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


// setter and getter



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