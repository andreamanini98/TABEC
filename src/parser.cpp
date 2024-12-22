#include <iostream>
#include <sstream>
#include <string>

#include "TAHeaders/TATileHeaders/parserOfTO/TOParser.hpp"
#include "TAHeaders/TATileHeaders/parserOfTO/TOTATileBuffer.hpp"

extern int yyparse();
extern void yy_scan_string(const char *str);

int parseInputString(const std::string &input)
{
    TATileBuffer::getInstance().initialize();
    yy_scan_string(input.c_str());
    return yyparse();
}

int main()
{
    std::string input = "\
create tile aa(5) with \
(\
initial = (0,1,2), \
input = (0,1), \
output = (3,4), \
accepting = (4,1,2,3), \
params = (param1, param2, param3), \
transitions = \
(\
((2,4),(),()),\
((1,3),(),()),\
((0,1),(),()),\
((1,4),(),())\
)\
);";
    std::cout << "Input:" << std::endl
              << input << std::endl;

    int parse_result = parseInputString(input);

    if (parse_result == 0)
    {
        std::cout << "Parsing successful" << std::endl;

        TATileBuffer::getInstance().tile()->output();
        bool is_valid = TATileBuffer::getInstance().tile()->check();
        if (is_valid)
        {
        }
        else
        {
            std::cout << "Invalid tile(did not pass checking)\n"
                      << std::endl;
            return 1;
        }
    }
    else
    {
        std::cout << "Parsing failed" << std::endl;
        return 1;
    }

    return 0;
}
