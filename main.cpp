#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "XMLtoJSONInclude/xml2json.hpp"
#include "ANSI-color-codes.h"
#include "TAHeaders/UtoTcTranslator.hpp"
#include "TAHeaders/TAChecker.hpp"
#include "printUtilities.hpp"
#include "structs.h"

using json = nlohmann::json;

#define STRING(x) #x
#define XSTRING(x) STRING(x)

// Used only in CLion.
//#define tChecker_bin "/Users/echo/Desktop/PoliPrograms/tchecker-0.8/bin"


/**
 * Function used to start the translation process from UPPAAL to tChecker syntax.
 * @param translator the translator in charge of performing the translation.
 * @param nameTA the name of the TA to translate.
 * @param inFile the input json file containing the description of the TA.
 * @param d_entry a DashBoardEntry to keep track of the translation results.
 */
void startTranslation(Translator translator, const std::string &nameTA, const json &inFile, DashBoardEntry &d_entry) {
    translator.translateTA(nameTA, inFile);
    std::cout << BHGRN << "Conversion successful" << reset << std::endl;
    d_entry.translationResult = true;
}

int main(int argc, char *argv[]) {
    // TODO find a better way to handle the command line parameters
    if (argc <= 4) {
        // An integer that will grow at each conversion leading to unique TA names.
        int idTA = 0;

        std::vector<DashBoardEntry> dashboardResults;
        DashBoardEntry d_entry;

        std::string currentDirPath = XSTRING(SOURCE_ROOT);
        std::string inputDirPath = (argc <= 2) ? (currentDirPath + "/inputFiles") : argv[1];
        std::string outputDirPath = (argc <= 2) ? (currentDirPath + "/outputFiles") : argv[2];
        std::string nameTA = std::string();
        std::string outputFileName = std::string();

        // Path to the directory containing shell scripts.
        std::string scriptsDirPath = currentDirPath + "/scriptsForChecks";
        // Path to the directory containing TA descriptions where parameters have been substituted with appropriate values.
        std::string outputDirForCheckingPath = currentDirPath + "/outputFilesForChecking";
        // Path to the bin folder of the installed tChecker tool (set during build with cmake -D).
        std::string tCheckerBinPath = XSTRING(TCHECKER_BIN);

        try {
            for (const auto &entry: std::filesystem::directory_iterator(inputDirPath)) {
                if (std::filesystem::is_regular_file(entry)) {
                    std::ifstream file(entry.path());

                    if (static_cast<std::string>(entry.path()).find(".xml") == std::string::npos)
                        continue;

                    if (file.is_open()) {
                        std::stringstream buffer;
                        buffer << file.rdbuf();
                        file.close();

                        std::string tmp = buffer.str();
                        const char *xml_str = tmp.c_str();

                        nameTA = "ta" + std::to_string(idTA);
                        outputFileName = nameTA + "_out.tck";
                        d_entry.nameTA = nameTA;

                        printTitle(nameTA, static_cast<std::string>(entry.path()), outputFileName, outputDirPath);

                        // We obtain the json representation of a TA from the xml generated by UPPAAL.
                        json j = json::parse(xml2json(xml_str));

                        if ((argc > 3 || argc == 2) && !strcmp(argv[(argc == 2) ? 1 : 3], "-j"))
                            std::cout << std::setw(4) << j << std::endl;

                        Translator translator(outputDirPath + "/" += outputFileName);

                        if ((argc > 3 || argc == 2) && !strcmp(argv[(argc == 2) ? 1 : 3], "-nrt")) {
                            if (Translator::isNRT(j)) {
                                // If -nrt option is enabled and the TA is actually a nrt, we proceed in its translation.
                                startTranslation(translator, nameTA, j, d_entry);
                            } else {
                                // If -nrt option is enabled and the TA is not a nrt, we stop its translation and raise an error.
                                std::cerr << BHRED << "Error: " << entry.path() << " is not an nrtTA and thus will not be translated" << reset << std::endl;
                                d_entry.translationResult = false;
                            }
                        } else {
                            // The normal translation (without any option enabled) is carried out.
                            startTranslation(translator, nameTA, j, d_entry);
                        }
                        idTA++;
                    } else {
                        std::cerr << BHRED << "Failed to open file: " << entry.path() << reset << std::endl;
                    }
                }

                // TODO maybe find a better place and manner to do this
                bool isThereAnAcceptanceCondition =
                        TAChecker::checkMuGreaterThan2C(scriptsDirPath,
                                                        outputDirPath + "/" += outputFileName,
                                                        outputDirForCheckingPath + "/gt2C_" += outputFileName,
                                                        tCheckerBinPath);
                if (isThereAnAcceptanceCondition)
                    std::cout << BHGRN << outputFileName << "'s language is not empty!\n" << reset;
                else {
                    // TODO replace this with actual implementation of the other verification case.
                    std::cout << BHRED << outputFileName << " may be empty, just wait for a new release of utotparser!\n" << reset;
                }

                d_entry.emptinessResult = isThereAnAcceptanceCondition;
                dashboardResults.emplace_back(d_entry);

                std::cout << std::string(21, '-') + "\n\n";
            }
            // At the end we print a convenient dashboard to quickly check the results.
            printDashBoard(dashboardResults);
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << BHRED << "Error while reading directory: " << e.what() << reset << std::endl;
        }
    } else {
        std::cerr << BHRED << "The maximum number of allowed command line arguments is 4!" << reset << std::endl;
    }

    return 0;
}
