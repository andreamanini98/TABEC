#ifndef UTOTPARSER_TILERANDOMCREATORFACTORY_H
#define UTOTPARSER_TILERANDOMCREATORFACTORY_H

#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/RandomCreator.hpp"


class TileRandomCreatorFactory {

public:
    virtual RandomCreator *createRandomCreator(const std::string &token) = 0;

};


#endif //UTOTPARSER_TILERANDOMCREATORFACTORY_H