#ifndef UTOTPARSER_PARSEROPERATORFACTORY_H
#define UTOTPARSER_PARSEROPERATORFACTORY_H

#include "DoublyLinkedList.hpp"
#include "TAHeaders/TATileHeaders/ParserNode.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/Operator.hpp"


class ParserOperatorFactory {

public:
    /**
     * Method used to select which operator must be executed based on the given 'token' parameter.
     * @param parserList the data structure containing the tiles and operators needed by the parser.
     * @param token the token which will be used to determine which operator to execute.
     * @return an Operator that will affect the 'parserList' parameter based on the 'token' parameter.
     */
    virtual Operator *createOperator(DoublyLinkedList<ParserNode> &parserList, const std::string &token) = 0;

};


#endif //UTOTPARSER_PARSEROPERATORFACTORY_H
