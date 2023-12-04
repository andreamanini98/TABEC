#ifndef UTOTPARSER_ACTIONFACTORY_H
#define UTOTPARSER_ACTIONFACTORY_H

#include "TAHeaders/TATileHeaders/parserActionFactory/ActionPushOperator.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionPushTile.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ParserActionFactory.hpp"
#include "DoublyLinkedList.hpp"
#include "Enums.h"


class ActionFactory : public ParserActionFactory {

private:
    StringsGetter &stringsGetter;

    // A vector of pairs defining in the second element the actual tile identifier appearing in the
    // compositional tile string, while in the first element the tile token corresponding to that symbol.
    std::vector<std::pair<std::string, std::string>> &availableTiles;


    /**
     * Method used to check if a given token corresponds to a tile's name.
     * This is done since we want to keep separate the enumeration and the way in which the available tiles are gathered.
     * @param token the token to check if it is corresponding to a tile's name.
     * @return true if 'token' corresponds to a tile, false otherwise.
     */
    bool checkIfTile(const std::string &token)
    {
        return std::any_of(
                availableTiles.begin(), availableTiles.end(),
                [token](const auto &availableTile) {
                    return availableTile.first == token;
                });
    }


public:
    ActionFactory(StringsGetter &stringsGetter, std::vector<std::pair<std::string, std::string>> &availableTiles) :
            stringsGetter(stringsGetter), availableTiles(availableTiles)
    {};


    /**
     * Method used to select which action has to be executed based on the given 'token' parameter.
     * @param parserList the data structure containing the tiles and operators needed by the parser.
     * @param token the token based on which the action will be executed.
     * @return an action that will affect the 'parserList' parameter based on the given 'token' parameter.
     */
    Action *createAction(DoublyLinkedList<ParserNode> &parserList, const std::string &token) override
    {
        TileConstructMethod method = fromStrTileConstructMethod(token);

        switch (method)
        {
            case only_one_out:
            case match_inout_size:
                return new ActionPushOperator(stringsGetter, parserList, token);

            case maybe_tile:
                if (checkIfTile(token))
                    return new ActionPushTile(stringsGetter, parserList, token);

            default:
                std::cerr << BHRED << "Not available token." << reset << std::endl;
                std::cerr << BHRED << "Action will not be performed. This may cause unexpected behaviour" << reset << std::endl;
                break;
        }
    }

};


#endif //UTOTPARSER_ACTIONFACTORY_H
