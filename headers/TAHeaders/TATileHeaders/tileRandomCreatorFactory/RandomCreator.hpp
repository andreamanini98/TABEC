#ifndef UTOTPARSER_RANDOMCREATOR_H
#define UTOTPARSER_RANDOMCREATOR_H

#include <random>
#include "nlohmann/json.hpp"

#include "utilities/Utils.hpp"

using json = nlohmann::json;


class RandomCreator {

private:
    // Vector containing semantically meaningful clock guards.
    // Note that 'const' can also be the keyword 'param'.
    std::vector<std::string> clockGuards
            {
                    "ck lt const",         // e.g. x <= 3
                    "ck gt const",         // e.g. y >= param
                    "const lt ck lt const" // e.g. 2 < x <= param
            };

    // Semantically meaningful string representing a clock assignment (a reset).
    std::string clockAssignment { "ck = 0" };

    // Vector containing possible choices for clock names.
    std::vector<std::string> ckChoices
            {
                    "x",
                    "y"
            };

    // Vector containing possible choices for 'less than' operators.
    std::vector<std::string> ltChoices
            {
                    "<",
                    "<="
            };

    // Vector containing possible choices for 'greater than' operators.
    std::vector<std::string> gtChoices
            {
                    ">",
                    ">="
            };

    // An integer that will be used to keep new transitions unique.
    int newTransNonce {};


protected:
    /**
     * Method used to get a string representation of the initial location name.
     * Its name should differ from 'IdX', with X an integer, due to the random generation algorithms implemented.
     * @return a string representing the initial location name.
     */
    static std::string getBlankInitialLocationName()
    {
        return "idInit";
    }


    /**
     * Method used to return a json representation of the initial location.
     * Note that it will also be marked as a 'in' location for the tile.
     * @return a json representation of the initial 'in' location of the tile.
     */
    static json getBlankInitialLocation()
    {
        return {{ ID,    getBlankInitialLocationName() },
                { LABEL, {{ TEXT, "x = 0; y = 0" }, { KIND, COMMENTS }}},
                { NAME,  {{ TEXT, "in" }}}};
    }


    /**
     * Method used to get a string representation of the 'out' location name.
     * Its name should differ from 'IdX', with X an integer, due to the random generation algorithms implemented.
     * @return a string representing the 'out' location name.
     */
    static std::string getBlankOutLocationName()
    {
        return "idOut";
    }


    /**
     * Method used to return a json representation of the 'out' location.
     * Note that it will also be marked as a 'out' location for the tile.
     * @return a json representation of the initial 'out' location of the tile.
     */
    static json getBlankOutLocation()
    {
        return {{ ID,   getBlankOutLocationName() },
                { NAME, {{ TEXT, "out" }}}};
    }


    /**
     * Method used to get a blueprint of an empty tile.
     * In order to get rid of null values, the corresponding fields should be cleared first (with the .clear() method).
     * @return a json representing an empty tile.
     */
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


    /**
     * Method used to get a transition without any label (guard or assignment) on it.
     * @param srcLoc the source location of the transition.
     * @param dstLoc the destination location of the transition.
     * @return a json representation of a transition from 'srcLoc' to 'dstLoc.
     */
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

    // x < p
    // y < p

    // x > p
    // y > p

    // p1 < x < p2
    // p1 < y < p2

    // The same but with equalities


    /**
     * Method used to handle the case where only assignments should be added in the new transitions.
     * @param newTrans the transition in which to add the assignments.
     * @param gen a random number generator.
     */
    void handleSwitchCase0(json &newTrans, std::mt19937 &gen)
    {
        // Uniform distribution used to select the shape of the assignment.
        std::uniform_int_distribution<int> int_dist_seed(0, 2);

        // Uniform distribution used to extract a value from ckChoices vector.
        std::uniform_int_distribution<int> int_dist(0, static_cast<int>(ckChoices.size()) - 1);

        std::string clock {};
        std::string assignmentString {};

        int assignmentSeed = int_dist_seed(gen);

        switch (assignmentSeed)
        {
            case 0: // Only one clock assigned.
                // Picking a random value from ckChoices vector.
                clock = { ckChoices[int_dist(gen)] };
                assignmentString = substituteStringInString(clockAssignment, "ck", clock);
                break;

            case 1: // Both clocks assigned.
                assignmentString = substituteStringInString(clockAssignment, "ck", "x");
                assignmentString.append("; " + substituteStringInString(clockAssignment, "ck", "y"));
                break;

            default: // No clocks assigned.
                break;
        }

        // If some clock has been assigned, the label must be assigned to the transition.
        if (assignmentSeed <= 1)
        {
            newTrans[LABEL] = {
                    { TEXT, assignmentString },
                    { KIND, ASSIGNMENT }
            };
        }
    }


    /**
     * Method used to get a transition labeled with guards and assignments (if any).
     * The respective guard or assignment is randomly generated.
     * In addition, the construction preserves the nrt property of TAs.
     * @param srcLoc the source location of the transition.
     * @param dstLoc the destination location of the transition.
     * @param gen a random number generator.
     * @return a json representation of a transition from 'srcLoc' to 'dstLoc with guards and assignments (if any).
     */
    json getLabeledTransition(const std::string &srcLoc, const std::string &dstLoc, std::mt19937 &gen)
    {
        json newTrans = getBlankTransition(srcLoc, dstLoc);

        std::uniform_int_distribution<int> int_dist(0, 2);
        int labelSeed = int_dist(gen);

        switch (labelSeed)
        {
            case 0: // No clocks in the guard.
                handleSwitchCase0(newTrans, gen);
                break;

            case 1: // Only one clock in the guard.
                newTrans[LABEL] = {
                        { TEXT, "1" },
                        { KIND, GUARD }
                }; // TO HAVE BOTH ASSIGNMENTS AND GUARDS, YOU HAVE TO INSERT AN ARRAY AS A LABEL, GUARDA UN JSON DI ESEMPIO PER VEDERE COME FARE
                break;

            default: // Both clocks in the guard.
                newTrans[LABEL] = {
                        { TEXT, "2" },
                        { KIND, GUARD }
                };
                break;
        }

        return newTrans;
    }


public:
    virtual json createRandomTile() = 0;

};


#endif //UTOTPARSER_RANDOMCREATOR_H
