#ifndef UTOTPARSER_ACTIONPUSHRANDOMTILE_H
#define UTOTPARSER_ACTIONPUSHRANDOMTILE_H

#include "nlohmann/json.hpp"

#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"
#include "TAHeaders/TAContentExtractor.hpp"
#include "TAHeaders/TATileHeaders/TileTokensEnum.h"
#include "TAHeaders/TATileHeaders/parserActionFactory/Action.hpp"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/TileRandomCreatorFactory.hpp"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/RandomCreatorFactory.hpp"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/RandomCreator.hpp"

using json = nlohmann::json;


class ActionPushRandomTile : public Action {

public:
    ActionPushRandomTile(StringsGetter &stringsGetter, DoublyLinkedList<ParserNode> &parserList, const std::string &token) :
            Action(stringsGetter, parserList, token)
    {};


    void performAction() override
    {
        std::cout << "Now generating random tile\n";

        TileRandomCreatorFactory *randomCreatorFactory = new RandomCreatorFactory();
        RandomCreator *randomCreator = randomCreatorFactory->createRandomCreator(token);
        json randomTile = randomCreator->createRandomTile();

        // Each tile has to be renamed in order to avoid name clashes.
        // TODO: once finished, check if the tile gets correctly renamed.
        TATileRenamer::renameIDs(randomTile);
        parserList.getHead()->content.tileStack.push(randomTile);
    }

};


#endif //UTOTPARSER_ACTIONPUSHRANDOMTILE_H


