#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "utilities/StringsGetter.hpp"
#include "utilities/CliHandler.hpp"
#include "TAHeaders/TADotConverter.hpp"
#include "TAHeaders/TAutotTranslator.hpp"
#include "TAHeaders/TATileHeaders/regexGenerator/TATileRegExGenerator.hpp"
#include "TAHeaders/TATileHeaders/regexGenerator/TATileRegExGeneratorStrict.hpp"
#include "TAHeaders/TATileHeaders/lexerAndParser/TATileInputParser.hpp"

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


// TODO: you should change the way in which you specify tiles here. The tool must automatically infer
//       which tiles are usable for binary or ternary operators, in addition to handling the special case
//       of tile-accepting. maybe a cycle on the tiles counting how many in/out locations are present may suffice.

// TODO: how to proceed:
//       1) Create a way to automatically run tests and gather results.

int main(int argc, char *argv[])
{
    CliHandler cliHandler(&argc, &argv);
    StringsGetter stringsGetter(cliHandler);

    // Integer indicating the maximum number of states randomly-generated tiles will have.
    int maxNumOfRandomStates { cliHandler.isCmd(sup) ? std::stoi(cliHandler.getCmdArgument(sup)) : 5 };

    // Now generating the right regex generator based on the given cli command.
    TATileRegExGenerator *taTileRegExGenerator;

    if (cliHandler.isCmd(tns))
        taTileRegExGenerator = new TATileRegExGenerator(std::stoi(cliHandler.getCmdArgument(tns)));
    else if (cliHandler.isCmd(tst))
        taTileRegExGenerator = new TATileRegExGeneratorStrict(std::stoi(cliHandler.getCmdArgument(tst)), maxNumOfRandomStates);
    else
    {
        std::cerr << BHRED << "Invalid argument specified for running the tester." << rstColor << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Starting generating random tests.\n";

    for (int i = 0; i < 5; i++)
    {
        std::string regEx { taTileRegExGenerator->generateRegEx() };
        std::cout << "Obtained string:\n" << regEx << "\n";

        TATileInputParser taTileInputParser(stringsGetter, regEx);
        json tiledTA = taTileInputParser.getTiledTA();

        std::string TAName = "RegExTA_" + std::to_string(i + 1);

        printTiledTA(tiledTA);
        convertTiledTAtoDOT(stringsGetter.getOutputDOTsDirPath(), TAName, tiledTA);
        convertTiledTAtoTCK(stringsGetter.getOutputDirPath(), TAName, tiledTA);
    }

    // Here you should gather the results of the tests.

    return EXIT_SUCCESS;
}

