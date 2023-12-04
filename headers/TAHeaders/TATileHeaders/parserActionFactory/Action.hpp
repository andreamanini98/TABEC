#ifndef UTOTPARSER_ACTION_H
#define UTOTPARSER_ACTION_H

#include "utilities/StringsGetter.hpp"
#include "DoublyLinkedList.hpp"
#include "TAHeaders/TATileHeaders/ParserNode.hpp"


class Action {

protected:
    StringsGetter &stringsGetter;

    // The data structure containing the tiles and operators needed by the parser.
    DoublyLinkedList<ParserNode> &parserList;

    // The token relative to the action that has to be executed.
    const std::string &token;

public:
    Action(StringsGetter &stringsGetter, DoublyLinkedList<ParserNode> &parserList, const std::string &token) :
            stringsGetter(stringsGetter), parserList(parserList), token(token)
    {};


    /**
     * Method used to perform the action related to the current token.
     */
    virtual void performAction() = 0;

};


#endif //UTOTPARSER_ACTION_H
