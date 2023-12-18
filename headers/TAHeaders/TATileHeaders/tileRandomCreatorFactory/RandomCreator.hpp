#ifndef UTOTPARSER_RANDOMCREATOR_H
#define UTOTPARSER_RANDOMCREATOR_H

#include "nlohmann/json.hpp"

using json = nlohmann::json;


class RandomCreator {

protected:
    static std::string getBlankInitialLocationName()
    {
        return "idInit";
    }


    static json getBlankInitialLocation()
    {
        return {{ ID,    getBlankInitialLocationName() },
                { LABEL, {{ TEXT, "x = 0; y = 0" }, { KIND, COMMENTS }}},
                { NAME,  {{ TEXT, "in" }}}};
    }


    static json getBlankTA()
    {
        json blankTA;
        blankTA[NTA][DECLARATION] = "BlankTA";
        blankTA[NTA][TEMPLATE][DECLARATION] = "clock x, y;";
        blankTA[NTA][TEMPLATE][INIT][REF] = {};
        blankTA[NTA][TEMPLATE][LOCATION] = {};
        blankTA[NTA][TEMPLATE][TRANSITION] = {};

        return blankTA;
    }


public:
    virtual json createRandomTile() = 0;

};


#endif //UTOTPARSER_RANDOMCREATOR_H
