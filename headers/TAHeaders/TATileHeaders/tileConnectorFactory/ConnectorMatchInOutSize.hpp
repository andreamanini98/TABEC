#ifndef UTOTPARSER_CONNECTORINOUT_H
#define UTOTPARSER_CONNECTORINOUT_H

#include "nlohmann/json.hpp"

#include "TAHeaders/TAContentExtractor.hpp"
#include "Exceptions.h"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/Connector.hpp"

using json = nlohmann::json;


class ConnectorMatchInOutSize : public Connector {

private:


public:
    ConnectorMatchInOutSize(const json &tile1, const json &tile2, json &destTile) : Connector(tile1, tile2, destTile)
    {};


    /**
     * Method in which two tiles are connected if the number of output locations of tile1 is equal to the number
     * of input locations of tile2. If this is the case, each output location from tile1 will be connected to an input
     * location of tile2, in the order they appear in such tiles.
     * @param tile1 the tile from which the new transitions will start, with out nodes as source.
     * @param tile2 the tile from which the new transitions will join, with out nodes as target.
     * @param destTile the destination tile in which the new transition will be added.
     * @throw ConnectTilesMatchInOutSizeException if the tile1 out locations and tile2 in locations sizes do not match.
     */
    void connectTiles() override
    {
        // First, let's take the ids necessary for creating a new transition.
        std::vector<std::string> tile1OutLocs = TAContentExtractor::getNamedLocations(tile1, OUT);
        std::vector<std::string> tile2InLocs = TAContentExtractor::getNamedLocations(tile2, IN);

        // If both the tile1OutLocs and tile2InLocs sizes coincide, for each location in tile1OutLocs a new transition
        // Towards a location in tile2InLocs is created, following the order in which they appear.
        if (tile1OutLocs.size() == tile2InLocs.size())
            for (int i = 0; i < tile1OutLocs.size(); i++)
            {
                std::string transitionText { TAContentExtractor::getLocationLabelText(tile2, tile2InLocs[i], COMMENTS) };
                insertNewTransition(tile1OutLocs[i], tile2InLocs[i], transitionText, ASSIGNMENT);
            }
        else
            throw ConnectTilesMatchInOutSizeException("Tiles have different In and Out size for chosen 'match_inout_size' connection method");
    }

};


#endif //UTOTPARSER_CONNECTORINOUT_H
