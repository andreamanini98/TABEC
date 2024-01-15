#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "utilities/StringsGetter.hpp"
#include "utilities/CliHandler.hpp"
#include "TAHeaders/TATileHeaders/TATileRegExGenerator.hpp"
#include "TAHeaders/TATileHeaders/TATileRegExGeneratorStrict.hpp"
#include "Exceptions.h"

using json = nlohmann::json;


int main(int argc, char *argv[])
{
    CliHandler cliHandler(&argc, &argv);
    StringsGetter stringsGetter(cliHandler);

    TATileRegExGenerator taTileRegExGenerator;

    // TODO: I fear that the TATileRegExGenerator is too general and will fail almost every time in constructing
    //       semantically correct tiles. For this reason, i think it would be nice to create another regex generator
    //       but more controlled, letting the user choose which one to use via the command line.

    std::cout << "Starting tester.\n";

    taTileRegExGenerator.printExpansionRules();

    std::cout << "\nObtaining TA regex.\n\n";

    for (int i = 0; i < 10; i++)
    {
        std::string regEx = taTileRegExGenerator.generateRegEx();
        std::cout << "Obtained string:\n" << regEx << "\n\n";
    }

    return EXIT_SUCCESS;
}

