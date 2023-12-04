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
     * Method used to delete all the in or out locations' names in order to avoid such unnecessary names for further connections.
     * This method deletes only the locations' name items, not the whole locations.
     * @param inFile the json file in which to delete the desired locations' names.
     * @param locationNames the name of the locations which name has to be deleted.
     * @param locationText the text that must match with the one of the location in order to delete such location's name.
     */
    static void deleteLocName(json &inFile, const std::vector<std::string> &locationNames, const std::string &locationText)
    {
        json *locations = TAContentExtractor::getLocationsPtr(inFile);

        // For each location to delete, the 'inFile' locations are scanned and, if the if conditions match, the location's name is deleted.
        for (const std::string &name: locationNames)
            for (auto &loc: *locations)
                if (loc.contains(NAME)
                    && static_cast<std::string>(loc.at(ID)) == name
                    && static_cast<std::string>(loc.at(NAME).at(TEXT)) == locationText)
                    loc.erase(NAME);
    }


    // TODO: I think here is better to create a factory method where you pass the linked list and perform actions by consequence.
    void performAction_STUB(const std::string &token)
    {
        if (token == "only_one_out")
        {
            parserList.getHead()->content.operatorStack.emplace("only_one_out");
        } else if (token == "match_inout_size")
        {
            parserList.getHead()->content.operatorStack.emplace("match_inout_size");
        } else
        {
            std::vector<std::pair<std::string, std::string>> tileNames = taTileInputLexer.getTileTokens();
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


    // TODO: Also here maybe a factory is necessary. also, beautify this shit.
    void consumeParserNode()
    {
        TileConnectorFactory *tileConnectorFactory = new ConnectorFactory;

        while (!parserList.getHead()->content.operatorStack.empty())
        {
            std::string currentOperator = parserList.getHead()->content.operatorStack.top();
            parserList.getHead()->content.operatorStack.pop();

            // We take the tiles in opposite order, since we used a stack internal representation.
            json t2 = parserList.getHead()->content.tileStack.top();
            parserList.getHead()->content.tileStack.pop();

            json t1 = parserList.getHead()->content.tileStack.top();
            parserList.getHead()->content.tileStack.pop();

            json destTile = t1;

            // For each tile required by the operator, we merge the locations and transitions into the destination one.
            TATileConstructor::mergeLocations(t2, destTile);
            TATileConstructor::mergeTransitions(t2, destTile);

            TileConstructMethod method = fromStrTileConstructMethod(currentOperator);

            Connector *connector;
            connector = tileConnectorFactory->createConnector(t1, t2, destTile, method);
            connector->connectTiles();

            // Gathering the names of the locations which in and out locations' names will be deleted.
            // If we assume t1 will be on the right and t2 on the left, then for t1, out locations' names have to be
            // deleted, while for t2, in locations' names have to be deleted.
            deleteLocName(destTile, TAContentExtractor::getNamedLocations(t2, IN), IN);
            deleteLocName(destTile, TAContentExtractor::getNamedLocations(t1, OUT), OUT);

            parserList.getHead()->content.tileStack.push(destTile);
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

        consumeParserNode();

        std::cout << std::setw(4) << parserList.getHead()->content.tileStack.top() << std::endl;

        return parserList.getHead()->content.tileStack.top();
    }

};


#endif //UTOTPARSER_TATILEINPUTPARSER_H
