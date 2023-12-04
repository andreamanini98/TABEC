#ifndef UTOTPARSER_PARSERACTIONFACTORY_H
#define UTOTPARSER_PARSERACTIONFACTORY_H

#include "DoublyLinkedList.hpp"
#include "TAHeaders/TATileHeaders/ParserNode.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/Action.hpp"


class ParserActionFactory {

public:
    /**
     * Method used to select which action has to be executed based on the given 'token' parameter.
     * @param parserList the data structure containing the tiles and operators needed by the parser.
     * @param token the token based on which the action will be executed.
     * @return an action that will affect the 'parserList' parameter based on the given 'token' parameter.
     */
    virtual Action *createAction(DoublyLinkedList<ParserNode> &parserList, const std::string &token) = 0;

};


#endif //UTOTPARSER_PARSERACTIONFACTORY_H
