#ifndef UTOTPARSER_CONNECTOR_H
#define UTOTPARSER_CONNECTOR_H

#include "nlohmann/json.hpp"

#include "utilities/JsonHelper.hpp"
#include "Enums.h"

using json = nlohmann::json;


class Connector {

protected:
    // The tile from which the new transitions will start, with out nodes as source.
    const json &tile1;

    // The tile from which the new transitions will join, with out nodes as target.
    const json &tile2;

    // The destination tile in which the new transition will be added.
    json &destTile;

    // An integer that will be used to keep new transitions unique.
    int newTransNonce;


public:
    Connector(const json &tile1, const json &tile2, json &destTile) : tile1(tile1), tile2(tile2), destTile(destTile)
    {
        newTransNonce = 0;
    };


    /**
     * Method used to get a string representation of newTransNonce.
     * @return a string representation of taNonce.
     */
    [[nodiscard]] std::string newTransNonceStr() const
    {
        return std::to_string(newTransNonce);
    }


    /**
     * Method used to create and insert a new transition in the given destTile json.
     * @param sourceLocId the id of the transition source.
     * @param destLocId the id of the transition target.
     * @param destTile the destination tile in which the new transition will be added.
     * @param assignmentText the text appearing on the assignment of the new transition.
     */
    void insertNewTransition(const std::string &sourceLocId, const std::string &destLocId, const std::string &assignmentText)
    {
        // A pointer to the json representation of the transitions section of the destination tile.
        json *destTileTransitionsPtr = getJsonPtrAsArray(TAContentExtractor::getTransitionsPtr(destTile));

        json newTrans = {
                { ID,     "newTrans" + newTransNonceStr() },
                { LABEL,  {
                                  { TEXT, assignmentText },
                                  { KIND, ASSIGNMENT }
                          }},
                { SOURCE, {
                                  { REF,  sourceLocId }
                          }},
                { TARGET, {
                                  { REF,  destLocId }
                          }}
        };

        destTileTransitionsPtr->push_back(newTrans);
        newTransNonce++;
    }


    /**
     * Method used to connect tile1 and tile2, saving the resulting transitions in destTile.
     */
    virtual void connectTiles() = 0;

};


#endif //UTOTPARSER_CONNECTOR_H
