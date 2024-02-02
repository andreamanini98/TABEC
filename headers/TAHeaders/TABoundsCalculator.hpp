#ifndef UTOTPARSER_TABOUNDSCALCULATOR_H
#define UTOTPARSER_TABOUNDSCALCULATOR_H

#include <limits>

#include "utilities/Utils.hpp"


// Language used for declaring bounds:
// -----------------------------------
// For common bounds:
//     bound:x1:y1|bound:x2:y2|bound:x3:y3
// where x1, y1, etc... can either be a number (0 included) or the 'inf' string, specifying an open bound on the right.
// -----------------------------------
// For tiles not having a bound:
//     bound:nan:nan

// WARNING:
// The actual implementation computes the strictest interval on all paths, not taking into account the semantic of the given
// compositional string. That is, if in a tile with two 'out' locations we have different paths using a disjoint set of tiles
// for each path, the actual implementation computes the interval considering all the tiles used (that is, it simply updates
// the interval for each added tile in sequence, look at the ActionPushTile.hpp class).
// As a future enhancement, intervals may be computed taking into account only the tiles relevant in a certain path.

typedef struct {
    bool isNan;
    bool isDisjoint;
    double l;
    double r;
} Bound;

class TABoundsCalculator {

private:
    // A map storing for each string key (which will be the name of a TA), the corresponding bounds that have been found.
    static std::map<std::string, std::vector<Bound>> taBounds;

    // Vector containing the bounds which constrain the value of the parameter.
    static std::vector<Bound> bounds;

    // The keyword corresponding to the nan bound.
    static const std::string nanKeyword;

    // The keyword corresponding to the inf bound.
    static const std::string infKeyword;


    /**
     * Method used to build a bound given a string describing it.
     * @param bound a string of the form "bound:l:r"
     * @return a Bound corresponding to the given string.
     */
    static Bound getBound(const std::string &bound)
    {
        // Vector containing the following elements as strings: {"bound", l, r}, where:
        // "bound" is the keyword used to specify the bounds in Uppaal (not used here).
        // l is the lower value of the bound (either a number, the "inf" keyword or the "nan" keyword).
        // r is the upper value of the bound (either a number, the "inf" keyword or the "nan" keyword).
        std::vector<std::string> tokens = getTokenizedString(bound, ':');

        double l { std::stod(tokens[1]) };
        double r { (tokens[2] == infKeyword) ? std::numeric_limits<double>::max() : std::stod(tokens[2]) };

        bool isDisjoint { l > r };
        bool isNan { tokens[1] == nanKeyword && tokens[2] == nanKeyword };

        return Bound { isNan, isDisjoint, l, r };
    }


    /**
     * Method used to get the intersection of two bounds.
     * @param b1 the first bound.
     * @param b2 the second bound.
     * @return a new bound corresponding to the intersection between 'b1' and 'b2'.
     */
    static Bound getBoundIntersection(const Bound &b1, const Bound &b2)
    {
        double newL { std::max(b1.l, b2.l) };
        double newR { std::min(b1.r, b2.r) };

        return Bound { (b1.isNan || b2.isNan), (newL > newR), newL, newR };
    }


    /**
     * Method used to update the available bounds.
     * @param bound the bound used to update the current ones.
     * @param pushBound used to tell if a new bound must be pushed (e.g. if we have a tile with two out locations, the first bound
     *                  will only update the current ones, while the second must generate new possible bounds and hence it must be pushed).
     */
    static void updateBounds(const std::string &boundString, bool pushBound)
    {
        Bound bound = getBound(boundString);

        // If l and r are equal to the "nan" keyword, then no bounds must be added/compared.
        if (!bound.isNan)
        {
            // Vector collecting bounds to be pushed into the 'bounds' vector at the end of the for loop.
            std::vector<Bound> toBePushed {};

            // Bounds must be updated and, if needed, pushed inside the 'bounds' vector.
            for (Bound &b: bounds)
            {
                // If a bound is already disjoint, or it is nan, the computation on it can be skipped.
                if (!(b.isDisjoint || b.isNan))
                {
                    Bound newBound = getBoundIntersection(b, bound);
                    if (pushBound)
                        toBePushed.push_back(newBound);
                    else
                        b = newBound;
                }
            }

            // Inserting eventual bounds that had to be pushed.
            bounds.insert(bounds.end(), toBePushed.begin(), toBePushed.end());
        }
    }


public:
    /**
     * Method used to delete all elements from the 'bounds' vector and to set 'isDisjoint' to false.
     */
    static void resetBoundsVector()
    {
        bounds.clear();
    }


