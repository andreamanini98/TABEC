#ifndef UTOTPARSER_TATILECONSTRUCTOR_HPP
#define UTOTPARSER_TATILECONSTRUCTOR_HPP

#include <utility>
#include "nlohmann/json.hpp"

#include "defines/UPPAALxmlAttributes.h"
#include "TAHeaders/TAContentExtractor.hpp"
#include "utilities/JsonHelper.hpp"
#include "Exceptions.h"
#include "Enums.h"

using json = nlohmann::json;


class TATileConstructor {

private:
    // Vector containing all the tiles that can be merged together.
    std::vector<json> tiles;

    // An integer that will be used to keep identifiers unique.
    int taNonce;

    // An integer that will be used to keep new transitions unique.
    int newTransNonce;


    /**
     * Method used to get a string representation of taNonce.
     * @return a string representation of taNonce.
     */
    [[nodiscard]] std::string taNonceStr() const
    {
        return std::to_string(taNonce);
    }


    /**
     * Method used to get a string representation of newTransNonce.
     * @return a string representation of taNonce.
     */
    [[nodiscard]] std::string newTransNonceStr() const
    {
        return std::to_string(newTransNonce);
    }


    /**
     * Method used to rename the initial location's id.
     * @param tile the tile for which to rename the initial location.
     */
    void renameInitialLocation(json &tile)
    {
        // A pointer to the initial location's id.
        json *tileInitLocPtr = TAContentExtractor::getInitialLocationNamePtr(tile);

        *tileInitLocPtr = static_cast<std::string>(*tileInitLocPtr) + taNonceStr();
    }


    /**
     * Method used to rename all the id occurrences inside the locations section of a tile.
     * @param tile a json representation of the tile in which we have to modify all the id locations occurrences.
     */
    void renameLocationsIDs(json &tile)
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
    void renameTransitions(json &tile)
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


    /**
     * Method used to rename all the id occurrences inside the tile's json representation
     * in order to avoid name conflicts when merging states into one single tile.
     */
    void renameIDs()
    {
        for (auto &tile: tiles)
        {
            renameInitialLocation(tile);
            renameLocationsIDs(tile);
            renameTransitions(tile);

            taNonce++;
        }
    }


    /**
     * Method used to merge two tiles' locations into one single tile.
     * @param sourceTile the tile which locations will be merged into the destTile tile.
     * @param destTile the tile in which locations will be merged from sourceTile tile.
     */
    static void mergeLocations(const json &sourceTile, json &destTile)
    {
        std::cout << "Now merging locations." << std::endl;

        // A json representation of the locations section of the source tile, given as a json array.
        json sourceTileLocations = getJsonAsArray(TAContentExtractor::getLocations(sourceTile));

        // A pointer to the json representation of the locations section of the destination tile.
        json *destTileLocationsPtr = getJsonPtrAsArray(TAContentExtractor::getLocationsPtr(destTile));

        for (const auto &loc: sourceTileLocations)
            destTileLocationsPtr->push_back(loc);
    }


    /**
     * Method used to merge two tiles' transitions into one single tile.
     * @param sourceTile the tile which transitions will be merged into the destTile tile.
     * @param destTile the tile in which transitions will be merged from sourceTile tile.
     */
    static void mergeTransitions(const json &sourceTile, json &destTile)
    {
        std::cout << "Now merging transitions." << std::endl;

        // A json representation of the transitions section of the source tile, given as a json array.
        json sourceTileTransitions = getJsonAsArray(TAContentExtractor::getTransitions(sourceTile));

        // A pointer to the json representation of the transitions section of the destination tile.
        json *destTileTransitionsPtr = getJsonPtrAsArray(TAContentExtractor::getTransitionsPtr(destTile));

        for (const auto &trans: sourceTileTransitions)
            destTileTransitionsPtr->push_back(trans);
    }


