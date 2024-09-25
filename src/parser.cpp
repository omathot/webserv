#include "../lib/includes/webserv.h"
// #include "Parser.h"

#include "ConfigComponent.h"
#include <string>

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


std::size_t find_true_start(std::string line) {
    std::size_t i = 0;
    while (isspace(line[i]))
        i++;
    return i;
}

std::size_t find_true_space(std::string line) {
    size_t start =  find_true_start(line);
    std::size_t i = start;
    while (!isspace(line[i]))
        i++;
    i -= start;
    return i;
}

const std::string loc_triger = "location";

Parse *make_parse(std::ifstream &fileToRead) {
    Parse *my_parse = new Parse;
    std::string buffer;
    std::string no_commants;
    std::string key_word;
    std::size_t space_loc;
    size_t true_start;
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
            (*my_parse).servers.push_back(make_parse(fileToRead)); // make a new child parse and added it to the vector
            if (no_commants.find(loc_triger) != std::string::npos) {
            size_t start = no_commants.find(loc_triger) + loc_triger.size();
            size_t len = start - no_commants.find('{');
                (*my_parse).servers.back()->loc_name =
                    no_commants.substr(start, len);
            }
        }
        else if (no_commants.find(';')!= std::string::npos) {
            true_start = find_true_start(no_commants);
            space_loc = find_true_space(no_commants);
            key_word = no_commants.substr(true_start, space_loc);
            if ( (*my_parse).basic.find(key_word) != (*my_parse).basic.end()) { // this is to see if it is already an existant key
                (*my_parse).basic[key_word].append(no_commants.substr(space_loc + true_start, no_commants.length())); // if the key already exist I append the new value to the old one with the ;
            }
            else {
                (*my_parse).basic[key_word] = no_commants.substr(space_loc + true_start, no_commants.length());
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
    for (std::map<std::string, std::string>::const_iterator it = (*my_parse).basic.cbegin(); it != (*my_parse).basic.cend(); it++) {
        for (int i = 0; i <=num_tab; i++) {
            std::cout << "|---|";
        }
        
        std::cout << "(" << it->first << ")"  << it->second << std::endl;
    }
    unsigned int vecSize = (*my_parse).servers.size();
    // run for loop from 0 to vecSize
    for(unsigned int i = 0; i < vecSize; i++)
    {
        if (!(*my_parse).servers[i]->loc_name.empty())
            std::cout << "location name is (" << (*my_parse).servers[i]->loc_name << ")\n";
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

const char* MethodToS(method_type v)
{
    switch (v)
    {
        case GET:   return "GET";
        case POST:   return "POST";
        case DELETE: return "DELETE";
        case HEADER: return "HEADER";
        default:      return "[Unknown OS_type]";
    }
}


std::ostream& operator<<(std::ostream& o, const std::map<method_type, bool>& s) {
    method_type temp;
    for (int i = GET; i != HEADER; i++) {
        temp = static_cast<method_type>(i); 
        if (s.find(temp)->second) {
            o << "|" << MethodToS(temp) << "|";
        }
    }
    o << std::endl;
    return o;
}

// Overloading the << operator for server struct
std::ostream& operator<<(std::ostream& o, const server& s) {
    o << "Server details:\n";
    o << "Port: " << s.port << "\n";
    o << "Name: " << s.name << "\n";
    o << "Root: " << s.root << "\n";
    o << "Uploads Dir: " << s.uploads_dir << "\n";
    o << "Autoindex: " << (s.autoindex ? "true" : "false") << "\n";
    o << "Error Pages: | ";
    for (std::map<int, std::string>::const_iterator it = s.error_pages.begin(); it != s.error_pages.end(); it++)
    {
        o << it->first << ": " << it->second << " | ";
    }
    o << std::endl;
    o << "Index: " << s.index << "\n";
    o << "Access Log: " << s.access_log << "\n";
    o << "Error Log: " << s.error_log << "\n";
    for (size_t i = 0; i < s.loc_method.size(); i++)
    {
        o << "Location Method:  path: " << s.loc_method[i].path << " alias: " << s.loc_method[i].alias << std::endl
            << "            auto index: " << s.loc_method[i].autoindex << " method: " << s.loc_method[i].method_type_allowed;
    }
    

    return o;
}

// Overloading the << operator for vector of servers
std::ostream& operator<<(std::ostream& o, const std::vector<server>* to_printf) {
    for (size_t i = 0; i < to_printf->size(); ++i) {
        o << "Server " << i + 1 << ":\n";
        o << (*to_printf)[i] << "\n";
    }
    return o;
}


std::vector<std::string> my_strsplit(std::string src, char delemiter) {
    std::vector<std::string> to_return;// = std::vector<std::string>;
    size_t start = 0;
    std::string arg;
    std::istringstream iss(src);
    int i;
    // maybe need to be better
    if (src.size() < 2) {
        return {src};
    }
    while (std::getline(iss, arg, delemiter)) {
        for (i = 0; i < arg.size(); i++) {
            if (arg[i] != delemiter)
                break;
        }
        if (i == arg.size())
            continue ;
        to_return.push_back(arg);
    }
    return to_return;
}

std::map<int, std::string> treat_error_pages(std::string all) {
    std::vector<std::string> all_config = my_strsplit(all, ';');
    std::vector<std::string> cur_config;
    std::map<int, std::string> to_return;
    int key;
    std::string value;
    for (size_t i = 0; i < all_config.size(); i++) {
        // std::cout << "doint this config error page rn (" << all_config[i] << ")\n";
        cur_config = my_strsplit(all_config[i], ' ');
        if (cur_config.size() == 2) {
            try {
                key = std::stoi(cur_config[0]);
                if (to_return.find(key) != to_return.end()) {
                    std::cout << "nooooon don't\n";
                    std::stoi("no!!!!!");
                }
                to_return[key] = cur_config[1];
            }
            catch(const std::exception& e)
            {
                std::cerr << "error_page not a num or dup: ()" << cur_config[0] <<  ") and other value is :" << cur_config[1] <<'\n';
                std::cin >> value;
            }
        } else {
            std::string template_file;
            template_file = cur_config[cur_config.size() - 1];
            size_t loc_x = template_file.find('x'); 
            if (loc_x == template_file.size()) {
                std::cerr << "error_page template doesn't have a x: " << template_file << '\n';
                std::cin >> value;
            }
            for (size_t i = 0; i < cur_config.size() - 1; i++) {
                try {
                    key = std::stoi(cur_config[i]);
                    to_return[key] =
                    template_file.substr(0, template_file.size());
                    to_return[key] = to_return[key].erase(loc_x, 1);
                    to_return[key] = to_return[key].insert(loc_x, cur_config[i]);
                }
                catch(const std::exception& e) {
                    std::cerr << "this error_page isn't a num: (" << cur_config[i] << ") with index of: " << i << '\n';
                    std::cin >> value;
                }
            }
        }
    }
    
    return to_return;
}

void trim_spaces_semi(std::string &str) {
    size_t start_trim = 0;
    size_t end_trim = 0;
    size_t i;
    for (i = 0; i < str.size(); i++) {
        if (!isspace(str[i])) {
            break ;
        }
    }
    start_trim = i;
    for (i = str.size() - 1; i > 0; i--) {
        if (!(isspace(str[i]) || str[i] == ';')) {
            break ;
        }
    }
    end_trim = i;
    str.erase(end_trim + 1, end_trim);
    str.erase(0, start_trim);
}

std::vector<method_path_option>  treat_loc_method(std::vector<Parse *> method, bool autoindex) {
    std::vector<method_path_option> to_return;
    method_path_option methot_temp;
    std::string temp;
    std::map<std::string, std::string> temp_rules;
    for (size_t i = 0; i < method.size(); i++) {
        methot_temp.autoindex = autoindex;
        if (method[i]->loc_name.empty()) {
            std::cout << "missing location\n";
            std::cin >> temp;
        }
        methot_temp.path = method[i]->loc_name;
        if (methot_temp.path[methot_temp.path.size() - 1] == '{')
            methot_temp.path.erase(methot_temp.path.size() - 1, 1);
        trim_spaces_semi(methot_temp.path);
        // std::cout << methot_temp.path << "|\n";
        if (!method[i]->basic["alias"].empty()) {
            methot_temp.alias = method[i]->basic["alias"];
        }
        else {
            methot_temp.alias.clear();
        }
        if (!method[i]->basic["autoindex"].empty()) {
            methot_temp.autoindex = method[i]->basic["autoindex"].find("on") != std::string::npos;
        }
        if (!method[i]->basic["allow"].empty()) {
            methot_temp.method_type_allowed[GET] = (method[i]->basic["allow"]).find("GET") != std::string::npos;
            methot_temp.method_type_allowed[POST] = (method[i]->basic["allow"]).find("POST") != std::string::npos;
            methot_temp.method_type_allowed[DELETE] = (method[i]->basic["allow"]).find("DELETE") != std::string::npos;
            methot_temp.method_type_allowed[HEADER] = (method[i]->basic["allow"]).find("HEADER") != std::string::npos;
        }
        else {
            methot_temp.method_type_allowed[GET] = false;
            methot_temp.method_type_allowed[POST] = false;
            methot_temp.method_type_allowed[DELETE] = false;
            methot_temp.method_type_allowed[HEADER] = false;
        }
        if (!method[i]->basic["redirect"].empty()) {
            std::cout << method[i]->basic["redirect"] << "|" << std::endl;
            // methot_temp.redirection = method[i]->basic["redirect"];
            trim_spaces_semi(method[i]->basic["redirect"]);
            
            methot_temp.redirection = build_config_component(method[i]->basic["redirect"]);
        }
        else 
            methot_temp.redirection.empty = true;
        to_return.push_back(methot_temp);
    }
    return to_return;
}

ConfigComponent build_config_component(std::string &str) {
    ConfigComponent component;
    std::string buff;
    std::istringstream stream(str);
    std::getline(stream, buff, ' ');
    // std::cout << buff << std::endl;
    try {
        component.retValue = std::stoi(buff);    
    } catch (...) {
        std::cout << "ConfigComponent could not build int" <<std::endl;
    }
    
    std::getline(stream, buff, ' ');
    // std::cout << buff << std::endl;

    component.path = buff;
    component.empty = false;
    return (component);
}

std::vector<server > *make_all_server(std::ifstream &fileToRead) {
    Parse *parser = make_parse(fileToRead);
    std::string useless;
    std::vector<server> *all_server = new std::vector<server>;
    size_t i;
    server temp;
    for (i = 0; i < parser->servers.size(); i++) {
        temp.port = std::stoi((parser->servers[i]->basic)["listen"]);
        temp.name = (parser->servers[i]->basic)["server_name"];
        temp.root = (parser->servers[i]->basic)["root"];
        trim_spaces_semi(temp.root);
        if (!(parser->servers[i]->basic)["redirect"].empty()) {
            std::cout << (parser->servers[i]->basic)["redirect"] << "|\n";
            trim_spaces_semi((parser->servers[i]->basic)["redirect"]);
            temp.redirect = build_config_component((parser->servers[i]->basic)["redirect"]);
            // std::cout << temp.redirect << "|\n";
        }
        else 
            temp.redirect.empty = true;
        temp.uploads_dir = (parser->servers[i]->basic)["uploads_dir"];
        temp.autoindex = parser->servers[i]->basic["autoindex"].find("on");
        temp.error_pages = treat_error_pages(parser->servers[i]->basic["error_page"]);
        // std::cout << "did one errorpage\n";
        // std::cin >> useless;
        temp.index = parser->servers[i]->basic["index"];
        if (!temp.index.empty())
            trim_spaces_semi(temp.index);

        temp.access_log = parser->servers[i]->basic["access_log"];
        temp.error_log = parser->servers[i]->basic["error_log"];
        temp.loc_method = treat_loc_method(parser->servers[i]->servers, temp.autoindex);
        all_server->push_back(temp);
    }
    return all_server;
}

const int BACKLOG = 10;

std::vector<std::string> splitString(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    tokens.push_back(str.substr(start));

    return tokens;
}

UserRequestInfo extract_from_buffer(char *buffer) {

    // std::istringstream iss(buffer);
	UserRequestInfo to_return;
    // std::string arg;


    // * check format of protocol
	for (auto it = to_return.methods_asked.begin(); it != to_return.methods_asked.end(); it++)
	{
		it->second = false;
	}
	// std::cout << "|buffer=|" << buffer <<  "|============|\n";
	std::vector<std::string> split_buffer =  splitString(buffer, "\r\n");
    // std::string split_buffer;
    // std::string delemiter = "\r\n";
    // std::getline(iss, split_buffer, delemiter);
	std::vector<std::string> usefull_info = my_strsplit(split_buffer[0], ' ');
	for (size_t i = 0; i < usefull_info.size(); i++)
	{
		if (usefull_info[i].find("GET") != std::string::npos) {
            std::cout << "GET request\n";
            to_return.methods_asked[method_type::GET] = true;
        }
        if (usefull_info[i].find("POST") != std::string::npos) {
            std::cout << "POST request\n";
            to_return.methods_asked[method_type::POST] = true;
        }
        // if (usefull_info[i].find("DELETE") != std::string::npos) {
        //     std::cout << "DELETE request\n";
        //     to_return.methods_asked[method_type::DELETE] = true;
        // }
        // if (usefull_info[i].find("HEADER") != std::string::npos) {
        //     std::cout << "HEADER request\n";
        //     to_return.methods_asked[method_type::HEADER] = true;
        // }
	}
    // std::cout << usefull_info[1] << std::endl; 
    to_return.subdomains = my_strsplit(usefull_info[1], '/');
	to_return.domain = to_return.subdomains[0];
    // std::string last_sub_domain_name = to_return.subdomains[to_return.subdomains.size() - 1];
    // std::cout << last_sub_domain_name << "\n";
    // if (last_sub_domain_name[last_sub_domain_name.size() - 1] == '/') {
    //     std::cout << "did remove\n";
    //     to_return.subdomains[to_return.subdomains.size() - 1].pop_back();
    // }
    to_return.subdomains.erase(to_return.subdomains.begin());
    // if (usefull_info[usefull_info.size() - 2])
    to_return.body = split_buffer.back(); 
    // std::cout << split_buffer[split_buffer.size() - 1] << "|||||||||" << std::endl;
	return (to_return);
}


