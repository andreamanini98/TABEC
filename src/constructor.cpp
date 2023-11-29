#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/TATileConstructor.hpp"
#include "TAHeaders/TATileHeaders/TATileInputParser.hpp"
#include "utilities/Utils.hpp"
#include "utilities/CliHandler.hpp"
#include "TAHeaders/TADotConverter.hpp"
#include "TAHeaders/TAutotTranslator.hpp"
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
    std::cout << BHGRN << "Conversion successful" << reset << std::endl;
}


/**
 * Function used to insert in the 'tiles' vector the name of the tile and its json representation.
 * These tiles will then be used for constructing the Tiled TA.
 * @param inputDirPath a path to the input directory from which to take the tiles.
 * @param tiles a vector of pairs, where the first element is the tile name, while the second is the tile json representation.
 * @param neededTiles a vector containing the names of the tiles to insert into the 'tiles' vector.
 */
void gatherNeededTiles(const std::string &inputDirPath, std::vector<std::pair<std::string, json>> &tiles, const std::vector<std::string> &neededTiles)
{
    for (const auto &entry: getEntriesInAlphabeticalOrder(inputDirPath))
    {
        std::ifstream file(entry.path());

        if (static_cast<std::string>(entry.path()).find(".xml") == std::string::npos)
            continue;

        std::string tileName = getStringGivenPosAndToken(getWordAfterLastSymbol(entry.path(), '/'), '.', 0);

        // If the current entry's name is not one in the neededTiles vector, we do not insert it in the result.
        if (std::find(neededTiles.begin(), neededTiles.end(), tileName) == neededTiles.end())
            continue;

        tiles.emplace_back(tileName, getJsonFromFileStream(file));
    }
    // If no needed tiles have been found, an exception is raised since no Tiled TA can be constructed.
    if (tiles.empty())
        throw NeededTilesNotPresentException("Exception: no wanted tile is present between the available ones");
}


int main(int argc, char *argv[])
{
    try
    {
        CliHandler cliHandler(&argc, &argv);
        StringsGetter stringsGetter(cliHandler);
        TATileInputParser taTileInputParser(stringsGetter);

        // Each pair contains the tile's name and its json representation.
        std::vector<std::pair<std::string, json>> tiles;

        // A vector containing the name of the tiles needed for the Tiled TA construction.
        std::vector<std::string> neededTiles {};

        // Based on the fact that the user chooses to input a compositional string, the needed tiles are determined.
        if (cliHandler.isCmd(inp))
            neededTiles = taTileInputParser.getNeededTilesNames();
        else
            neededTiles = getAllFileNamesInDirectory(stringsGetter.getInputTilesDirPath());

        try
        {
            gatherNeededTiles(stringsGetter.getInputTilesDirPath(), tiles, neededTiles);

            TATileConstructor taTileConstructor(tiles);
            json tiledTA = taTileConstructor.createTAFromTiles();

            std::string tiledTAName = "TiledTA";

            if (!(cliHandler.isCmd(inp) || cliHandler.isCmd(tdt) || cliHandler.isCmd(ttt)))
                printTiledTA(tiledTA);

            if (cliHandler.isCmd(tdt))
                convertTiledTAtoDOT(stringsGetter.getOutputDOTsDirPath(), tiledTAName, tiledTA);

            if (cliHandler.isCmd(ttt))
                convertTiledTAtoTCK(stringsGetter.getOutputDirPath(), tiledTAName, tiledTA);

        } catch (ConnectTilesMatchInOutSizeException &e)
        {
            std::cerr << BHRED << e.what() << reset << std::endl;
            return EXIT_FAILURE;
        } catch (NeededTilesNotPresentException &e)
        {
            std::cerr << BHRED << e.what() << reset << std::endl;
            return EXIT_FAILURE;
        }
    } catch (CommandNotProvidedException &e)
    {
        std::cerr << BHRED << e.what() << reset << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
