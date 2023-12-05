#ifndef UTOTPARSER_CONNECTORTREEOP_H
#define UTOTPARSER_CONNECTORTREEOP_H

#include "nlohmann/json.hpp"

using json = nlohmann::json;

class ConnectorTreeOp : public Connector {

private:
    // The tile from which the new transitions will join, with out nodes as target.
    const json &tile3;

public:
    ConnectorTreeOp(const json &tile1, const json &tile2, const json &tile3, json &destTile) :
            Connector(tile1, tile2, destTile), tile3(tile3)
    {};


    /**
     * Method in which three tiles are connected if the number of output locations of tile1 is equal to 2 and the
     * number of input locations of both tile2 and tile3 is equal to 1.
     * If this is the case, each output location from tile1 will be connected to an input
     * location of tile2 and tile 3, in order to build a tree structure.
     * tile1 the tile from which the new transitions will start, with out nodes as source.
     * tile2 the tile from which the new transitions will join, with in nodes as target.
     * tile3 the tile from which the new transitions will join, with in nodes as target.
     * destTile the destination tile in which the new transition will be added.
     * @throw ConnectorException if the tile1 has not exactly out 2 locations and both tile2 and tile3 have not exactly 1 in locations.
     */
    void connectTiles() override
    {
        // First, let's take the ids necessary for creating a new transition.
        std::vector<std::string> tile1OutLocs = TAContentExtractor::getNamedLocations(tile1, OUT);
        std::vector<std::string> tile2InLocs = TAContentExtractor::getNamedLocations(tile2, IN);
        std::vector<std::string> tile3InLocs = TAContentExtractor::getNamedLocations(tile3, IN);

        // If the tile1OutLocs size is equal to 2 and both tile2InLocs and tile3InLocs sizes are equal to 1, we can build a tree.
        if (tile1OutLocs.size() == 2 && tile2InLocs.size() == 1 && tile3InLocs.size() == 1)
        {
            std::string transitionText {};

            transitionText = TAContentExtractor::getLocationLabelText(tile2, tile2InLocs[0], COMMENTS);
            insertNewTransition(tile1OutLocs[0], tile2InLocs[0], transitionText, ASSIGNMENT);

            transitionText = TAContentExtractor::getLocationLabelText(tile3, tile3InLocs[0], COMMENTS);
            insertNewTransition(tile1OutLocs[1], tile3InLocs[0], transitionText, ASSIGNMENT);
        } else
            throw ConnectorException("Exception: cannot build tree for chosen 'tree_op' connection method");
    }

};


#endif //UTOTPARSER_CONNECTORTREEOP_H
