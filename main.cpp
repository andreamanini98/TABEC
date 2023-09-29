#include <iostream>
#include <sstream>
#include <fstream>
#include <nlohmann/json.hpp>

#include "XMLtoJSONInclude/xml2json.hpp"
#include "ANSI-color-codes.h"
#include "TAHeaders/UtoTcTranslator.hpp"
#include "printUtilities.hpp"

using json = nlohmann::json;

#define STRING(x) #x
#define XSTRING(x) STRING(x)


int main(int argc, char *argv[]) {
    if (argc <= 4) {
        int idTA = 0;
        std::vector<std::pair<std::string, bool>> translationsResults;

        std::string currentDirPath = XSTRING(SOURCE_ROOT);
        std::string inputDirPath = (argc <= 2) ? (currentDirPath + "/inputFiles") : argv[1];
        std::string outputDirPath = (argc <= 2) ? (currentDirPath + "/outputFiles") : argv[2];

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

                        std::string nameTA = "ta" + std::to_string(idTA);
                        std::string outputFileName = nameTA + "_out.tck";

                        std::cout << "-------- " << nameTA << " --------\n";
                        std::cout << "Starting conversion of file: " <<
                                  std::endl << static_cast<std::string>(entry.path()) <<
                                  std::endl << "as " << outputFileName <<
                                  std::endl << "in dest directory: " << outputDirPath << std::endl;

                        json j = json::parse(xml2json(xml_str));

                        if ((argc > 3 || argc == 2) && !strcmp(argv[(argc == 2) ? 1 : 3], "-j"))
                            std::cout << std::setw(4) << j << std::endl;

                        Translator translator(outputDirPath + "/" += outputFileName);

                        if ((argc > 3 || argc == 2) && !strcmp(argv[(argc == 2) ? 1 : 3], "-nrt")) {
                            if (Translator::isNRT(j)) {
                                translator.translateTA(nameTA, j);
                                std::cout << BHGRN << "Conversion successful\n" << reset;
                                std::cout << "---------------------\n\n";
                                translationsResults.emplace_back(nameTA, true);
                            } else {
                                std::cerr << BHRED << "Error: " << entry.path() << " is not an nrtTA and thus will not be translated" << reset << std::endl;
                                std::cout << "---------------------\n\n";
                                translationsResults.emplace_back(nameTA, false);
                            }
                        } else {
                            translator.translateTA(nameTA, j);
                            std::cout << BHGRN << "Conversion successful\n" << reset;
                            std::cout << "---------------------\n\n";
                            translationsResults.emplace_back(nameTA, true);
                        }
                        idTA++;
                    } else {
                        std::cerr << BHRED << "Failed to open file: " << entry.path() << reset << std::endl;
                    }
                }
            }
            printResultsDashboard(translationsResults);
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << BHRED << "Error while reading directory: " << e.what() << reset << std::endl;
        }
    } else {
        std::cerr << BHRED << "The maximum number of allowed command line arguments is 4!" << reset << std::endl;
    }

    return 0;
}
