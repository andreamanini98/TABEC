#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "defines/ANSI-color-codes.h"
#include "TAHeaders/TAChecker.hpp"
#include "utilities/PrintUtilities.hpp"
#include "utilities/CliHandler.hpp"
#include "utilities/StringsGetter.hpp"
#include "utilities/Utils.hpp"

using json = nlohmann::json;


int main(int argc, char *argv[])
{
    CliHandler cliHandler(&argc, &argv, false);
    StringsGetter stringsGetter(cliHandler);

    std::vector<DashBoardEntry> dashboardResults;
    DashBoardEntry d_entry;

    TAChecker taChecker(stringsGetter, cliHandler);

    deleteDirectoryContents(stringsGetter.getOutputDirForCheckingPath());

    try
    {
        for (const auto &entry: getEntriesInAlphabeticalOrder(stringsGetter.getOutputDirPath()))
        {
            if (std::filesystem::is_regular_file(entry))
            {

                if (static_cast<std::string>(entry.path()).find(".tck") == std::string::npos)
                    continue;

                std::string nameTA = getStringGivenPosAndToken(
                        getWordAfterLastSymbol(entry.path(), '/'),
                        '.', 0);
                d_entry.nameTA = nameTA;

                std::cout << "\n-------- " << nameTA << " --------\n";

                // We check if the TA admits a Büchi acceptance condition.
                bool isAccepting = taChecker.checkTA(nameTA);

                d_entry.emptinessResult = isAccepting;
                dashboardResults.emplace_back(d_entry);
                std::cout << std::string(21, '-') << std::endl;
            }
        }
        // At the end we print a convenient dashboard to quickly check the results.
        printDashBoard(dashboardResults, false, true);
    } catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << BHRED << "Error while reading directory: " << e.what() << reset << std::endl;
    }

    return 0;
}
