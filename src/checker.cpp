#include <iostream>
#include <sstream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "XMLtoJSONInclude/xml2json.hpp"
#include "defines/ANSI-color-codes.h"
#include "TAHeaders/TAChecker.hpp"
#include "utilities/PrintUtilities.hpp"
#include "utilities/CliHandler.hpp"
#include "utilities/StringsGetter.hpp"
#include "utilities/Utils.hpp"


int main(int argc, char *argv[]) {
    CliHandler cliHandler(&argc, &argv, false);
    StringsGetter stringsGetter(cliHandler);

    std::vector<DashBoardEntry> dashboardResults;
    DashBoardEntry d_entry;

    deleteDirectoryContents(stringsGetter.getOutputDirForCheckingPath());

    try {
        for (const auto &entry: std::filesystem::directory_iterator(stringsGetter.getOutputDirPath())) {
            if (std::filesystem::is_regular_file(entry)) {

                if (static_cast<std::string>(entry.path()).find(".tck") == std::string::npos)
                    continue;

                std::string outputFileName = getWordAfterLastSymbol(entry.path(), '/');
                std::string nameTA = getStringGivenPosAndToken(outputFileName, '_', 0);
                d_entry.nameTA = nameTA;

                bool isThereAnAcceptanceCondition =
                        TAChecker::checkMuGreaterThan2C(stringsGetter.getScriptsDirPath(),
                                                        stringsGetter.getOutputDirPath() + "/" += outputFileName,
                                                        stringsGetter.getOutputDirForCheckingPath() + "/gt2C_" += outputFileName,
                                                        stringsGetter.getTCheckerBinPath());
                if (isThereAnAcceptanceCondition)
                    std::cout << BHGRN << outputFileName << "'s language is not empty!\n" << reset << std::endl;
                else {
                    // TODO replace this with actual implementation of the other verification case.
                    std::cout << BHRED << outputFileName << " may be empty, just wait for a new release of utotparser!\n" << reset << std::endl;
                }

                d_entry.emptinessResult = isThereAnAcceptanceCondition;
                dashboardResults.emplace_back(d_entry);
            }
        }
        // At the end we print a convenient dashboard to quickly check the results.
        printDashBoard(dashboardResults, false, true);
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << BHRED << "Error while reading directory: " << e.what() << reset << std::endl;
    }
}
