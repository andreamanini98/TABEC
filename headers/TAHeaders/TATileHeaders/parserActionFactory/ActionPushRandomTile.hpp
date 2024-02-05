#ifndef UTOTPARSER_ACTIONPUSHRANDOMTILE_H
#define UTOTPARSER_ACTIONPUSHRANDOMTILE_H

#define USE_BOUNDS

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


#ifdef USE_BOUNDS
    /**
     * Method used to duplicate the bounds history in the case where, if a tile not having multiple
     * outputs is read, if a multi 'out' location tile operator has still to be performed, the history
     * needs to be duplicated in order to use it in the other branch.
     */
    void handleBounds()
    {
        // The following if condition performs those checks:
        // 1) Check that at least one multi 'out' location tile will be done after consuming the current node.
        // 2) Check if the tile is the first one of the new branch (WARNING: this condition is dependent on whether the bounds
        //    manipulation is performed before or after pushing the tile onto the stack).
        // 3) Check that one branch has already been consumed (a tile has been put in
        //    the lower level of the list, together with the multi 'out' location one).
        // 4) Check that at least one operator in the lower level of the list is present.
        // TODO: can be enhanced by not hard-coding the tile type.
        if (parserList.getHead()->content.nestingLevel > 0
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
#endif

public:
    ActionPushRandomTile(StringsGetter &stringsGetter, DoublyLinkedList<ParserNode> &parserList, const std::string &token, int syntaxParameter) :
            Action(stringsGetter, parserList, token), syntaxParameter(syntaxParameter)
    {};


    void performAction() override
    {
#ifdef USE_BOUNDS
        handleBounds();
#endif

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


