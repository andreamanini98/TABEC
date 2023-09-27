#include <iostream>
#include <sstream>
#include <fstream>
#include "xml2json.hpp"
#include <nlohmann/json.hpp>

#include "UtoTcTranslator.hpp"

using json = nlohmann::json;

#define STRING(x) #x
#define XSTRING(x) STRING(x)


// Conventions to follow:
// In UPPAAL you have to mark final  states by giving them a color
// Clock declarations must be written like: clock x, y, z;
// Write the invariants like: x<2&&y==1
// Write the guards like: x<2&&y>1
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

                        json j = json::parse(xml2json(xml_str));
                        if (argc > 3 && !strcmp(argv[3], "-j"))
                            std::cout << "\n" << "----- " << nameTA << " -----" << "\n" << std::setw(4) << j << "\n\n" << std::endl;

                        Translator translator(outputDirPath + "/" += outputFileName);
                        translator.translateTA(nameTA, j);

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
        std::cerr << "The maximum number of allowed command line arguments is 2!" << std::endl;
        return 1;
    }

    return 0;
}
