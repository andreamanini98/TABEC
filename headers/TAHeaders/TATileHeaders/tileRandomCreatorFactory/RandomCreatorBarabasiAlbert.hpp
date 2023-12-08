#ifndef UTOTPARSER_RANDOMCREATORBARABASIALBERT_H
#define UTOTPARSER_RANDOMCREATORBARABASIALBERT_H

#include "defines/UPPAALxmlAttributes.h"
#include "TAHeaders/TAContentExtractor.hpp"
#include "TAHeaders/TATileHeaders/tileRandomCreatorFactory/RandomCreator.hpp"


class RandomCreatorBarabasiAlbert : public RandomCreator {

public:
    json createRandomTile() override
    {
        std::cout << "Creating random barabasi albert tile\n";

        json randomTA = getBlankTA();

        // Setting the initial location.
        randomTA.at(NTA).at(TEMPLATE).at(INIT).at(REF) = getBlankInitialLocationName();

        // Inserting the initial location.
        json *locationsPtr = getJsonPtrAsArray(TAContentExtractor::getLocationsPtr(randomTA));
        // Since no locations are still present, clearing makes the 'null' value disappear.
        locationsPtr->clear();
        locationsPtr->push_back(getBlankInitialLocation());

        // TODO: perform insertion of locations here in a barabasi style.

        json *transitionsPtr = getJsonPtrAsArray(TAContentExtractor::getTransitionsPtr(randomTA));
        // Since no transitions are still present, clearing makes the 'null' value disappear.
        transitionsPtr->clear();

        // TODO: perform insertion of transitions here in a barabasi style.

        // Maybe it is better to insert locations and transitions at the same time.
        // If it is the case, just rearrange the above code accordingly.

        std::cout << "\n\nResultingTiledTA:\n\n";
        std::cout << std::setw(4) << randomTA << std::endl;

        // Before returning, remember to add an output location and 2 new transitions: one from in to a random
        // location, and one from a random location to out (vedi un po' poi come fare bene sta roba).

        return randomTA;
    }

};


#endif //UTOTPARSER_RANDOMCREATORBARABASIALBERT_H
