#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "defines/ANSI-color-codes.h"
#include "TAHeaders/TADotConverter.hpp"
#include "utilities/StringsGetter.hpp"
#include "utilities/Utils.hpp"
#include "utilities/CliHandler.hpp"

using json = nlohmann::json;


/**
 * Function used to translate a .xml file into a .dot file, writing it into a specified output directory.
 * @param outputDirPath the output directory from which to write the resulting .dot file.
 * @param outputFileName the name of the resulting .dot file.
 * @param fileToConvert the .xml file to convert into .dot format.
*/
void convertXMLtoDOT(const std::string &outputDirPath, const std::string &outputFileName, std::ifstream &fileToConvert)
{
    TADotConverter taDotConverter(outputDirPath + "/" += (outputFileName + ".dot"));
    try
    {
        // Converting the .xml file into .dot format.
        taDotConverter.translateTAtoDot(outputFileName, getJsonFromFileStream(fileToConvert));

    } catch (NotXMLFormatException &e)
    {
        std::cerr << BHRED << e.what() << rstColor << std::endl;
    }
}


/**
 * Function used to translate all .dot files from a given directory into .pdf files into a specified output directory.
 * @param sourceDirPath the directory from which to take the .dot files to translate.
 * @param outputDirPath the directory from which to write the resulting .pdf files.
 */
void convertDOTtoPDF(const std::string &sourceDirPath, const std::string &outputDirPath)
{
    std::cout << "Starting conversion from .dot to .pdf file format." << std::endl;
    std::cout << std::string(50, '-') << std::endl;

    for (const auto &entry: getEntriesInAlphabeticalOrder(sourceDirPath))
    {
        std::string outputFileName = getStringGivenPosAndToken(getWordAfterLastSymbol(entry.path(), '/'), '.', 0) + ".pdf";

        std::string outputDirFile = outputDirPath + "/" += outputFileName;
        system(("dot -Tpdf -o " + outputDirFile + " " + static_cast<std::string>(entry.path())).c_str());

        std::cout << "Written file from: " << entry.path() << std::endl << "to: " << outputDirFile << "\n\n";
    }
}


int main(int argc, char *argv[])
{
    try
    {
        CliHandler cliHandler(&argc, &argv);
        StringsGetter stringsGetter(cliHandler);

        deleteDirectoryContents(stringsGetter.getOutputPDFsDirPath());

        if (!cliHandler.isCmd(rfd))
        {
            deleteDirectoryContents(stringsGetter.getOutputDOTsDirPath());
            try
            {
                for (const auto &entry: getEntriesInAlphabeticalOrder(stringsGetter.getInputDirPath()))
                {
                    if (std::filesystem::is_regular_file(entry))
                    {
                        std::ifstream file(entry.path());

                        if (static_cast<std::string>(entry.path()).find(".xml") == std::string::npos)
                            continue;

                        if (file.is_open())
                        {
                            // Computing the name of the .dot file to subsequently translate into PDF format.
                            std::string outputFileName = getStringGivenPosAndToken(getWordAfterLastSymbol(entry.path(), '/'), '.', 0);

                            std::cout << "Starting translation from .xml to .dot of file:\n" << entry.path() << std::endl;

                            convertXMLtoDOT(stringsGetter.getOutputDOTsDirPath(), outputFileName, file);
                        }
                    }
                    std::cout << std::endl;
                }
            } catch (const std::filesystem::filesystem_error &e)
            {
                std::cerr << BHRED << "Error while reading directory: " << e.what() << rstColor << std::endl;
                return EXIT_FAILURE;
            }
        }

        // At the end, we translate all the .dot files into .pdf files.
        convertDOTtoPDF(stringsGetter.getOutputDOTsDirPath(), stringsGetter.getOutputPDFsDirPath());

    } catch (CommandNotProvidedException &e)
    {
        std::cerr << BHRED << e.what() << rstColor << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
