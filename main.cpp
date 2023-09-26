#include <iostream>
#include <sstream>
#include <fstream>
#include "xml2json.hpp"
#include <nlohmann/json.hpp>

#include "UtoTcTranslator.hpp"

using json = nlohmann::json;

#define STRING(x) #x
#define XSTRING(x) STRING(x)


int main() {
    // Conventions to follow:
    // In UPPAAL you have to mark final  states by giving them a color
    // Write the invariants like x<2&&y==1
    // Write the guards like x<2&&y>1
    int idTA = 0;
    std::string currentDirPath = XSTRING(SOURCE_ROOT);

    try {
        for (const auto &entry: std::filesystem::directory_iterator(currentDirPath + "/inputFiles")) {
            if (std::filesystem::is_regular_file(entry)) {
                std::ifstream file(entry.path());

                if (file.is_open()) {
                    std::stringstream buffer;
                    buffer << file.rdbuf();
                    file.close();

                    std::string tmp = buffer.str();
                    const char *xml_str = tmp.c_str();

                    std::string nameTA = "ta" + std::to_string(idTA);
                    std::string outputFileName = nameTA + "_out.txt";

                    Translator translator(currentDirPath + "/outputFiles/" += outputFileName);
                    translator.translateTA(nameTA, json::parse(xml2json(xml_str)));

                    idTA++;
                } else {
                    std::cerr << "Failed to open file: " << entry.path() << std::endl;
                }
            }
        }
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error while reading directory: " << e.what() << std::endl;
    }

    return 0;
}
