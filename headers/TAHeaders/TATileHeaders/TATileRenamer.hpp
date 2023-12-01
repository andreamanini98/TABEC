#ifndef UTOTPARSER_TATILERENAMER_H
#define UTOTPARSER_TATILERENAMER_H

#include "nlohmann/json.hpp"

#include "utilities/JsonHelper.hpp"

using json = nlohmann::json;


class TATileRenamer {

private:
    // An integer that will be used to keep identifiers unique.
    static int taNonce;


    /**
     * Method used to get a string representation of taNonce.
     * @return a string representation of taNonce.
     */
    static std::string taNonceStr()
    {
        return std::to_string(taNonce);
    }


    /**
    * Method used to rename the initial location's id.
    * @param tile the tile for which to rename the initial location.
    */
    static void renameInitialLocation(json &tile)
    {
        // A pointer to the initial location's id.
        json *tileInitLocPtr = TAContentExtractor::getInitialLocationNamePtr(tile);

        *tileInitLocPtr = static_cast<std::string>(*tileInitLocPtr) + taNonceStr();
    }


    /**
     * Method used to rename all the id occurrences inside the locations section of a tile.
     * @param tile a json representation of the tile in which we have to modify all the id locations occurrences.
     */
    static void renameLocationsIDs(json &tile)
    {
        // A pointer to the json representation of the locations section of the considered tile.
        json *tileLocationPtr = getJsonPtrAsArray(TAContentExtractor::getLocationsPtr(tile));

        for (auto &loc: *tileLocationPtr)
            loc.at(ID) = static_cast<std::string>(loc.at(ID)) + taNonceStr();
    }


    /**
     * Method used to rename all the id occurrences inside the transitions section of a tile.
     * Since a transition has id occurrences inside the 'id', 'source' and 'target' fields, all these id occurrences must be renamed.
     * @param tile a json representation of the tile in which we have to modify all the id transitions occurrences.
     */
    static void renameTransitions(json &tile)
    {
        // A pointer to the json representation of the transitions section of the considered tile.
        json *tileTransitionPtr = getJsonPtrAsArray(TAContentExtractor::getTransitionsPtr(tile));

        for (auto &trans: *tileTransitionPtr)
        {
            trans.at(ID) = static_cast<std::string>(trans.at(ID)) + taNonceStr();
            trans.at(SOURCE).at(REF) = static_cast<std::string>(trans.at(SOURCE).at(REF)) + taNonceStr();
            trans.at(TARGET).at(REF) = static_cast<std::string>(trans.at(TARGET).at(REF)) + taNonceStr();
        }
    }


public:
    /**
     * Method used to rename all the id occurrences inside the tile's json representation
     * in order to avoid name conflicts when merging states into one single tile.
     * @param tiles a vector containing all the tiles which ids have to be renamed.
     */
    static void renameIDs(std::vector<std::pair<std::string, json>> &tiles)
    {
        for (auto &tile: tiles)
        {
            renameInitialLocation(tile.second);
            renameLocationsIDs(tile.second);
            renameTransitions(tile.second);

            taNonce++;
        }
    }


    /**
     * Method used to rename all the id occurrences inside a single tile's json representation
     * in order to avoid name conflicts when merging states into one single tile.
     * @param tiles a json representation of the tile which ids have to be renamed.
     */
    static void renameIDs(json &tile)
    {
        renameInitialLocation(tile);
        renameLocationsIDs(tile);
        renameTransitions(tile);
        taNonce++;
    }

};

// Defining static attribute taNonce.
int TATileRenamer::taNonce { 0 };


#endif //UTOTPARSER_TATILERENAMER_H
