#ifndef UTOTPARSER_RANDOMCREATORBARABASIALBERT_H
#define UTOTPARSER_RANDOMCREATORBARABASIALBERT_H

#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/RandomCreator.hpp"


class RandomCreatorBarabasiAlbert : public RandomCreator {

public:
    json createRandomTile() override
    {
        std::cout << "Creating random barabasi albert tile\n";
        return json {};
    }

};


#endif //UTOTPARSER_RANDOMCREATORBARABASIALBERT_H
