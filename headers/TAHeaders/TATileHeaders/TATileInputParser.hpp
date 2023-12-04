#ifndef UTOTPARSER_TATILEINPUTPARSER_H
#define UTOTPARSER_TATILEINPUTPARSER_H

#include <stack>
#include "nlohmann/json.hpp"

#include "Structs.h"
#include "DoublyLinkedList.hpp"
#include "utilities/StringsGetter.hpp"
#include "utilities/Utils.hpp"
#include "TAHeaders/TATileHeaders/TATileConstructor.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/Connector.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/ConnectorFactory.hpp"
#include "TAHeaders/TATileHeaders/ParserNode.hpp"
#include "TAHeaders/TATileHeaders/TATileInputLexer.hpp"
#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/Action.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionFactory.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ParserActionFactory.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/Operator.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/OperatorFactory.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/ParserOperatorFactory.hpp"

using json = nlohmann::json;


class TATileInputParser {

private:
    StringsGetter &stringsGetter;

    // A string containing the user input describing how to merge the tiles.
    std::string compositionalTileString {};

    // A doubly linked list that will be used to build stacks in order to keep track of the tiles and operators.
    DoublyLinkedList<ParserNode> parserList {};

    // A lexer able to get an input string from the user and tokenize it.
    TATileInputLexer taTileInputLexer;


    /**
     * Method used to perform an action based on the current token being parsed.
     * @param token the token that will determine the action to execute.
     */
    void performAction(const std::string &token)
    {
        std::vector<std::pair<std::string, std::string>> availableTiles = taTileInputLexer.getTileTokens();

        ParserActionFactory *actionFactory = new ActionFactory(stringsGetter, availableTiles);
        Action *action = actionFactory->createAction(parserList, token);
        action->performAction();
    }


    // Method used to consume the top-level parserList's node in order to compose the tiles contained in its stack with
    // respect to the operators contained in its other stack.
    void consumeParserNode()
    {
        while (!parserList.getHead()->content.operatorStack.empty())
        {
            // Get the current operator and remove it from the top of the stack.
            std::string currentOperator = parserList.getHead()->content.operatorStack.top();
            parserList.getHead()->content.operatorStack.pop();

            ParserOperatorFactory *operatorFactory = new OperatorFactory;
            Operator *parserOperator = operatorFactory->createOperator(parserList, currentOperator);
            parserOperator->executeOperator();
        }

        // TODO: Here you have to implement the logic of checking if you still have other nesting levels (i.e. you were not in nesting level 0).
        //       If that is the case, you'll have to pass the remaining tile on the current level on top of the stack of the next level (remember
        //       you'll also have to update the linked list accordingly, maybe just setting head to head->prev will suffice.
    }


    /**
     * Method used to parse the tokenized string and execute actions accordingly, based on the current symbol encountered.
     */
    void parseAndPerformActions()
    {
        // The vector containing pairs { token, token symbol }.
        std::vector<std::pair<std::string, std::string>> tokens { taTileInputLexer.getTokens() };

        // This is the tokenized string to parse in order to detect tokens.
        std::string tokenizedStringToParse { compositionalTileString };

        // Iterate through the entire string, until tokenizedStringToParse is completely parsed.
        while (!tokenizedStringToParse.empty())
        {
            bool found { false };

            // Check substrings from the current position inside tokenizedStringToParse.
            for (const auto &token: tokens)
            {
                // This if-clause checks if, starting from the current position of tokenizedStringToParse, a token matches.
                if (tokenizedStringToParse.find(token.first) == 0)
                {
                    performAction(token.first);

                    // Updating tokenizedStringToParse with the remaining portion to parse.
                    tokenizedStringToParse = tokenizedStringToParse.substr(token.first.length());
                    found = true;
                    break;
                }
            }
            // If no token is found at the current position, move to the next character.
            if (!found)
                tokenizedStringToParse = tokenizedStringToParse.substr(1);
        }
    }


public:
    explicit TATileInputParser(StringsGetter &stringsGetter) : stringsGetter(stringsGetter), taTileInputLexer(stringsGetter)
    {
        parserList.insertFirst(ParserNode());
        compositionalTileString = taTileInputLexer.getTokenizedCompositionalString();
    };


    /**
     * Method used to merge together the given tiles and obtain a Tiled TA.
     * @return a json representation of a Tiled TA obtained by combining the given tiles and operators.
     */
    json getTiledTA()
    {
        parseAndPerformActions();

        consumeParserNode();

        return parserList.getHead()->content.tileStack.top();
    }

};


#endif //UTOTPARSER_TATILEINPUTPARSER_H
