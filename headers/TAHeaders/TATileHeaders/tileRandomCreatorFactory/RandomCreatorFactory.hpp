#ifndef UTOTPARSER_RANDOMCREATORFACTORY_H
#define UTOTPARSER_RANDOMCREATORFACTORY_H

#include "TAHeaders/TATileHeaders/TileTokensEnum.h"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/TileRandomCreatorFactory.hpp"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/RandomCreator.hpp"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/RandomCreatorBarabasiAlbert.hpp"


class RandomCreatorFactory : public TileRandomCreatorFactory {

public:
    RandomCreator *createRandomCreator(const std::string &token) override
    {
        TileTokensEnum tk = fromStrTileTokenEnum(token);

        switch (tk)
        {
            case t_barabasi_albert:
                return new RandomCreatorBarabasiAlbert();

            default:
                std::cerr << BHRED << "Not available token." << reset << std::endl;
                std::cerr << BHRED << "Random tile will not be generated. This may cause undefined behaviour." << reset << std::endl;
                break;
        }
    }

};


#endif //UTOTPARSER_RANDOMCREATORFACTORY_H
