#include <iostream>
#include <sstream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "XMLtoJSONInclude/xml2json.hpp"
#include "defines/ANSI-color-codes.h"
#include "TAHeaders/UtoTcTranslator.hpp"
#include "utilities/StringsGetter.hpp"
#include "utilities/PrintUtilities.hpp"
#include "utilities/Utils.hpp"
#include "utilities/CliHandler.hpp"
#include "Structs.h"

using json = nlohmann::json;


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
    CliHandler cliHandler(&argc, &argv, false);
    StringsGetter stringsGetter(cliHandler);

    std::vector<DashBoardEntry> dashboardResults;
    DashBoardEntry d_entry;

    // An integer that will grow at each conversion leading to unique TA names.
    int idTA = 0;
    std::string nameTA = std::string();
    std::string outputFileName = std::string();

    deleteDirectoryContents(stringsGetter.getOutputDirPath());

    try {
        for (const auto &entry: std::filesystem::directory_iterator(stringsGetter.getInputDirPath())) {
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

                    printTitle(nameTA, static_cast<std::string>(entry.path()), outputFileName, stringsGetter.getOutputDirPath());

                    // We obtain the json representation of a TA from the xml generated by UPPAAL.
                    json j = json::parse(xml2json(xml_str));

                    Translator translator(stringsGetter.getOutputDirPath() + "/" += outputFileName);

                    if (cliHandler.isCmd(jsn))
                        std::cout << std::setw(4) << j << std::endl;

                    if (cliHandler.isCmd(nrt)) {
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
            dashboardResults.emplace_back(d_entry);
        }
        // At the end we print a convenient dashboard to quickly check the results.
        printDashBoard(dashboardResults, true, false);
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << BHRED << "Error while reading directory: " << e.what() << reset << std::endl;
    }

    return 0;
}
