#include <iostream>
#include <sstream>
#include <fstream>
#include "XMLtoJSONInclude/xml2json.hpp"
#include <nlohmann/json.hpp>

#include "UtoTcTranslator.hpp"

using json = nlohmann::json;

#define STRING(x) #x
#define XSTRING(x) STRING(x)


int main(int argc, char *argv[]) {
    if (argc <= 4) {
        int idTA = 0;
        std::string currentDirPath = XSTRING(SOURCE_ROOT);
        std::string inputDirPath = (argc == 1) ? (currentDirPath + "/inputFiles") : argv[1];
        std::string outputDirPath = (argc == 1) ? (currentDirPath + "/outputFiles") : argv[2];

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
                        if (argc > 3 && !strcmp(argv[3], "-j"))
                            std::cout << std::setw(4) << j << std::endl;

                        Translator translator(outputDirPath + "/" += outputFileName);

                        if (argc > 3 && !strcmp(argv[3], "-nrt")) {
                            if (Translator::isNRT(j)) {
                                translator.translateTA(nameTA, j);
                                std::cout << "Conversion successful\n";
                                std::cout << "---------------------\n\n";
                            }
                            else {
                                std::cerr << "Error: " << entry.path() << " is not an nrtTA and thus will not be translated" << std::endl;
                                std::cout << "---------------------\n\n";
                            }
                        } else {
                            translator.translateTA(nameTA, j);
                            std::cout << "Conversion successful\n";
                            std::cout << "---------------------\n\n";
                        }
                        idTA++;
                    } else {
                        std::cerr << "Failed to open file: " << entry.path() << std::endl;
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error &e) {
            std::cerr << "Error while reading directory: " << e.what() << std::endl;
        }
    } else {
        std::cerr << "The maximum number of allowed command line arguments is 3!" << std::endl;
        return 1;
    }

    return 0;
}
