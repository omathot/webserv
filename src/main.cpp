#include "Webserver.hpp"

int main(int argc, char **argv)
{
    Parser parse;

    if (argc == 2)
        parse.webserver(argv);
    return (0);
}