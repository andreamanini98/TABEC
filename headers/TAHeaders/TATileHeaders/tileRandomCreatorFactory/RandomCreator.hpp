#ifndef UTOTPARSER_RANDOMCREATOR_H
#define UTOTPARSER_RANDOMCREATOR_H

#include <cassert>
#include <random>
#include "nlohmann/json.hpp"

#include "utilities/Utils.hpp"

using json = nlohmann::json;


class RandomCreator {

private:
    // The minimum constant appearing in guards.
    int minConst { 0 };

    // The maximum constant appearing in guards.
    int maxConst { 5 };

    // Vector containing semantically meaningful clock guards.
    // Note that 'const' can also be the keyword 'param'.
    std::vector<std::string> clockGuards
            {
                    "ck eq const",         // e.g. x == 2
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
                assignmentString = subSinS(clockAssignment, "ck", clock);
                break;

            case 1: // Both clocks assigned.
                assignmentString = subSinS(clockAssignment, "ck", "x");
                assignmentString.append("; " + subSinS(clockAssignment, "ck", "y"));
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
     * Method used to convert 'lt', 'gt' and 'eq' into their corresponding mathematical symbol.
     * @param result the string that will be modified, transforming 'lt', 'gt' and 'eq' in their corresponding mathematical symbol.
     * @param gen a random number generator.
     */
    void sanitizeComparisonOperator(std::string &result, std::mt19937 &gen)
    {
        // Uniform distribution used to select randomly between different choices of comparison operators.
        std::uniform_int_distribution<int> int_dist_bool(0, 1);

        if (result.find("eq") != std::string::npos)
            result = subSinS(result, "eq", "==");

        else if (result.find("gt") != std::string::npos)
            result = subSinS(result, "gt", gtChoices[int_dist_bool(gen)]);

        else
            // This handles both the cases in which the guard have one or two 'lt' comparison operators.
            while (result.find("lt") != std::string::npos)
                result = subSinS(result, "lt", ltChoices[int_dist_bool(gen)]);
    }


    /**
     * Method used to convert 'const' into an actual constant or into the parameter keyword 'param'.
     * With the current implementation, if only one 'const' keyword in contained in 'result',
     * the maximum value of the constant will be equal to 'maxConst' - 1.
     * @param result the string that will be modified, transforming 'const' into an actual constant or into the parameter keyword 'param'.
     * @param gen a random number generator.
     */
    void sanitizeConstants(std::string &result, std::mt19937 &gen) const
    {
        // Uniform distribution used to select if actual constants or the 'param' keyword have to be put in the guard.
        std::uniform_int_distribution<int> int_dist_bool(0, 1);

        // Uniform distribution used to generate the first random constant.
        // Here we subtract 1 from maxConst just in case we have to create 2 different constants.
        std::uniform_int_distribution<int> int_dist_const1(minConst, maxConst - 1);

        int const1 { int_dist_const1(gen) };

        // The probability of putting the 'param' keyword instead of an actual constant.
        int paramProbability { int_dist_bool(gen) };

        result = subSinS(result, "const", paramProbability ? "param" : std::to_string(const1));

        // Case in which another 'const' keyword is contained in the 'result' string.
        if (result.find("const") != std::string::npos)
        {
            // Uniform distribution used to generate the second random constant.
            std::uniform_int_distribution<int> int_dist_const2(const1 + 1, maxConst);

            int const2 { int_dist_const2(gen) };

            // If the first constant has been set to 'param', the second constant will not be set to 'param' again.
            paramProbability = int_dist_bool(gen) * (1 - paramProbability);

            result = subSinS(result, "const", paramProbability ? "param" : std::to_string(const2));
        }
    }


    /**
     * Method used to sanitize a guard, substituting all the keyword occurrences contained in the vectors with
     * symbols that are compliant with the tChecker's syntax.
     * @param guardString the string to sanitize.
     * @param clock the name of the clock to be used in the current guard.
     * @param gen a random number generator.
     * @return a sanitized version of the given 'guardString'.
     */
    std::string sanitizeGuard(const std::string &guardString, const std::string &clock, std::mt19937 &gen)
    {
        assert(static_cast<int>(ltChoices.size()) == static_cast<int>(gtChoices.size()) && static_cast<int>(ltChoices.size()) == 2);

        // Now substituting 'ck' with the actual clock.
        std::string result = subSinS(guardString, "ck", clock);

        // Now substituting the comparison operators.
        sanitizeComparisonOperator(result, gen);

        // Now substituting the constants.
        sanitizeConstants(result, gen);

        return result;
    }


    /**
     * Method used to handle the case where a guard only has one clock involved and there may be the possibility
     * of having an assignment for the other clock, hence preserving the nrt property.
     * @param newTrans the transition in which to add the guard and possibly the assignment.
     * @param gena random number generator.
     */
    void handleSwitchCase1(json &newTrans, std::mt19937 &gen)
    {
        // Uniform distribution used to select the shape of the guard.
        std::uniform_int_distribution<int> int_dist(0, static_cast<int>(clockGuards.size()) - 1);

        // Uniform distribution used to select the clocks and if to put also an assignment.
        std::uniform_int_distribution<int> int_dist_bool(0, static_cast<int>(ckChoices.size()) - 1);

        int clock { int_dist_bool(gen) };

        std::string guardString {};
        std::string assignmentString {};

        guardString = sanitizeGuard(clockGuards[int_dist(gen)], ckChoices[clock], gen);

        // Deciding if to put a guard for the other clock, hence keeping the nrt condition.
        int alsoPutAssignment = int_dist_bool(gen);

        if (alsoPutAssignment)
        {
            assignmentString = subSinS(clockAssignment, "ck", ckChoices[1 - clock]);

            newTrans[LABEL] = {
                    {{ TEXT, guardString },      { KIND, GUARD }},
                    {{ TEXT, assignmentString }, { KIND, ASSIGNMENT }}
            };
        } else
        {
            newTrans[LABEL] = {
                    { TEXT, guardString },
                    { KIND, GUARD }
            };
        }
    }


    /**
     * Method used to handle the case where only a guard involving both clocks should be added in the new transitions.
     * @param newTrans the transition in which to add the guard.
     * @param gen a random number generator.
     */
    void handleSwitchCase2(json &newTrans, std::mt19937 &gen)
    {
        // Uniform distribution used to select the shape of the guard.
        std::uniform_int_distribution<int> int_dist(0, static_cast<int>(clockGuards.size()) - 1);

        // Uniform distribution used to select the clocks.
        std::uniform_int_distribution<int> int_dist_bool(0, static_cast<int>(ckChoices.size()) - 1);

        int clock { int_dist_bool(gen) };

        std::string guardString {
                sanitizeGuard(clockGuards[int_dist(gen)], ckChoices[clock], gen)
                + " && "
                + sanitizeGuard(clockGuards[int_dist(gen)], ckChoices[1 - clock], gen)
        };

        newTrans[LABEL] = {
                { TEXT, guardString },
                { KIND, GUARD }
        };
    }


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
     * Bounds are set to [0, inf) since they're not known for a randomly-generated tile.
     * @return a json representing an empty tile.
     */
    static json getBlankTA()
    {
        json blankTA;
        blankTA[NTA][DECLARATION] = "bound:0:inf";
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

        // Uniform distribution used to select the shape of the transition.
        std::uniform_int_distribution<int> int_dist(0, 2);

        int labelSeed = int_dist(gen);

        switch (labelSeed)
        {
            case 0: // No clocks in the guard.
                handleSwitchCase0(newTrans, gen);
                break;

            case 1: // Only one clock in the guard.
                handleSwitchCase1(newTrans, gen);
                break;

            default: // Both clocks in the guard.
                handleSwitchCase2(newTrans, gen);
                break;
        }

        return newTrans;
    }


public:
    virtual json createRandomTile() = 0;

};


#endif //UTOTPARSER_RANDOMCREATOR_H
