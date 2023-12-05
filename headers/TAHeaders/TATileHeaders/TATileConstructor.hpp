#ifndef UTOTPARSER_TATILECONSTRUCTOR_HPP
#define UTOTPARSER_TATILECONSTRUCTOR_HPP

#include <utility>
#include "nlohmann/json.hpp"

#include "defines/UPPAALxmlAttributes.h"
#include "TAHeaders/TAContentExtractor.hpp"
#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/Connector.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/ConnectorFactory.hpp"
#include "utilities/JsonHelper.hpp"
#include "Exceptions.h"

using json = nlohmann::json;

//TODO: even if you do not explicitly use the tiles name, since you have it, you can pass it to the constructors so that they can
// Better log eventual errors.

class TATileConstructor {

private:
    // Vector containing all the tiles that can be merged together.
    std::vector<std::pair<std::string, json>> tiles {};


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
            mergeLocations(tiles[i].second, destTile);
            mergeTransitions(tiles[i].second, destTile);
        }

        TileConnectorFactory *tileConnectorFactory = new ConnectorFactory;

        // For each tile couple, we have to make new transitions to let them be connected.
        for (int i = 0; i < tiles.size() - 1; i++)
        {
            Connector *connector;
            connector = tileConnectorFactory->createConnector(tiles[i].second, tiles[i + 1].second, destTile, only_one_out);
            connector->connectTiles();
        }
    }


public:
    TATileConstructor() = default;


    explicit TATileConstructor(std::vector<std::pair<std::string, json>> tiles) : tiles(std::move(tiles))
    {};


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
     * Method used to create a TA by concatenating tiles.
     * The renaming of the ids is performed first, in order to avoid name clashes in the resulting TA.
     * The destination tile in which all the other tiles will be merged is the first contained in the 'tiles' vector,
     * that is, the one stored in position 0.
     * @return the TA derived from the composition of all the tiles taken from the 'tiles' vector.
     */
    json createTAFromTiles()
    {
        std::cout << "Renaming IDs." << std::endl;
        TATileRenamer::renameIDs(tiles);

        // Destination tile, the one in which all the other tiles will be merged.
        json destTile = tiles[0].second;

        // If we have 2 or more tiles, we can merge all of them together.
        if (tiles.size() > 1)
            mergeTiles(destTile);

        return destTile;
    }

};


#endif //UTOTPARSER_TATILECONSTRUCTOR_HPP