    /**
     * Method used to add new bounds or update old ones.
     * @param boundString a string containing one or more bounds, specified using the syntax for bounds reported
     *                    at the beginning of this class.
     */
    static void addBounds(const std::string &boundString)
    {
        // Vector containing strings of the form: "bound:l:r", specifying bounds.
        std::vector<std::string> singleBounds { getTokenizedString(boundString, '|') };

        // If no bound is present, simply push the given ones.
        if (bounds.empty())
            for (const std::string &bound: singleBounds)
            {
                Bound b = getBound(bound);

                if (!b.isNan)
                    bounds.push_back(b);
            }
        else
        {
            bool pushBound { false };
            for (const std::string &bound: singleBounds)
            {
                updateBounds(bound, pushBound);

                // If more than one bound is present in the given string, new possible bounds must be inserted.
                pushBound = true;
            }
        }
    }


    /**
     * Method used to insert into the 'taBounds' map the bounds found for a given key.
     * @param nameTA the name of the TA for which to store bounds.
     */
    static void storeTABounds(const std::string &nameTA)
    {
        taBounds.insert(std::make_pair(nameTA, bounds));
    }


    /**
     * Method used to retrieve from the 'taBounds' map the bounds found for a given key.
     * @param nameTA the name of the TA for which to retrieve bounds.
     * @return the bounds relative to the 'nameTA' TA.
     */
    static std::vector<Bound> getStoredBounds(const std::string &nameTA)
    {
        return taBounds.find(nameTA)->second;
    }


    /**
     * Method used to get a string representation of the bound specified in the 'bound' parameter.
     * @param bound the bound to turn into a string.
     * @return a string representation of the 'bound' parameter.
     */
    static std::string getBoundAsString(const Bound &bound)
    {
        std::stringstream res;
        res << "{ " << std::fixed << std::setprecision(1) << bound.l << " - ";
        if (bound.r >= (std::numeric_limits<double>::max() - 1))
            res << "inf";
        else
            res << std::fixed << std::setprecision(1) << bound.r;
        res << " }\n";
        return res.str();
    }


    /**
     * Method used to get a string representation of the 'bounds' vector.
     * @return a string representation of the 'bounds' vector.
     */
    static std::string getBoundsAsString(const std::vector<Bound> &inputBounds)
    {
        std::stringstream res;

        for (const Bound &bound: inputBounds)
        {
            if (bound.isDisjoint)
                res << "Disjoint bound :: ";
            res << "Bound: " << getBoundAsString(bound);
        }
        return res.str();
    }


    /*
     * Method used to return the bounds for a given TA name, its respective Bounds as a string representation,
     * @param nameTA the name of the TA for which to return the bounds as a string.
     * @return a string representation of the bounds retrieved for he 'nameTA' TA.
     */
    static std::string getBoundsAsString(const std::string &nameTA)
    {
        return getBoundsAsString(taBounds.find(nameTA)->second);
    }


    static std::vector<Bound> getBounds()
    {
        return bounds;
    };

};

// Defining static attributes.
std::map<std::string, std::vector<Bound>> TABoundsCalculator::taBounds {};
std::vector<Bound> TABoundsCalculator::bounds {};
const std::string TABoundsCalculator::nanKeyword { "nan" };
const std::string TABoundsCalculator::infKeyword { "inf" };

#endif //UTOTPARSER_TABOUNDSCALCULATOR_H
