#ifndef UTOTPARSER_ACTIONPUSHRANDOMTILE_H
#define UTOTPARSER_ACTIONPUSHRANDOMTILE_H

#include "nlohmann/json.hpp"

#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"
#include "TAHeaders/TAContentExtractor.hpp"
#include "TAHeaders/TATileHeaders/tileEnums/TileTokensEnum.h"
#include "TAHeaders/TATileHeaders/parserActionFactory/Action.hpp"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/TileRandomCreatorFactory.hpp"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/RandomCreatorFactory.hpp"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/RandomCreator.hpp"

using json = nlohmann::json;


class ActionPushRandomTile : public Action {

private:
    int syntaxParameter {};

public:
    ActionPushRandomTile(StringsGetter &stringsGetter, DoublyLinkedList<ParserNode> &parserList, const std::string &token, int syntaxParameter) :
            Action(stringsGetter, parserList, token), syntaxParameter(syntaxParameter)
    {};


    void performAction() override
    {
        std::cout << "Now generating random tile\n";

        TileRandomCreatorFactory *randomCreatorFactory = new RandomCreatorFactory();
        RandomCreator *randomCreator = randomCreatorFactory->createRandomCreator(token, syntaxParameter);
        json randomTile = randomCreator->createRandomTile();

        // Each tile has to be renamed in order to avoid name clashes.
        TATileRenamer::renameIDs(randomTile);
        parserList.getHead()->content.tileStack.push(randomTile);
    }

};


#endif //UTOTPARSER_ACTIONPUSHRANDOMTILE_H


