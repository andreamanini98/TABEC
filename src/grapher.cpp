#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "defines/ANSI-color-codes.h"
#include "TAHeaders/TADotConverter.hpp"
#include "utilities/StringsGetter.hpp"
#include "utilities/PrintUtilities.hpp"
#include "utilities/Utils.hpp"
#include "utilities/CliHandler.hpp"

using json = nlohmann::json;


int main(int argc, char *argv[]) {
    CliHandler cliHandler(&argc, &argv, false);
    StringsGetter stringsGetter(cliHandler);

    deleteDirectoryContents(stringsGetter.getOutputDOTsDirPath());
    deleteDirectoryContents(stringsGetter.getOutputPDFsDirPath());

    try {
        for (const auto &entry: getEntriesInAlphabeticalOrder(stringsGetter.getInputDirPath())) {
            if (std::filesystem::is_regular_file(entry)) {
                std::ifstream file(entry.path());

                if (static_cast<std::string>(entry.path()).find(".xml") == std::string::npos)
                    continue;

                if (file.is_open()) {
                    // Computing the name of the .dot file to subsequently translate into PDF format.
                    std::string outputFileName = getStringGivenPosAndToken(getWordAfterLastSymbol(entry.path(), '/'), '.', 0);

                    std::cout << "Starting translation from .xml to .dot of file:\n" << entry.path() << std::endl;

                    TADotConverter taDotConverter(stringsGetter.getOutputDOTsDirPath() + "/" += (outputFileName  + ".dot"));
                    try {
                        // Converting the .xml file into .dot format.
                        taDotConverter.translateTAtoDot(outputFileName, getJsonFromFileStream(file));
                    } catch (NotXMLFormatException &e) {
                        std::cerr << BHRED << e.what() << reset << std::endl;
                    }
                }
            }
            std::cout << std::endl;
        }
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << BHRED << "Error while reading directory: " << e.what() << reset << std::endl;
    }

    return 0;
}
