#include "../lib/includes/webserv.h"
#include <filesystem>


std::vector<std::string> get_all_server_files(std::string root);

std::string make_autoindex_body(std::string root) {
    std::string body;
    // body = R"(test)";
    body = R"(<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8" />
  <title>Index of )";
  body.append(root);
  body.append( R"(</title>
</head>
<body>
    <table>)");
    // std::vector<std::string> all_files = get_all_server_files(root);
    // for (auto &path : all_files) {
    for (const auto & entry : std::filesystem::directory_iterator(root)) {

        std::string temp = "<tr>\n";
        for (size_t i = 0; i < 3; i++)
        {

            temp.append("<th>\n");
            if (i == 0) {
                temp.append(entry.path().filename().string());
            } else if (i == 1) {
                temp.append(entry.path().filename().string());
            } else {
                temp.append(entry.path().filename().string());
            }
            temp.append("</th>\n");
        }
        temp.append("</tr>\n");
        body.append(temp);
    }
    body.append("</table>\n</body>\n</html>");

    return body;
}

