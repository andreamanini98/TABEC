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
#include "TAHeaders/TATileHeaders/lexerAndParser/ParserNode.hpp"
#include "TAHeaders/TATileHeaders/lexerAndParser/TATileInputLexer.hpp"
#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/Action.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionFactory.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ParserActionFactory.hpp"
#include "TAHeaders/TATileHeaders/tileEnums/TileTypeEnum.h"

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
     * Method used to delete the first 'trimLength' characters from the given string.
     * @param str the string in which to delete the first 'trimLength' characters.
     * @param trimLength the number of characters to delete from the string starting from the first character.
     */
    static void trimStringFromStart(std::string &str, int trimLength)
    {
        str = str.substr(trimLength);
    }


    /**
     * Method used to skip whitespaces while parsing a string starting from the initial character of the string.
     * The procedure is stopped when encountering the first non-whitespace character.
     * The string is then trimmed starting from the first character by an amount of characters equal to the number
     * of whitespace characters encountered.
     * @param tokenizedStringToParse the string in which to ignore whitespace characters.
     */
    static void skipWhitespaces(std::string &tokenizedStringToParse)
    {
        int cursor = 0;
        while (cursor < tokenizedStringToParse.length() && std::isspace(tokenizedStringToParse[cursor]))
            ++cursor;
        trimStringFromStart(tokenizedStringToParse, cursor);
    }


    /**
     * Method used to check if the next symbol is the one specified in the argument.
     * If this is the case, consume such symbol, moving forward inside the string to
     * parse by an amount of characters equal to the symbol's length.
     * @param tokenizedStringToParse the string in which to check if 'symbol' is present.
     * @param symbol string that must be found (or not) inside the given string to parse.
     * @return true if the specified symbol is present, false otherwise.
     */
    static bool expectSymbol(std::string &tokenizedStringToParse, const std::string &symbol)
    {
        skipWhitespaces(tokenizedStringToParse);

        // Checking if the expected symbol is present.
        if (tokenizedStringToParse.find(symbol) == 0)
        {
            // If the symbol is found, move forward inside the string to parse.
            trimStringFromStart(tokenizedStringToParse, static_cast<int>(symbol.length()));
            return true;
        }
        return false;
    }


    /**
     * Method used to read a sequence of digits inside a string and return it as a decimal number.
     * The digits are read until a non-digit character is found.
     * @param tokenizedStringToParse the string in which to read the number.
     * @return a decimal representation of the read number.
     */
    static int readIntegerFromString(std::string &tokenizedStringToParse)
    {
        int res = 0;
        int cursor = 0;

        skipWhitespaces(tokenizedStringToParse);

        // Reading digits until a non-digit character is encountered.
        while (cursor < tokenizedStringToParse.length() && std::isdigit(tokenizedStringToParse[cursor]))
        {
            res = res * 10 + (tokenizedStringToParse[cursor] - '0');
            ++cursor;
        }

        // Moving forward inside the string to parse by an amount of characters equals to the ones read by the while loop.
        trimStringFromStart(tokenizedStringToParse, cursor);

        return res;
    }


    /**
     * Method used to look for a syntax parameter inside the string to parse.
     * The presence of a syntax parameter is denoted by a '[' character.
     * Remember that, in order to be syntactically correct, a ']' must then be found to match the opening one.
     * @param tokenizedStringToParse the string in which to look for a syntax parameter.
     * @param parameterValue the numeric value of the found parameter.
     * @return true if a parameter has been found, false otherwise.
     */
    static bool lookForSyntaxParameter(std::string &tokenizedStringToParse, int &parameterValue)
    {
        // If a left square bracket has been found, consume it and read the parameter value (if present).
        if (expectSymbol(tokenizedStringToParse, "lsqparen"))
        {
            // Obtaining the parameter value.
            parameterValue = readIntegerFromString(tokenizedStringToParse);

            // Expecting the corresponding right square bracket.
            expectSymbol(tokenizedStringToParse, "rsqparen");

            return true;
        }

        // Assigning a dummy value to the not found parameter.
        parameterValue = 0;
        return false;
    }


    /**
     * Method used to perform an action based on the current token being parsed.
     * @param token the token that will determine the action to execute.
     */
    void performAction(const std::string &token, int syntaxParameter)
    {
        // The content of this vector is the following:
        // 1) The type of the tile.
        // 2) A vector of pairs containing:
        //    1) The token of the tile.
        //    2) The symbol representing the tile used in the compositional string.
        std::vector<std::pair<TileTypeEnum, std::vector<std::pair<std::string, std::string>>>> availableTiles = taTileInputLexer.getTileTokens();

        ParserActionFactory *actionFactory = new ActionFactory(stringsGetter, availableTiles);
        Action *action = actionFactory->createAction(parserList, token, syntaxParameter);
        action->performAction();
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
            // Check substrings from the current position inside tokenizedStringToParse.
            for (const auto &token: tokens)
            {
                skipWhitespaces(tokenizedStringToParse);

                // This if-clause checks if, starting from the current position of tokenizedStringToParse, a token matches.
                if (tokenizedStringToParse.find(token.first) == 0)
                {
                    // Updating tokenizedStringToParse with the remaining portion to parse.
                    tokenizedStringToParse = tokenizedStringToParse.substr(token.first.length());

                    int syntaxParameter {};
                    bool syntaxParameterFound = lookForSyntaxParameter(tokenizedStringToParse, syntaxParameter);

                    if (syntaxParameterFound)
                        std::cout << "Found parameter with value: " << syntaxParameter << '\n';

                    performAction(token.first, syntaxParameter);
                    break;
                }
            }
        }

        // At the end of the parsing, we have to force the current node to be consumed, since it may
        // not be ended by a right parenthesis. This is just like inserting a dummy ')' symbol at the end of the parsed string.
        ActionRParen action = ActionRParen(stringsGetter, parserList, "rparen");
        action.performAction();
    }


