#ifndef UTOTPARSER_TATILEINPUTPARSER_H
#define UTOTPARSER_TATILEINPUTPARSER_H

#include <stack>
#include "nlohmann/json.hpp"

#include "Structs.h"
#include "DoublyLinkedList.hpp"
#include "utilities/StringsGetter.hpp"
#include "utilities/Utils.hpp"
#include "TAHeaders/TATileHeaders/ParserNode.hpp"
#include "TAHeaders/TATileHeaders/TATileInputLexer.hpp"
#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"

using json = nlohmann::json;


//TODO: i think you can have a TATileConstructor attribute here and in order to create a tiledta:
// 1) find a way of getting the tiles directly from a file only when parsed
// 2) rename the ids with the renamer
// 3) find a way of connecting them based on the operator (you may delete the enumerations and work with strings, but remember to change all related contents in the factory.


class TATileInputParser {

private:
    StringsGetter &stringsGetter;

    // A string containing the user input describing how to merge the tiles.
    std::string compositionalTileString {};

    // A doubly linked list that will be used to build stacks in order to keep track of the tiles and operators.
    DoublyLinkedList<ParserNode> parserList {};

    // A lexer able to get an input string from the user and tokenize it.
    TATileInputLexer taTileInputLexer;


    // TODO: these should serve the purpose of performing actions for each token in the tokenized string.
    //       Most likely, this is where the TATileConstructor will be used.
    // I think here is better to create a factory method where you pass the linked list and perform actions by consequence.
    void performAction_STUB(const std::string &token)
    {
        std::vector<std::pair<std::string, std::string>> tileNames = taTileInputLexer.getTileTokens();

        if (token == "only_one_out")
        {
            parserList.getHead()->content.operatorStack.emplace("only_one_out");
        } else if (token == "match_inout_size")
        {
            parserList.getHead()->content.operatorStack.emplace("match_inout_size");
        } else
        {
            for (auto &t: tileNames)
            {
                if (t.first == token)
                {
                    json tile = getJsonFromFileName(stringsGetter.getInputTilesDirPath(), token);
                    TATileRenamer::renameIDs(tile);
                    parserList.getHead()->content.tileStack.push(tile);
                }
            }
        }
    }


    //void consumeParserNode()
    //{
    //    while (!parserList.getHead()->content.operatorStack.empty())
    //    {
    //
    //    }
    //}


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
                    performAction_STUB(token.first);

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


    // TODO: this is just a stub, you'll have to properly construct a tiledTA.
    json getTiledTA()
    {
        parseAndPerformActions();

        parserList.printList();

        return json::array();
    }

};


#endif //UTOTPARSER_TATILEINPUTPARSER_H
