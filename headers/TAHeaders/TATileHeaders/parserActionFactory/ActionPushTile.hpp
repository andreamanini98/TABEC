#ifndef UTOTPARSER_ACTIONPUSHTILE_H
#define UTOTPARSER_ACTIONPUSHTILE_H

#define USE_BOUNDS

#include "nlohmann/json.hpp"

#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"
#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/Action.hpp"
#include "TAHeaders/TATileHeaders/tileEnums/TileTypeEnum.h"
#include "TAHeaders/TABoundsCalculator.hpp"
#include "TAHeaders/TABoundsCalculator.hpp"

using json = nlohmann::json;


// Tiles naming conventions (how they have to be stored in the folders as .xml files):
// Accepting tiles: acc.xml
// Binary tiles: bin_bounds.xml
// Ternary tiles: tri_bounds.xml
// Binary tiles with an accepting location: bin_acc_bounds.xml

class ActionPushTile : public Action {

private:
    TileTypeEnum tileType;


#ifdef USE_BOUNDS
    /**
     * Method used to duplicate the bounds history in the case where, if a tile not having multiple
     * outputs is read, if a multi 'out' location tile operator has still to be performed, the history
     * needs to be duplicated in order to use it in the other branch.
     */
    void duplicateHistoryIfNotMultiOut()
    {
        // The following if condition performs those checks:
        // 1) Check if the tile is not a multi 'out' location tile.
        // 2) Check if the tile is the first one of the new branch (WARNING: this condition is dependent on whether the bounds
        //    manipulation is performed before or after pushing the tile onto the stack).
        // 3) Check that at least one multi 'out' location tile will be done after consuming the current node.
        // 4) Check that one branch has already been consumed (a tile has been put in
        //    the lower level of the list, together with the multi 'out' location one).
        // 5) Check that at least one operator in the lower level of the list is present.
        // TODO: can be enhanced by not hard-coding the tile type.
        if (tileType != triTile
            && parserList.getHead()->content.nestingLevel > 0
            && parserList.getHead()->content.tileStack.empty()
            && parserList.getHead()->prev->content.tileStack.size() > 1
            && !parserList.getHead()->prev->content.operatorStack.empty())
        {
            // The following if condition performs those checks:
            // 1) Check that the actual operator present in the lower level is a multi 'out' location tile operator.
            // TODO: can be enhanced by not hard-coding the operator name.
            if (parserList.getHead()->prev->content.operatorStack.top() == "tree_op")
                TABoundsCalculator::duplicateHistory();
        }
    }


    /**
     * Method used to handle bounds calculation when a new tile is processed.
     * @param tile the tile being processed.
     */
    void handleBounds(const json &tile, const std::string &token)
    {
        duplicateHistoryIfNotMultiOut();

        // Updating the bounds for the current TA based on the tile just collected.
        TABoundsCalculator::addBounds(TAContentExtractor::getDeclaration(tile));

        // If a tile is not properly an accepting tile but has an accepting location, the bound needs to be stored.
        // Necessary for example if such tile is the first of the Tl0 string.
        // TODO: this is based on naming convention of the .xml file, it can be enhanced someway.
        if (token.find("_acc") != std::string::npos)
            TABoundsCalculator::saveCurrentHistoryInPathBounds();

        // If the tile is a multi 'out' location tile, the history of current paths bounds must be saved.
        // The check on the size is done to avoid duplicates if the first tile is already a triTile.
        // TODO: can be enhanced by not hard-coding the tile type.
        if (tileType == triTile)
            TABoundsCalculator::duplicateHistory();
    }
#endif


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
        // USE json tile = getTile() AND NOT json tile { getTile() } SINCE THE LATTER IN GCC WILL BE
        // CONSIDERED AN ARRAY, WHILE IN CLANG IT WILL BE CONSIDERED SIMPLY A JSON AS IT SHOULD BE.
        json tile = getTile();

#ifdef USE_BOUNDS
        handleBounds(tile, token);
#endif

        // Each tile has to be renamed in order to avoid name clashes.
        TATileRenamer::renameIDs(tile);
        parserList.getHead()->content.tileStack.push(tile);
    }

};


#endif //UTOTPARSER_ACTIONPUSHTILE_H
