#ifndef UTOTPARSER_ACTIONFACTORY_H
#define UTOTPARSER_ACTIONFACTORY_H

#include "TAHeaders/TATileHeaders/parserActionFactory/ActionPushOperator.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionPushTile.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ParserActionFactory.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionLParen.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionRParen.hpp"
#include "TAHeaders/TATileHeaders/parserActionFactory/ActionPushRandomTile.hpp"
#include "TAHeaders/TATileHeaders/lexerAndParser/ParserNode.hpp"
#include "DoublyLinkedList.hpp"
#include "TAHeaders/TATileHeaders/tileEnums/TileTokensEnum.h"
#include "TAHeaders/TATileHeaders/tileEnums/TileTypeEnum.h"


class ActionFactory : public ParserActionFactory {

private:
    StringsGetter &stringsGetter;

    // A vector of pairs which content is:
    // 1) The type of the tile.
    // 2) A vector of pairs defining in the second element the actual tile identifier appearing in the
    //    compositional tile string, while in the first element the tile token corresponding to that symbol.
    std::vector<std::pair<TileTypeEnum, std::vector<std::pair<std::string, std::string>>>> &availableTiles;


    /**
     * Method used to check if a given token corresponds to a tile's name.
     * This is done since we want to keep separate the enumeration and the way in which the available tiles are gathered.
     * @param token the token to check if it is corresponding to a tile's name.
     * @param tiles a vector of pairs defining in the second element the actual tile identifier appearing in the
     *              compositional tile string, while in the first element the tile token corresponding to that symbol.
     * @return true if 'token' corresponds to a tile, false otherwise.
     */
    static bool checkIfTile(const std::string &token, const std::vector<std::pair<std::string, std::string>> &tiles)
    {
        return std::any_of(
                tiles.begin(), tiles.end(),
                [token](const auto &tile) {
                    return tile.first == token;
                });
    }


    /**
     * Method used to tell if a given token is a tile, checking every type of available tiles as specified in
     * the availableTiles vector, gathered from the lexer.
     * The returned value is a pair, containing:
     * 1) The type of the tile.
     * 2) True if the token corresponds to a tile, false otherwise.
     * Note that, if false must be returned, the 'notAType' dummy value is inserted in the first position of the pair.
     * @param token the token to check if it is corresponding to a tile's name.
     * @return a pair as described above.
     */
    std::pair<TileTypeEnum, bool> checkIfTileForType(const std::string &token)
    {
        // For each element of 'availableTiles', the check must be carried out.
        for (auto &tileVec: availableTiles)
            if (checkIfTile(token, tileVec.second))
                return std::make_pair(tileVec.first, true);

        // If no tile corresponds to the given token, false is returned.
        return std::make_pair(notAType, false);
    }


public:
    ActionFactory(StringsGetter &stringsGetter, std::vector<std::pair<TileTypeEnum, std::vector<std::pair<std::string, std::string>>>> &availableTiles) :
            stringsGetter(stringsGetter), availableTiles(availableTiles)
    {};


    /**
     * Method used to select which action has to be executed based on the given 'token' parameter.
     * @param parserList the data structure containing the tiles and operators needed by the parser.
     * @param token the token based on which the action will be executed.
     * @return an action that will affect the 'parserList' parameter based on the given 'token' parameter.
     */
    Action *createAction(DoublyLinkedList<ParserNode> &parserList, const std::string &token, int syntaxParameter) override
    {
        TileTokensEnum tk = fromStrTileTokenEnum(token);

        switch (tk)
        {
            case only_one_out:
            case match_inout_size:
            case tree_op:
                return new ActionPushOperator(stringsGetter, parserList, token);

            case lparen:
                return new ActionLParen(stringsGetter, parserList, token);

            case rparen:
                return new ActionRParen(stringsGetter, parserList, token);

            case t_barabasi_albert:
                return new ActionPushRandomTile(stringsGetter, parserList, token, syntaxParameter);

            case maybe_tile:
            {
                std::pair<TileTypeEnum, bool> isTile { checkIfTileForType(token) };
                if (isTile.second)
                    return new ActionPushTile(stringsGetter, parserList, token, isTile.first);
            }

            default:
                std::cerr << BHRED << "Not available token." << rstColor << std::endl;
                std::cerr << BHRED << "Action will not be performed. This may cause unexpected behaviour" << rstColor << std::endl;
                break;
        }
    }

};


#endif //UTOTPARSER_ACTIONFACTORY_H
