#ifndef UTOTPARSER_TATILEREGEXGENERATORSTRICT_H
#define UTOTPARSER_TATILEREGEXGENERATORSTRICT_H

#include "utilities/Utils.hpp"
#include "TAHeaders/TATileHeaders/TATileRegExGenerator.hpp"

#define PRINT_DERIVATIONS


// Here we assume tiles have only one 'in' location regardless of the number of 'out' locations.

// Context-free grammar describing our compositional language (strict version):
// ----------------------------------------------------------------------------
// TiledTA -> BinTile | TriTile | BinTile Bin TiledTA | TriTile Tri '(' TiledTA ')' '(' TiledTA ')' | '(' TiledTA ')'
// Bin -> '+'
// Tri -> '++'
// BinTile -> t (where t is a tile derived from an .xml file or a randomly-generated tile which can only apply to a binary operator).
// TriTile -> t (where t is a tile derived from an .xml file or a randomly-generated tile which can only apply to a ternary operator).

// tile-accepting is a tile that should not appear anywhere but at the end of the entire string.

class TATileRegExGeneratorStrict : public TATileRegExGenerator {

private:
    // The non-terminal representing a tile which can be connected with a binary operator.
    const std::string binTile { "BinTile" };

    // The non-terminal representing a tile which can be connected with a ternary operator.
    const std::string triTile { "TriTile" };

    // The non-terminal representing a tile composed only by one 'in' accepting state.
    const std::string acceptingTile { "t3" };


public:
    explicit TATileRegExGeneratorStrict(int maxIter) : TATileRegExGenerator(maxIter)
    {
        expansionRules = {
                { startSymbol, { binTile + " " + binOp + " " + startSymbol,
                                       triTile + " " + triOp + " ( " + startSymbol + " ) " + " ( " + startSymbol + " ) ",
                                       "( " + startSymbol + " )" }},

                { binOp,       { "+" }},

                { triOp,       { "++" }},

                { binTile,     { "t1", // tile_0_5.
                                       "t2", // tile_3_inf.
                                       "t:BA" }},

                { triTile,     { "t4" }} // tile_double_out
        };
        nonStartingNonTerminals = { binOp, triOp, binTile, triTile };
    }


    /**
     * Method used to create a random string corresponding to the context-free grammar specified inside the 'expansionRules' map.
     * The idea is to first lay out the structure of the string by expanding the 'startSymbol' non-terminals.
     * Then, after 'maxIter' iterations, all the 'startSymbol' non-terminals are transformed into 'binTile' non-terminals.
     * This step could have used also other types of tiles (e.g. 'triTile' tiles): the choice made on binary ones
     * is only a convention.
     * Finally, for each non-terminal inside the 'nonStartingNonTerminals' vector, pick a random terminal
     * for it from its respective expansion rules.
     * At the end, a tile composed by only one 'in' accepting state is connected to the end of the generated tiled TA,
     * in order to ensure that there will be at least one accepting state. This is done since tiles may not necessarily
     * include an accepting state.
     * @return a string corresponding to the context-free grammar specified inside the 'expansionRules' map.
     */
    std::string generateRegEx() override
    {
        std::random_device rd;
        std::mt19937 gen(rd());

        std::cout << "Generating regular expression.\n";

        std::string regEx { startSymbol };

#ifdef PRINT_DERIVATIONS
        std::cout << regEx << '\n';
#endif

        // Laying out the structure of the regular expression, stopping after 'maxIter' iterations.
        for (int i = 0; i < maxIter; i++)
        {
            regEx = subSinS(regEx, startSymbol, pickRandomExpansion(startSymbol, gen));
#ifdef PRINT_DERIVATIONS
            std::cout << regEx << '\n';
#endif
        }

        // Now each remaining 'startingSymbol' occurrence must be transformed into a 'binTile' non-terminal,
        // in order to subsequently transform it into a proper tile name.
        // This step could be simplified by directly transforming 'startingSymbol' into a 'binTile' non-terminal,
        // but it has been done this way to be coherent with the final step involving all the non-starting non-terminals.
        while (regEx.find(startSymbol) != std::string::npos)
            regEx = subSinS(regEx, startSymbol, binTile);
#ifdef PRINT_DERIVATIONS
        std::cout << regEx << '\n';
#endif

        // Finally, each non-terminal must be substituted with a proper terminal symbol, since at
        // this level no recursive productions can happen, given that no more 'startingSymbol' occurrence
        // is present, that is, no recursive productions are available.
        // The 'nonStartingNonTerminals' vector must be sorted to avoid name clashes (e.g. between 'Bin' and 'BinTile')
        std::sort(nonStartingNonTerminals.begin(), nonStartingNonTerminals.end(), sortVectorByElementLength);

        for (const std::string &nonTerminal: nonStartingNonTerminals)
            while (regEx.find(nonTerminal) != std::string::npos)
            {
                regEx = subSinS(regEx, nonTerminal, pickRandomExpansion(nonTerminal, gen));
#ifdef PRINT_DERIVATIONS
                std::cout << regEx << '\n';
#endif
            }

        // Adding the accepting tile at the end since tiles may not have one accepting location inside them.
        // Using the '+1' operator since it ensures that the accepting_tile will be connected to at least
        // one location of the randomly-generated tiled TA.
        regEx = "( " + regEx + " )" + " +1 " + acceptingTile;

        return regEx;
    }

};


#endif //UTOTPARSER_TATILEREGEXGENERATORSTRICT_H
