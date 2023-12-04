#ifndef UTOTPARSER_OPERATORONLYONEOUT_H
#define UTOTPARSER_OPERATORONLYONEOUT_H

#include "nlohmann/json.hpp"

#include "DoublyLinkedList.hpp"
#include "TAHeaders/TATileHeaders/ParserNode.hpp"
#include "TAHeaders/TATileHeaders/parserOperatorFactory/Operator.hpp"
#include "TAHeaders/TATileHeaders/TATileConstructor.hpp"
#include "TAHeaders/TAContentExtractor.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/TileConnectorFactory.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/ConnectorFactory.hpp"
#include "TAHeaders/TATileHeaders/tileConnectorFactory/Connector.hpp"

using json = nlohmann::json;


class OperatorOnlyOneOut : public Operator {

public:
    explicit OperatorOnlyOneOut(DoublyLinkedList<ParserNode> &parserList) : Operator(parserList)
    {};


    /**
     * Method used to execute the match_inout_size operator.
     * The resulting tile must be pushed on top of the stack after the operator has finished its actions.
     */
    void executeOperator() override
    {
        // We take the tiles in opposite order, since we used a stack internal representation.
        json t2 = parserList.getHead()->content.tileStack.top();
        parserList.getHead()->content.tileStack.pop();

        json t1 = parserList.getHead()->content.tileStack.top();
        parserList.getHead()->content.tileStack.pop();

        // The tile in which the merge result will be stored.
        // In this way, t1 will be on the left and t2 will be on the right of the resulting tile.
        json destTile = t1;

        // For each tile required by the operator, we merge the locations and transitions into the destination one.
        TATileConstructor::mergeLocations(t2, destTile);
        TATileConstructor::mergeTransitions(t2, destTile);

        TileConnectorFactory *tileConnectorFactory = new ConnectorFactory;
        Connector *connector = tileConnectorFactory->createConnector(t1, t2, destTile, only_one_out);
        connector->connectTiles();

        // Gathering the names of the locations which in and out locations' names will be deleted.
        // If we assume t1 will be on the right and t2 on the left, then for t1, 'out' locations' names have to be
        // deleted, while for t2, 'in' locations' names have to be deleted.
        // This is because the resulting tile must have as 'in' locations the ones of the tile on its left, while
        // as 'out' locations the ones of the tile on itr right.
        deleteLocName(destTile, TAContentExtractor::getNamedLocations(t2, IN), IN);
        deleteLocName(destTile, TAContentExtractor::getNamedLocations(t1, OUT), OUT);

        parserList.getHead()->content.tileStack.push(destTile);
    }

};


#endif //UTOTPARSER_OPERATORONLYONEOUT_H
