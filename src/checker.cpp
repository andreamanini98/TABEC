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


int main(int argc, char *argv[]) {
    CliHandler cliHandler(&argc, &argv, false);
    StringsGetter stringsGetter(cliHandler);

    std::vector<DashBoardEntry> dashboardResults;
    DashBoardEntry d_entry;

    deleteDirectoryContents(stringsGetter.getOutputDirForCheckingPath());

    try {
        for (const auto &entry: getEntriesInAlphabeticalOrder(stringsGetter.getOutputDirPath())) {
            if (std::filesystem::is_regular_file(entry)) {

                if (static_cast<std::string>(entry.path()).find(".tck") == std::string::npos)
                    continue;

                std::string outputFileName = getWordAfterLastSymbol(entry.path(), '/');
                std::string nameTA = getStringGivenPosAndToken(outputFileName, '.', 0);
                d_entry.nameTA = nameTA;

                std::cout << "\n-------- " << nameTA << " --------\n";

                // We first try to see if the TA admits an acceptance condition with a parameter mu > 2C.
                bool isThereAnAcceptanceCondition =
                        TAChecker::checkMuGreaterThan2C(stringsGetter.getScriptsDirPath(),
                                                        stringsGetter.getOutputDirPath() + "/" += outputFileName,
                                                        stringsGetter.getOutputDirForCheckingPath() + "/gt2C_" += outputFileName,
                                                        stringsGetter.getTCheckerBinPath());
                if (isThereAnAcceptanceCondition)
                    std::cout << BHGRN << outputFileName << "'s language is not empty!" << reset << std::endl;
                else {
                    // If the previous check fails, we try to see if the TA admits an acceptance condition with a parameter mu < 2C.
                    isThereAnAcceptanceCondition =
                            TAChecker::checkMuLessThan2C(stringsGetter.getScriptsDirPath(),
                                                         stringsGetter.getOutputDirPath() + "/" += outputFileName,
                                                         stringsGetter.getOutputDirForCheckingPath() + "/lt2C_" += outputFileName,
                                                         stringsGetter.getTCheckerBinPath(),
                                                         stringsGetter.getOutputDirForCheckingPath() + "/lt2C_tmp_" += outputFileName);
                    if (isThereAnAcceptanceCondition)
                        std::cout << BHGRN << outputFileName << "'s language is not empty!" << reset << std::endl;
                    else
                        std::cout << BHRED << outputFileName << "'s language is empty!" << reset << std::endl;
                }
                d_entry.emptinessResult = isThereAnAcceptanceCondition;
                dashboardResults.emplace_back(d_entry);
                std::cout << std::string(21, '-') << std::endl;
            }
        }
        // At the end we print a convenient dashboard to quickly check the results.
        printDashBoard(dashboardResults, false, true);
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << BHRED << "Error while reading directory: " << e.what() << reset << std::endl;
    }

    return 0;
}
