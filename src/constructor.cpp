#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/TATileConstructor.hpp"
#include "utilities/Utils.hpp"
#include "utilities/CliHandler.hpp"
#include "TAHeaders/TADotConverter.hpp"
#include "TAHeaders/TAutotTranslator.hpp"

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


int main(int argc, char *argv[])
{
    try
    {
        CliHandler cliHandler(&argc, &argv);
        StringsGetter stringsGetter(cliHandler);

        std::vector<json> tiles;

        for (const auto &entry: getEntriesInAlphabeticalOrder(stringsGetter.getInputTilesDirPath()))
        {
            std::ifstream file(entry.path());

            if (static_cast<std::string>(entry.path()).find(".xml") == std::string::npos)
                continue;

            if (file.is_open())
                tiles.push_back(getJsonFromFileStream(file));
        }

        try
        {
            TATileConstructor taTileConstructor(tiles);
            json tiledTA = taTileConstructor.createTAFromTiles();

            std::string tiledTAName = "TiledTA";

            if (!(cliHandler.isCmd(tdt) && cliHandler.isCmd(ttt)))
                printTiledTA(tiledTA);

            if (cliHandler.isCmd(tdt))
                convertTiledTAtoDOT(stringsGetter.getOutputDOTsDirPath(), tiledTAName, tiledTA);

            if (cliHandler.isCmd(ttt))
                convertTiledTAtoTCK(stringsGetter.getOutputDirPath(), tiledTAName, tiledTA);

        } catch (ConnectTilesMatchInOutSizeException &e)
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
