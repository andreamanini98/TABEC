#ifndef UTOTPARSER_ACTION_H
#define UTOTPARSER_ACTION_H

#include "utilities/StringsGetter.hpp"
#include "DoublyLinkedList.hpp"
#include "TAHeaders/TATileHeaders/ParserNode.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/Operator.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/OperatorFactory.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/ParserOperatorFactory.hpp"


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


    // Method used to consume the top-level parserList's node in order to compose the tiles contained in its stack with
    // respect to the operators contained in its other stack.
    void consumeParserNode()
    {
        while (!(parserList.getHead()->content.operatorStack.empty()))
        {
            // Get the current operator and remove it from the top of the stack.
            std::string currentOperator = parserList.getHead()->content.operatorStack.top();
            parserList.getHead()->content.operatorStack.pop();

            ParserOperatorFactory *operatorFactory = new OperatorFactory;
            Operator *parserOperator = operatorFactory->createOperator(parserList, currentOperator);
            parserOperator->executeOperator();
        }
    }


    /**
     * Method used to perform the action related to the current token.
     */
    virtual void performAction() = 0;

};


#endif //UTOTPARSER_ACTION_H
