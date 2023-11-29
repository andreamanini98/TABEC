#ifndef UTOTPARSER_CONNECTORFACTORY_H
#define UTOTPARSER_CONNECTORFACTORY_H

#include "TAHeaders/TATileHeaders/tileConnectorFactory/TileConnectorFactory.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/ConnectorOnlyOneOut.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/ConnectorMatchInOutSize.hpp"


class ConnectorFactory : public TileConnectorFactory {

public:
    /**
     * Method used to select the way in which two tiles will be connected.
     * @param tile1 the first tile to be connected.
     * @param tile2 the second tile to be connected.
     * @param destTile the destination tile in which the new transition will be added.
     * @param method the way in which the new transition(s) will be generated.
     * @return a proper tile connector based on the method parameter.
     */
    Connector *createConnector(const json &tile1, const json &tile2, json &destTile, TileConstructMethod method) override
    {
        switch (method)
        {
            case only_one_out:
                return new ConnectorOnlyOneOut(tile1, tile2, destTile);

            case match_inout_size:
                return new ConnectorMatchInOutSize(tile1, tile2, destTile);

            default:
                std::cerr << BHRED << "Not available connection method." << reset << std::endl;
                std::cerr << BHRED << "New transitions will not be generated." << reset << std::endl;
                break;
        }
    }

};


#endif //UTOTPARSER_CONNECTORFACTORY_H
