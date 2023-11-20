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
    // Vector containing all the tiles that must be merged together.
    std::vector<json> tiles;

    // An integer that will be used to keep identifiers unique.
    int taNonce;


    /**
     * Helper method used to get a json pointer to the tile's locations section, which content is structured as a json array.
     * Having a json array enables to perform array operations on such json data.
     * @param tile the tile for which to get a pointer to the location's section, structuring its content as a json array.
     * @return a pointer pointing to the locations section, having its content structured as a json array.
     */
    static json *helper_getLocationsPtrAsArray(json &tile)
    {
        // A pointer to the json representation of the locations section of the considered tile.
        json *tileLocationPtr = TAContentExtractor::getLocationsPtr(tile);

        // If the tile pointer's content is not an array, we have to make it an array.
        if (!tileLocationPtr->is_array())
            *tileLocationPtr = getJsonAsArray(*tileLocationPtr);

        return tileLocationPtr;
    }


    /**
     * Method used to rename all the id occurrences inside the locations section of a tile.
     * @param tile a json representation of the tile in which we have to modify all the id occurrences.
     */
    void renameLocationsIDs(json &tile) const
    {
        // A pointer to the json representation of the locations section of the considered tile.
        json *tileLocationPtr = helper_getLocationsPtrAsArray(tile);

        for (auto &loc: *tileLocationPtr)
            loc.at(ID) = static_cast<std::string>(loc.at(ID)) + std::to_string(taNonce);
    }

    // TODO: keep this updated (transitions are still missing).
    //       for transitions you will have to modify:
    //       - the id of the transition itself.
    //       - the source and ref id of the transition.
    // You'll also need to update the ref. Also, how will you manage the initial locations???
    /**
     * Method used to rename all the id occurrences inside the tile's json representation in order to get
     * no name conflicts when merging states into one single tile.
     */
    void renameIDs()
    {
        for (auto &tile: tiles)
        {
            renameLocationsIDs(tile);

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
        // A json representation of the locations section of the source tile, given as a json array.
        json sourceTileLocations = getJsonAsArray(TAContentExtractor::getLocations(std::move(sourceTile)));

        // A pointer to the json representation of the locations section of the destination tile.
        json *destTileLocationsPtr = helper_getLocationsPtrAsArray(destTile);

        for (const auto &loc: sourceTileLocations)
            destTileLocationsPtr->push_back(loc);
    }


public:
    explicit TATileConstructor(std::vector<json> tiles, int taNonce = 0) : tiles(std::move(tiles)), taNonce(taNonce)
    {};

    // TODO: keep this method description updated.
    // TODO: maybe we can overload this method so that one can also decide to merge only two tiles or an arbitrary number of tiles.
    /**
     * Method used to concatenate tiles.
     */
    void concatenateTiles()
    {
        std::cout << "Renaming IDs." << std::endl;
        renameIDs();

        std::cout << "Entering concatenate tiles." << std::endl;

        // Destination tile, the one in which all the other tiles will be merged.
        json destTile = tiles[0];

        std::cout << "Now merging locations." << std::endl;

        // For each tile, we have to merge it with the destination one.
        for (int i = 1; i < tiles.size(); i++)
            mergeLocations(tiles[i], destTile);

        std::cout << std::setw(4) << destTile << std::endl;
    }

};

#endif //UTOTPARSER_TATILECONSTRUCTOR_HPP
