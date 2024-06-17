#include "Webserver.hpp"

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        Parser parse = Parser();

        parse.webserver(argv);
        
    }
    return (0);
}
