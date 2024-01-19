#ifndef UTOTPARSER_ACTIONPUSHTILE_H
#define UTOTPARSER_ACTIONPUSHTILE_H

#include "nlohmann/json.hpp"

#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"
#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/Action.hpp"
#include "TAHeaders/TATileHeaders/tileEnums/TileTypeEnum.h"

using json = nlohmann::json;


class ActionPushTile : public Action {

private:
    TileTypeEnum tileType;

public:
    ActionPushTile(StringsGetter &stringsGetter, DoublyLinkedList<ParserNode> &parserList, const std::string &token, TileTypeEnum tileType) :
            Action(stringsGetter, parserList, token), tileType(tileType)
    {};


    /**
     * Method used to get a json representation of the tile which name is specified inside the 'token'
     * attribute, getting it from the right directory based on the tile's type.
     * @return a json representation of the tile whose name is represented by 'token'.
     */
    json getTile()
    {
        std::string tileDirPath {};

        switch (tileType)
        {
            case accTile:
                tileDirPath = stringsGetter.getAccTilesDirPath();
                break;

            case binTile:
                tileDirPath = stringsGetter.getBinTilesDirPath();
                break;

            case triTile:
                tileDirPath = stringsGetter.getTriTilesDirPath();
                break;

            default:
                std::cerr << BHRED << "Not available type." << rstColor << std::endl;
                std::cerr << BHRED << "Tile corresponding to " << token << " will not be retrieved."
                          << " This may cause unexpected behaviour" << rstColor << std::endl;
                break;
        }

        return getJsonFromFileName(tileDirPath, token);
    }


    /**
     * Method used to perform an action corresponding to a tile name.
     * The action consists in pushing the tile's name on top to the tileStack, contained in the parserList.
     */
    void performAction() override
    {
        json tile { getTile() };

        // Each tile has to be renamed in order to avoid name clashes.
        TATileRenamer::renameIDs(tile);
        parserList.getHead()->content.tileStack.push(tile);
    }

};


#endif //UTOTPARSER_ACTIONPUSHTILE_H