public:
    explicit TATileInputParser(StringsGetter &stringsGetter) : stringsGetter(stringsGetter), taTileInputLexer(stringsGetter)
    {};


    TATileInputParser(StringsGetter &stringsGetter, bool showHelp) : stringsGetter(stringsGetter), taTileInputLexer(stringsGetter)
    {
        parserList.insertFirst(ParserNode());
        compositionalTileString = taTileInputLexer.getTokenizedCompositionalString(showHelp);
    };


    /**
     * Method used to merge together the given tiles and obtain a Tiled TA.
     * @return a json representation of a Tiled TA obtained by combining the given tiles and operators.
     */
    json getTiledTA()
    {
        std::cout << "Compositional string: " << compositionalTileString << std::endl;
        parseAndPerformActions();

        return parserList.getHead()->content.tileStack.top();
    }


    /**
     * Method used to merge together the given tiles and obtain a Tiled TA, contained in the given 'str' parameter.
     * Useful when creating the parser using the constructor accepting only the 'stringsGetter' parameter.
     * @param str the string describing the structure of the TA.
     * @return a json representation of a Tiled TA obtained by combining the given tiles and operators.
     */
    json getTiledTA(std::string &str)
    {
        parserList.insertFirst(ParserNode());
        compositionalTileString = taTileInputLexer.getTokenizedCompositionalString(str);
        return getTiledTA();
    }


    std::vector<std::string> getRngTileTokens()
    {
        return taTileInputLexer.getRngTileTokens();
    }


    std::vector<std::string> getAccTileTokens()
    {
        return taTileInputLexer.getAccTileTokens();
    }


    std::vector<std::string> getBinTileTokens()
    {
        return taTileInputLexer.getBinTileTokens();
    }


    std::vector<std::string> getTriTileTokens()
    {
        return taTileInputLexer.getTriTileTokens();
    }

};


#endif //UTOTPARSER_TATILEINPUTPARSER_H
