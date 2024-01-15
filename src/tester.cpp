#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "utilities/StringsGetter.hpp"
#include "utilities/CliHandler.hpp"
#include "TAHeaders/TADotConverter.hpp"
#include "TAHeaders/TAutotTranslator.hpp"
#include "TAHeaders/TATileHeaders/TATileRegExGenerator.hpp"
#include "TAHeaders/TATileHeaders/TATileRegExGeneratorStrict.hpp"
#include "TAHeaders/TATileHeaders/TATileInputParser.hpp"
#include "Exceptions.h"

using json = nlohmann::json;


/**
 * Function used to print the TiledTA.
 * @param tiledTA the TiledTA to print.
 */
void printTiledTA(const json &tiledTA)
{
    std::cout << "\n\nResultingTiledTA:\n\n";
    std::cout << std::setw(4) << tiledTA << std::endl;
}


/**
 * Function used to convert a TA to a .dot file.
 * @param outputDOTDirPath the path to the directory where the .dot file will be written.
 * @param tiledTAName the name of the Tiled TA.
 * @param tiledTA a json representation of the Tiled TA to convert into .dot format.
 */
void convertTiledTAtoDOT(const std::string &outputDOTDirPath, const std::string &tiledTAName, const json &tiledTA)
{
    TADotConverter taDotConverter(outputDOTDirPath + "/" += (tiledTAName + ".dot"));
    taDotConverter.translateTAtoDot(tiledTAName, tiledTA);
}


/**
 * Function used to convert a TA to a .tck file.
 * @param outputDirPath the path to the directory where the .tck file will be written.
 * @param tiledTAName the name of the Tiled TA.
 * @param tiledTA a json representation of the Tiled TA to convert into .tck format.
 */
void convertTiledTAtoTCK(const std::string &outputDirPath, const std::string &tiledTAName, const json &tiledTA)
{
    Translator translator(outputDirPath + "/" += (tiledTAName + ".tck"));
    translator.translateTA(tiledTAName, tiledTA);
    std::cout << BHGRN << "Conversion successful" << rstColor << std::endl;
}


// TODO: I fear that the TATileRegExGenerator is too general and will fail almost every time in constructing
//       semantically correct tiles. For this reason, i think it would be nice to create another regex generator
//       but more controlled, letting the user choose which one to use via the command line.

int main(int argc, char *argv[])
{
    CliHandler cliHandler(&argc, &argv);
    StringsGetter stringsGetter(cliHandler);

    TATileRegExGenerator taTileRegExGenerator;

    std::cout << "Starting tester.\n";

    std::string regEx { taTileRegExGenerator.generateRegEx() };
    std::cout << "Obtained string:\n" << regEx << "\n\n";

    TATileInputParser taTileInputParser(stringsGetter, regEx);
    json tiledTA = taTileInputParser.getTiledTA();

    printTiledTA(tiledTA);
    convertTiledTAtoDOT(stringsGetter.getOutputDOTsDirPath(), "RegExTA", tiledTA);
    convertTiledTAtoTCK(stringsGetter.getOutputDirPath(), "RegExTA", tiledTA);

    return EXIT_SUCCESS;
}

