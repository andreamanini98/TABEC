#ifndef UTOTPARSER_ACTIONRPAREN_H
#define UTOTPARSER_ACTIONRPAREN_H

#include "nlohmann/json.hpp"

#include "utilities/StringsGetter.hpp"
#include "Enums.h"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionPushTile.hpp"

using json = nlohmann::json;


class ActionRParen : public Action {

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

        if (parserList.getHead()->content.nestingLevel > 0)
        {
            json tileToTransfer = parserList.getHead()->content.tileStack.top();
            parserList.deleteFirst();
            parserList.getHead()->content.tileStack.push(tileToTransfer);
        }
    }

};


#endif //UTOTPARSER_ACTIONRPAREN_H
