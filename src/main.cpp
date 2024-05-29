#include "Webserver.hpp"

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        Parser parse;

        webserver(argv, parse);
        
    }
    return (0);
}