    /**
     * Method used to create and insert a new transition in the given destTile json.
     * @param sourceLocId the id of the transition source.
     * @param destLocId the id of the transition target.
     * @param destTile the destination tile in which the new transition will be added.
     * @param assignmentText the text appearing on the assignment of the new transition.
     */
    void insertNewTransition(const std::string &sourceLocId, const std::string &destLocId, json &destTile, const std::string &assignmentText)
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
     * Method in which two tiles are connected if the number of output locations of tile1 is equal to the number
     * of input locations of tile2. If this is the case, each output location from tile1 will be connected to an input
     * location of tile2, in the order they appear in such tiles.
     * @param tile1 the tile from which the new transitions will start, with out nodes as source.
     * @param tile2 the tile from which the new transitions will join, with out nodes as target.
     * @param destTile the destination tile in which the new transition will be added.
     * @throw ConnectTilesMatchInOutSizeException if the tile1 out locations and tile2 in locations sizes do not match.
     */
    void connectTiles_match_inout_size(const json &tile1, const json &tile2, json &destTile)
    {
        // First, let's take the ids necessary for creating a new transition.
        std::vector<std::string> tile1OutLocs = TAContentExtractor::getNamedLocations(tile1, OUT);
        std::vector<std::string> tile2InLocs = TAContentExtractor::getNamedLocations(tile2, IN);

        // If both the tile1OutLocs and tile2InLocs sizes coincide, for each location in tile1OutLocs a new transition
        // Towards a location in tile2InLocs is created, following the order in which they appear.
        if (tile1OutLocs.size() == tile2InLocs.size())
            for (int i = 0; i < tile1OutLocs.size(); i++)
                insertNewTransition(tile1OutLocs[i], tile2InLocs[i], destTile, "x = 0, y = 0");
        else
            throw ConnectTilesMatchInOutSizeException("Tiles have different In and Out size for chosen 'match_inout_size' connection method");

    }


    /**
     * Method used to connect two tiles by creating a new transition between them.
     * @param tile1 the first tile to be connected.
     * @param tile2 the second tile to be connected.
     * @param destTile the destination tile in which the new transition will be added.
     * @param method the way in which the new transition(s) will be generated.
     */
    void connectTiles(const json &tile1, const json &tile2, json &destTile, TileConstructMethod method)
    {
        switch (method)
        {
            case match_inout_size:
                connectTiles_match_inout_size(tile1, tile2, destTile);
                break;

            default:
                std::cerr << BHRED << "Not available connection method." << reset << std::endl;
                break;
        }
    }


    /**
     * Method used to call auxiliary methods that will then merge locations and transitions.
     * @param destTile the destination tile in which all the tiles will be merged.
     */
    void mergeTiles(json &destTile)
    {
        std::cout << "Starting tile merging process." << std::endl;

        // For each tile, we have to merge it with the destination one.
        for (int i = 1; i < tiles.size(); i++)
        {
            mergeLocations(tiles[i], destTile);
            mergeTransitions(tiles[i], destTile);
        }

        // For each tile couple, we have to make new transitions to let them be connected.
        for (int i = 0; i < tiles.size() - 1; i++)
            connectTiles(tiles[i], tiles[i + 1], destTile, match_inout_size);
    }


public:
    explicit TATileConstructor(std::vector<json> tiles, int taNonce = 0, int newTransNonce = 0) :
            tiles(std::move(tiles)), taNonce(taNonce), newTransNonce(newTransNonce)
    {};


    /**
     * Method used to create a TA by concatenating tiles.
     * The renaming of the ids is performed first, in order to avoid name clashes in the resulting TA.
     * The destination tile in which all the other tiles will be merged is the first contained in the 'tiles' vector,
     * that is, the one stored in position 0.
     * @return the TA derived from the composition of all the tiles taken from the 'tiles' vector.
     */
    json createTAFromTiles()
    {
        std::cout << "Renaming IDs." << std::endl;
        renameIDs();

        // Destination tile, the one in which all the other tiles will be merged.
        json destTile = tiles[0];

        // If we have 2 or more tiles, we can merge all of them together.
        if (tiles.size() > 1)
            mergeTiles(destTile);

        return destTile;
    }

};


#endif //UTOTPARSER_TATILECONSTRUCTOR_HPP
