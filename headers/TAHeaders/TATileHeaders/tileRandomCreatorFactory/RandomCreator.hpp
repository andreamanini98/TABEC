#ifndef UTOTPARSER_RANDOMCREATOR_H
#define UTOTPARSER_RANDOMCREATOR_H

#include "nlohmann/json.hpp"

using json = nlohmann::json;


class RandomCreator {

private:
    // An integer that will be used to keep new transitions unique.
    int newTransNonce {};

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


    static std::string getBlankOutLocationName()
    {
        return "idOut";
    }


    static json getBlankOutLocation()
    {
        return {{ ID,   getBlankOutLocationName() },
                { NAME, {{ TEXT, "out" }}}};
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


    json getBlankTransition(const std::string &srcLoc, const std::string &dstLoc)
    {
        json newTrans = {
                { ID,     "newTrans" + std::to_string(newTransNonce) },
                { SOURCE, {{ REF, srcLoc }}},
                { TARGET, {{ REF, dstLoc }}}
        };

        ++newTransNonce;
        return newTrans;
    }


    json getLabeledTransition(const std::string &srcLoc, const std::string &dstLoc)
    {
        json newTrans = getBlankTransition(srcLoc, dstLoc);

        newTrans[LABEL] = {
                { TEXT, "some text" },
                { KIND, GUARD }
        };

        //TODO: add text on the transitions

        return newTrans;
    }


public:
    virtual json createRandomTile() = 0;

};


#endif //UTOTPARSER_RANDOMCREATOR_H
