#ifndef UTOTPARSER_ACTIONRPAREN_H
#define UTOTPARSER_ACTIONRPAREN_H

#define USE_BOUNDS

#include "nlohmann/json.hpp"

#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/tileEnums/TileTokensEnum.h"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionPushTile.hpp"
#include "TAHeaders/TABoundsCalculator.hpp"

using json = nlohmann::json;


class ActionRParen : public Action {

private:
#ifdef USE_BOUNDS
    /**
     * Method used to get the first bound that was inserted in history at the beginning of the TA construction.
     * Useful in case the TA is composed by just one tile, otherwise the results may contain a redundant bound.
     */
    void getFirstInsertedBound()
    {
        // The following if condition performs those checks:
        // 1) Check if the current node is at nesting level 1 (this is done since, by construction of tests, the entire TA
        //    is surrounded by parentheses). The value with which the nesting level is compared may hence be adjusted by needs.
        // 2) Check if the current node only has one remaining tile.
        if (parserList.getHead()->content.nestingLevel == 1 && parserList.getHead()->content.tileStack.size() == 1)
            TABoundsCalculator::saveCurrentHistoryInPathBounds();
    }


    /**
     * Method used to save the current level of bounds history if the consumed
     * node is part of a branch of a multi 'out' location tile.
     */
    void saveHistoryIfMultiOut()
    {
        // The following if condition performs those checks:
        // 1) Check that at least one multi 'out' location tile will be done after consuming the current node.
        // 2) Check that at least one operator in the lower level of the list is present.
        if (parserList.getHead()->content.nestingLevel > 0
            && !parserList.getHead()->prev->content.operatorStack.empty())
        {
            // The following if condition performs those checks:
            // 1) Check that the actual operator present in the lower level is a multi 'out' location tile operator.
            // TODO: can be enhanced by not hard-coding the operator name.
            if (parserList.getHead()->prev->content.operatorStack.top() == "tree_op")
            {
                TABoundsCalculator::saveCurrentHistoryInPathBounds();

                // Popping the current head of the history since it will not be useful anymore.
                TABoundsCalculator::popFromHistory();
            }
        }
    }


    /**
     * Method used to handle bounds calculation when a ParserNode must be consumed.
     */
    void handleBounds()
    {
        getFirstInsertedBound();
        saveHistoryIfMultiOut();
    }
#endif


public:
    ActionRParen(StringsGetter &stringsGetter, DoublyLinkedList<ParserNode> &parserList, const std::string &token) :
            Action(stringsGetter, parserList, token)
    {};


    /**
     * Method used to perform an action corresponding to an operator.
     * The action consists in consuming the current parserList head, getting the tile present in the current head tileStack,
     * delete the current head node and transfer such tile on top of the stack contained in the node at the previous nesting level.
     */
    void performAction() override
    {
        consumeParserNode();

#ifdef USE_BOUNDS
        handleBounds();
#endif

        if (parserList.getHead()->content.nestingLevel > 0)
        {
            json tileToTransfer = parserList.getHead()->content.tileStack.top();
            parserList.deleteFirst();
            parserList.getHead()->content.tileStack.push(tileToTransfer);
        }
    }

};


#endif //UTOTPARSER_ACTIONRPAREN_H
