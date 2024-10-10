#include "../lib/includes/webserv.h"
#include <filesystem>
#include <iomanip>
#include <chrono>

std::vector<std::string> get_all_server_files(std::string root);
std::string get_error_response(int code, method_type method_type, server *server);
std::vector<std::string> my_strsplit(std::string src, char delemiter);


bool exists_test (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

std::string file_time_to_string(const std::filesystem::file_time_type& file_time) {
    // Convert file_time to system clock time_point
    auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
        file_time - std::filesystem::file_time_type::clock::now() + 
        std::chrono::system_clock::now());

    // Convert to time_t
    std::time_t time = std::chrono::system_clock::to_time_t(sctp);

    // Convert to local time struct
    std::tm* local_time = std::localtime(&time);

    // Format as string
    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}


// Usage example
void process_directory_entry(const std::filesystem::directory_entry& entry) {
    auto file_time = entry.last_write_time();
    std::string time_str = file_time_to_string(file_time);
    std::cout << "Last write time of " << entry.path() << ": " << time_str << std::endl;
}

std::string make_hyper_link(std::string true_url, std::string file_name, std::string display) {
    std::string hyper_link = "<a href=\"";
    hyper_link.append(true_url + "/" + file_name);
    hyper_link.append("\">" + display + "</a>");
    return hyper_link;
}

std::string get_path_parrent(std::string root, std::string virtual_root, std::string true_url) {
    if (virtual_root.empty()  || virtual_root == "/")
        return (true_url);
    virtual_root.pop_back();
    size_t index_last_slash = virtual_root.rfind("/");
    virtual_root.erase(virtual_root.begin() + index_last_slash, virtual_root.end());
    return virtual_root;
}

std::string make_autoindex_body(std::string root, std::string path, std::string cur_url) {
    std::string virtual_root = "/";
    if (!path.empty())
        virtual_root.append(path);
    if (!exists_test(root + virtual_root))
    {
        std::cout << root + virtual_root << std::endl;
        return (get_error_response(500, GET, nullptr));
    }
    std::string true_url = "http://localhost:";
    true_url.append(cur_url);
    std::string body;
    // body = R"(test)";
    body = "<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\" />\n</head>\n<body>\n<h1>Index of ";
  body.append(virtual_root);
  body.append("</h1>\n<table>");
    // std::vector<std::string> all_files = get_all_server_files(root);
    // for (auto &path : all_files) {
    body.append("<tr>\n<th>Name</th>\n<th>Last modified</th>\n<th>Size</th>\n </tr>\n");
    if (!virtual_root.empty() && virtual_root != "/")
        body.append("<tr>\n<th>" + make_hyper_link(true_url, get_path_parrent(root, virtual_root, true_url), "..") +  + "</th> </tr>\n");
    for (const auto & entry : std::filesystem::directory_iterator(root + virtual_root)) {
        auto file_t_time = entry.last_write_time();
        std::string t = file_time_to_string(file_t_time);
        
        std::string temp = "<tr>\n";
        for (size_t i = 0; i < 3; i++)
        {
            
            temp.append("<th>\n");
            if (i == 0) {
                if (entry.is_directory()) {
                    temp.append(make_hyper_link(true_url + virtual_root, entry.path().filename().string(), entry.path().filename().string() + "/"));
                    // temp.append(entry.path().filename().string());
                    // temp.append("/");
                } else {
                    temp.append(make_hyper_link(true_url + virtual_root, entry.path().filename().string(), entry.path().filename().string()));
                //     temp.append(entry.path().filename().string());
                }
            } else if (i == 1) {
                temp.append(t);
                // std::ostringstream oss;
                // oss << std::put_time((entry.last_write_time().), "%d-%m-%Y %H-%M-%S");
                // temp.append(oss.str());
                // temp.append(std::format("{}", ));
            } else if (!entry.is_directory()) {
                temp.append(std::to_string(std::filesystem::file_size(entry.path())));
            }
            temp.append("</th>\n");
        }
        temp.append("</tr>\n");
        body.append(temp);
    }
    body.append("</table>\n</body>\n</html>");

    return body;
}

