#ifndef UTOTPARSER_ACTIONLPAREN_H
#define UTOTPARSER_ACTIONLPAREN_H

#include "utilities/StringsGetter.hpp"
#include "Enums.h"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionPushTile.hpp"


class ActionLParen : public Action {

public:
    ActionLParen(StringsGetter &stringsGetter, DoublyLinkedList<ParserNode> &parserList, const std::string &token) :
            Action(stringsGetter, parserList, token)
    {};


    /**
     * Method used to perform an action corresponding to an operator.
     * The action consists in creating a new node in the parserList, incrementing the nesting level by 1.
     */
    void performAction() override
    {
        int prevNestingLevel = parserList.getHead()->content.nestingLevel;
        parserList.insertFirst(ParserNode(++prevNestingLevel));
    }

};


#endif //UTOTPARSER_ACTIONLPAREN_H
