#ifndef UTOTPARSER_ACTIONPUSHOPERATOR_HPP
#define UTOTPARSER_ACTIONPUSHOPERATOR_HPP

#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/TileOperatorEnum.h"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionPushTile.hpp"


class ActionPushOperator : public Action {

public:
    ActionPushOperator(StringsGetter &stringsGetter, DoublyLinkedList<ParserNode> &parserList, const std::string &token) :
            Action(stringsGetter, parserList, token)
    {};


    /**
     * Method used to perform an action corresponding to an operator.
     * The action consists in pushing the operator's name on top to the operatorStack, contained in the parserList.
     */
    void performAction() override
    {
        parserList.getHead()->content.operatorStack.emplace(token);
    }

};


#endif //UTOTPARSER_ACTIONPUSHOPERATOR_HPP
