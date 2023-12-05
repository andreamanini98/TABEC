#ifndef UTOTPARSER_TILECONNECTORFACTORY_H
#define UTOTPARSER_TILECONNECTORFACTORY_H

#include "TAHeaders/TATileHeaders/tileConnectorFactory/Connector.hpp"
#include "TAHeaders/TATileHeaders/TileOperatorEnum.h"


class TileConnectorFactory {

public:
    /**
     * Method used to select the way in which two tiles will be connected.
     * @param tile1 the first tile to be connected.
     * @param tile2 the second tile to be connected.
     * @param destTile the destination tile in which the new transition will be added.
     * @param method the way in which the new transition(s) will be generated.
     * @return a proper tile connector based on the method parameter.
     */
    virtual Connector *createConnector(const json &tile1, const json &tile2, json &destTile, TileOperatorEnum op) = 0;

};


#endif //UTOTPARSER_TILECONNECTORFACTORY_H
