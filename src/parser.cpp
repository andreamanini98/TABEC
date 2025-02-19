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

#define PARSE_FAIL 0
#define PARSE_SUCCESS 1
#define BUFFER_CLEAN TOBuffer::getInstance().destroy()

// parse input string
int parseInputString(const std::string &input)
{
    TOBuffer::getInstance().initialize();
    yy_scan_string(input.c_str());
    return yyparse();
}

// handle a single formula and reset the buffer
int handleSingleFormula(const std::string &formula)
{
    int result = parseInputString(formula);

    if (!result)
    {
        // ITODO:post-process
        if (TOBuffer::getInstance().tile()->postProcess())
        {
            if (TOBuffer::getInstance().tile()->check())
            {
                return PARSE_SUCCESS;
            }
            else
                printf("check failed, ");
        }
        else
            printf("post process failed, ");
    }

    // parse failed
    return PARSE_FAIL;
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

        int result = handleSingleFormula(TOformula);
        if (result == PARSE_SUCCESS)
        {
            printf("parse success\n");
            // post process of the formula(only create statement now, so just create it)
            TOBuffer::getInstance().tile()->exportTo(stringsGetter.getOutputDirPath());
        }
        else
        {
            // post process of the formula(when failed)
            printf("parse failed\n");
            BUFFER_CLEAN;
        }
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
            std::vector<std::string> commands;
            std::string line, currentCommand;

            while (std::getline(input_file, line))
            {
                if (line.empty() || line.find("--") == 0)
                {
                    continue;
                }

                size_t start = line.find_first_not_of(" \t");
                size_t end = line.find_last_not_of(" \t");
                if (start == std::string::npos || end == std::string::npos)
                {
                    continue;
                }
                line = line.substr(start, end - start + 1);

                currentCommand += line + " ";

                if (line.back() == ';')
                {
                    commands.push_back(currentCommand);
                    currentCommand.clear();
                }
            }

            input_file.close();

            for (const std::string &command : commands)
            {
                int result = handleSingleFormula(command);

                if (result == PARSE_SUCCESS)
                {
                    std::cout << "Parse success\n";
                    TOBuffer::getInstance().tile()->exportTo(stringsGetter.getOutputDirPath());
                }
                else
                {
                    std::cout << "Parse failed\n";
                    BUFFER_CLEAN;
                }
            }
        }
    }

    return 0;
}
