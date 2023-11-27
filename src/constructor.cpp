#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/TATileConstructor.hpp"
#include "utilities/Utils.hpp"
#include "utilities/CliHandler.hpp"
#include "TAHeaders/TADotConverter.hpp"
#include "TAHeaders/TAutotTranslator.hpp"

#define CONSTRUCTOR_DEBUG

using json = nlohmann::json;


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

#ifndef CONSTRUCTOR_DEBUG
            std::cout << "\n\nResultingTiledTA:\n\n";
            std::cout << std::setw(4) << tiledTA << std::endl;
#endif

            std::string tiledTAName = "TiledTA";

            if (cliHandler.isCmd(tdt))
            {
                TADotConverter taDotConverter(stringsGetter.getOutputDOTsDirPath() + "/" += (tiledTAName + ".dot"));
                taDotConverter.translateTAtoDot(tiledTAName, tiledTA);
            }
            if (cliHandler.isCmd(ttt))
            {
                Translator translator(stringsGetter.getOutputDirPath() + "/" += (tiledTAName + ".tck"));
                translator.translateTA(tiledTAName, tiledTA);
                std::cout << BHGRN << "Conversion successful" << reset << std::endl;
            }
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
