#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

#include "utilities/Utils.hpp"
#include "utilities/CliHandler.hpp"
#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/parserOfTO/TOParser.hpp"
#include "TAHeaders/TATileHeaders/parserOfTO/TOTATileBuffer.hpp"

extern int yyparse();
extern void yy_scan_string(const char *str);

// parse input string
int parseInputString(const std::string &input)
{
    TATileBuffer::getInstance().initialize();
    yy_scan_string(input.c_str());
    return yyparse();
}

// handle a single formula and reset the buffer
int handleSingleFormula(const std::string &formula)
{
    int result = parseInputString(formula);

    if (!result)
    {
        TATileBuffer::getInstance().tile()->output();
        std::cout << "parse successful" << std::endl;
    }
    else
        std::cout << "parse failed" << std::endl;

    TATileBuffer::getInstance().destroy();
    return result;
}

int main(int argc, char *argv[])
{
    CliHandler cliHandler(&argc, &argv);
    StringsGetter stringsGetter(cliHandler);

    // must specify one of the two options
    if (!(cliHandler.isCmd(tos) ^ cliHandler.isCmd(tom)))
    {
        std::cerr << "Cannot specify both(neither) -tos and(nor) -tom options" << std::endl;
        return 1;
    }

    if (cliHandler.isCmd(tos))
    {
        // single formula (from command line)
        std::string TOformula = cliHandler.getCmdArgument(tos);

        return handleSingleFormula(TOformula);
    }
    else if (cliHandler.isCmd(tom))
    {
        // multiple formulas (from a input source file)
        std::string file_name = cliHandler.getCmdArgument(tom);
        std::string file_path = stringsGetter.getInputDirPath() + "/" + file_name;
        std::ifstream input_file(file_path);

        if (!input_file.is_open())
            std::cerr << "Error opening file: " << file_path << std::endl;
        else
        {
            std::string line;
            std::vector<std::string> lines;

            while (std::getline(input_file, line))
            {
                // Add each line to the vector
                lines.push_back(line);
            }

            for (int i = 0; i < (int)lines.size(); i++)
            {
                // handle every line of the lines
                (void)handleSingleFormula(lines[i]);
            }

            input_file.close();
        }
    }

    return 0;
}
