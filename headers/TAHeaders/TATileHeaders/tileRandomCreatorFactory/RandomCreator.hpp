#ifndef UTOTPARSER_RANDOMCREATOR_H
#define UTOTPARSER_RANDOMCREATOR_H

#include "nlohmann/json.hpp"

using json = nlohmann::json;


class RandomCreator {

public:
    virtual json createRandomTile() = 0;

};


#endif //UTOTPARSER_RANDOMCREATOR_H
