#ifndef UTOTPARSER_CONNECTORONLYONEOUT_HPP
#define UTOTPARSER_CONNECTORONLYONEOUT_HPP

#include "nlohmann/json.hpp"

#include "TAHeaders/TAContentExtractor.hpp"
#include "Exceptions.h"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/Connector.hpp"

using json = nlohmann::json;


class ConnectorOnlyOneOut : public Connector {

public:
    ConnectorOnlyOneOut(const json &tile1, const json &tile2, json &destTile) : Connector(tile1, tile2, destTile)
    {}


    /**
     * Method in which the first out location of tile1 is connected to the first in location of tile2.
     * tile1 the tile from which the new transitions will start, with first out node as source.
     * tile2 the tile from which the new transitions will join, with first in node as target.
     * destTile the destination tile in which the new transition will be added.
     * @throw ConnectorException if either tile1 or tile2 does not have respectively out or in locations.
     */
    void connectTiles() override
    {
        // First, let's take the ids necessary for creating a new transition.
        std::vector<std::string> tile1OutLocs = TAContentExtractor::getNamedLocations(tile1, OUT);
        std::vector<std::string> tile2InLocs = TAContentExtractor::getNamedLocations(tile2, IN);

        // If both the tile1OutLocs and tile2InLocs size it at least one, we connect the first out location of
        // tile1 to the first in location of tile2.
        if (!(tile1OutLocs.empty() || tile2InLocs.empty()))
        {
            std::string transitionText { TAContentExtractor::getLocationLabelText(tile2, tile2InLocs[0], COMMENTS) };
            insertNewTransition(tile1OutLocs[0], tile2InLocs[0], transitionText, ASSIGNMENT);
        } else
            throw ConnectorException("Exception: either tile1 or tile2 does not have respectively out or in locations for chosen 'only_one_out' connection method");
    }

};


#endif //UTOTPARSER_CONNECTORONLYONEOUT_HPP
