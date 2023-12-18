#ifndef UTOTPARSER_CONNECTORFACTORY_H
#define UTOTPARSER_CONNECTORFACTORY_H

#include "TAHeaders/TATileHeaders/tileConnectorFactory/TileConnectorFactory.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/ConnectorOnlyOneOut.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/ConnectorMatchInOutSize.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/ConnectorTreeOp.hpp"


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
    Connector *createConnector(const json &tile1, const json &tile2, json &destTile, TileTokensEnum op) override
    {
        switch (op)
        {
            case only_one_out:
                return new ConnectorOnlyOneOut(tile1, tile2, destTile);

            case match_inout_size:
                return new ConnectorMatchInOutSize(tile1, tile2, destTile);

            default:
                std::cerr << BHRED << "Not available connection method." << rstColor << std::endl;
                std::cerr << BHRED << "New transitions will not be generated." << rstColor << std::endl;
                break;
        }
    }


    /**
     * Method used to select the way in which two tiles will be connected.
     * @param tile1 the first tile to be connected.
     * @param tile2 the second tile to be connected.
     * @param tile3 the third tile to be connected.
     * @param destTile the destination tile in which the new transition will be added.
     * @param method the way in which the new transition(s) will be generated.
     * @return a proper tile connector based on the method parameter.
     */
    Connector *createConnector(const json &tile1, const json &tile2, const json &tile3, json &destTile, TileTokensEnum tk) override
    {
        switch (tk)
        {
            case tree_op:
                return new ConnectorTreeOp(tile1, tile2, tile3, destTile);

            default:
                std::cerr << BHRED << "Not available connection method." << rstColor << std::endl;
                std::cerr << BHRED << "New transitions will not be generated." << rstColor << std::endl;
                break;
        }
    }

};


#endif //UTOTPARSER_CONNECTORFACTORY_H
