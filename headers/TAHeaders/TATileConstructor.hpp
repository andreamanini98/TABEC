#ifndef UTOTPARSER_TATILECONSTRUCTOR_HPP
#define UTOTPARSER_TATILECONSTRUCTOR_HPP

#include <utility>
#include "nlohmann/json.hpp"

#include "defines/UPPAALxmlAttributes.h"
#include "TAHeaders/TAContentExtractor.hpp"
#include "utilities/JsonHelper.hpp"

using json = nlohmann::json;


class TATileConstructor {

private:
    // Vector containing all the tiles that can be merged together.
    std::vector<json> tiles;

    // An integer that will be used to keep identifiers unique.
    int taNonce;


    /**
     * Method used to get a string representation of taNonce.
     * @return a string representation of taNonce.
     */
    [[nodiscard]] std::string taNonceStr() const
    {
        return std::to_string(taNonce);
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
    static void mergeLocations(json sourceTile, json &destTile)
    {
        std::cout << "Now merging locations." << std::endl;

        // A json representation of the locations section of the source tile, given as a json array.
        json sourceTileLocations = getJsonAsArray(TAContentExtractor::getLocations(std::move(sourceTile)));

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
    static void mergeTransitions(json sourceTile, json &destTile)
    {
        std::cout << "Now merging transitions." << std::endl;

        // A json representation of the transitions section of the source tile, given as a json array.
        json sourceTileTransitions = getJsonAsArray(TAContentExtractor::getTransitions(std::move(sourceTile)));

        // A pointer to the json representation of the transitions section of the destination tile.
        json *destTileTransitionsPtr = getJsonPtrAsArray(TAContentExtractor::getTransitionsPtr(destTile));

        for (const auto &trans: sourceTileTransitions)
            destTileTransitionsPtr->push_back(trans);
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
    }


public:
    explicit TATileConstructor(std::vector<json> tiles, int taNonce = 0) : tiles(std::move(tiles)), taNonce(taNonce)
    {};


    /**
     * Method used to concatenate tiles.
     * The renaming of the ids is performed first, in order to avoid name clashes in the resulting tile.
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
