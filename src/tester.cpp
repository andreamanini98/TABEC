#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "utilities/Utils.hpp"
#include "utilities/StringsGetter.hpp"
#include "utilities/CliHandler.hpp"
#include "TAHeaders/TADotConverter.hpp"
#include "TAHeaders/TAutotTranslator.hpp"
#include "TAHeaders/TATileHeaders/regexGenerator/TATileRegExGenerator.hpp"
#include "TAHeaders/TATileHeaders/regexGenerator/TATileRegExGeneratorStrict.hpp"
#include "TAHeaders/TATileHeaders/lexerAndParser/TATileInputParser.hpp"
#include "utilities/Logger.hpp"
#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"

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


/**
 * Function used to clean directories before starting the automatic test generation.
 * @param stringsGetter a 'StringsGetter' that will retrieve the paths for the directories to clean.
 */
void cleanDirectories(StringsGetter &stringsGetter)
{
    deleteDirectoryContents(stringsGetter.getOutputDirForCheckingPathLogs());
    deleteDirectoryContents(stringsGetter.getOutputDOTsDirPath());
    deleteDirectoryContents(stringsGetter.getOutputDirPath());
    deleteDirectoryContents(stringsGetter.getOutputDirForCheckingPath());
    deleteDirectoryContents(stringsGetter.getOutputPDFsDirPath());
    deleteDirectoryContents(stringsGetter.getTestingResultsDirPath());
    deleteDirectoryContents(stringsGetter.getTestingResourceUsageDirPath());
}


/**
 * Function used to collect the name of all the used tiles inside the randomly-generated regular expression.
 * @param regEx the regular expression in which to look for the used tiles.
 * @param parser an instance of the parser.
 * @return a vector containing the name of all the used tiles.
 */
std::vector<std::string> gatherUsedTiles(const std::string &regEx, TATileInputParser &parser)
{
    std::vector<std::string> usedTiles {};

    for (const std::string &tile: parser.getBinTileSymbols())
        if (regEx.find(tile) != std::string::npos)
            usedTiles.push_back(tile);

    for (const std::string &tile: parser.getTriTileSymbols())
        if (regEx.find(tile) != std::string::npos)
            usedTiles.push_back(tile);

    return usedTiles;
}


/**
 * Function used to write additional information in the logs, useful for analyzing test results.
 * @param stringsGetter a string getter.
 * @param parser an instance of the parser.
 * @param TAName the name of the randomly-generated TA.
 * @param regEx the regular expression used to generate the TA.
 */
void writeLogs(StringsGetter &stringsGetter, TATileInputParser &parser, const std::string &TAName, const std::string &regEx)
{
    Logger logger(stringsGetter.getOutputDirForCheckingPathLogs(), TAName + ".txt");

    std::string logToWrite { "RegEx generated:\n" + regEx + "\n" };

    std::vector<std::string> usedTiles { gatherUsedTiles(regEx, parser) };
    if (!usedTiles.empty())
    {
        logToWrite.append("Used tiles (random and accepting tiles excluded):\n");
        for (const std::string &tile: usedTiles)
            logToWrite.append(tile + " ");
    }
    logger.writeLog(logToWrite.append("\n\n"));
}


/**
 * Function which, by calling a shell script, collects all the results in a single file.
 * @param stringsGetter a string getter.
 */
void gatherResults(StringsGetter &stringsGetter, CliHandler &cliHandler)
{
    system(
            spaceStr({
                             stringsGetter.getOtherScriptsPath() + "/collectResults.sh", // Script name
                             stringsGetter.getOutputDirForCheckingPathLogs(),            // $1
                             stringsGetter.getTestingResultsDirPath(),                   // $2
                             stringsGetter.getTCheckerBinPath(),                         // $3
                             std::to_string(cliHandler.isCmd(atc)), // $4
                             std::to_string(cliHandler.isCmd(atp))  // $5
                     }).c_str());
}


int main(int argc, char *argv[])
{
    CliHandler cliHandler(&argc, &argv);
    StringsGetter stringsGetter(cliHandler);
    TATileInputParser taTileInputParser(stringsGetter);

    // Cleaning directories before starting test generation.
    cleanDirectories(stringsGetter);

    // Integer indicating the maximum number of states randomly-generated tiles will have.
    int maxNumOfRandomStates { cliHandler.isCmd(sup) ? std::stoi(cliHandler.getCmdArgument(sup)) : 5 };

    // Now generating the right regex generator based on the given cli command.
    TATileRegExGenerator *taTileRegExGenerator;

    if (cliHandler.isCmd(tns))
        taTileRegExGenerator = new TATileRegExGenerator(std::stoi(cliHandler.getCmdArgument(tns)));
    else if (cliHandler.isCmd(tst))
    {
        taTileRegExGenerator = new TATileRegExGeneratorStrict(
                std::stoi(cliHandler.getCmdArgument(tst)), maxNumOfRandomStates,
                taTileInputParser.getRngTileTokens(),
                taTileInputParser.getAccTileTokens(),
                taTileInputParser.getBinTileTokens(),
                taTileInputParser.getTriTileTokens());
    } else
    {
        std::cerr << BHRED << "Invalid argument specified for running the tester." << rstColor << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Starting generating random tests.\n";

    int numTests = cliHandler.isCmd(nbt) ? std::stoi(cliHandler.getCmdArgument(nbt)) : 10;
    for (int i = 0; i < numTests; i++)
    {
        // Resetting the nonce at each test generation in order to avoid the index becoming too big.
        TATileRenamer::resetTANonce();

        std::string regEx { taTileRegExGenerator->generateRegEx() };
        std::cout << "Obtained string:\n" << regEx << "\n";

        json tiledTA = taTileInputParser.getTiledTA(regEx);

        std::string TAName = "RegExTA_" + std::to_string(i + 1);

        writeLogs(stringsGetter, taTileInputParser, TAName, regEx);

        printTiledTA(tiledTA);
        convertTiledTAtoDOT(stringsGetter.getOutputDOTsDirPath(), TAName, tiledTA);
        convertTiledTAtoTCK(stringsGetter.getOutputDirPath(), TAName, tiledTA);
    }

    gatherResults(stringsGetter, cliHandler);

    return EXIT_SUCCESS;
}

