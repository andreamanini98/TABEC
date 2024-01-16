#ifndef UTOTPARSER_PARSEROPERATOR_H
#define UTOTPARSER_PARSEROPERATOR_H

#include "nlohmann/json.hpp"

#include "DoublyLinkedList.hpp"
#include "TAHeaders/TATileHeaders/lexerAndParser/ParserNode.hpp"
#include "TAHeaders/TAContentExtractor.hpp"

using json = nlohmann::json;


class Operator {

protected:
    DoublyLinkedList<ParserNode> &parserList;

public:
    explicit Operator(DoublyLinkedList<ParserNode> &parserList) : parserList(parserList)
    {};


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

        // For each location to delete, the 'inFile' locations are scanned and, if the if-condition match, the location's name is deleted.
        for (const std::string &name: locationNames)
            for (auto &loc: *locations)
                if (loc.contains(NAME)
                    && static_cast<std::string>(loc.at(ID)) == name
                    && static_cast<std::string>(loc.at(NAME).at(TEXT)) == locationText)
                    loc.erase(NAME);
    }


    /**
     * Method used to execute an Operator.
     */
    virtual void executeOperator() = 0;

};


#endif //UTOTPARSER_PARSEROPERATOR_H
