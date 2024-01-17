#ifndef UTOTPARSER_TATILEREGEXGENERATORSTRICT_H
#define UTOTPARSER_TATILEREGEXGENERATORSTRICT_H

#include "utilities/Utils.hpp"
#include "TAHeaders/TATileHeaders/regexGenerator/TATileRegExGenerator.hpp"

#define PRINT_DERIVATIONS


// Here we assume tiles have only one 'in' location regardless of the number of 'out' locations.

// Context-free grammar describing our compositional language (strict version):
// ----------------------------------------------------------------------------
// TiledTA -> BinTile | TriTile | RngTile | (BinTile | RngTile) Bin TiledTA | TriTile Tri '(' TiledTA ')' '(' TiledTA ')' | '(' TiledTA ')'
// Bin -> '+'
// Tri -> '++'
// BinTile -> t (where t is a tile derived from an .xml file which can only apply to a binary operator).
// TriTile -> t (where t is a tile derived from an .xml file which can only apply to a ternary operator).
// RngTile -> t | t '[' Integer ']' (where t is a randomly-generated tile which can only apply to a binary operator).
// Integer -> epsilon | (1..9)(0..9)*

// tile-accepting is a tile that should not appear anywhere but at the end of the entire string.

class TATileRegExGeneratorStrict : public TATileRegExGenerator {

private:
    // The non-terminal representing a tile which can be connected with a binary operator.
    const std::string binTile { "BinTile" };

    // The non-terminal representing a tile which can be connected with a ternary operator.
    const std::string triTile { "TriTile" };

    // The non-terminal representing a random tile which can be connected with a binary operator.
    const std::string rngTile { "RngTile" };

    // The non-terminal representing a random integer number.
    const std::string integer { "Integer" };

    // The non-terminal representing a tile composed only by one 'in' accepting state.
    const std::string acceptingTile { "t3" };

    // Integer indicating the maximum number of states randomly-generated tiles will have.
    int maxNumOfRandomStates {};


    /**
     * Method used to perform productions corresponding to the 'startSymbol' non-terminal.
     * @param regEx the regular expression that has to be manipulated.
     * @param gen a random number generator.
     */
    void performStartSymbolProductions(std::string &regEx, std::mt19937 &gen)
    {
        for (int i = 0; i < maxIter; i++)
        {
            regEx = subSinS(regEx, startSymbol, pickRandomExpansion(startSymbol, gen));

#ifdef PRINT_DERIVATIONS
            std::cout << regEx << '\n';
#endif
        }
    }


    /**
     * Method used to substitute every occurrence of the 'startSymbol' non-terminal with either the 'binTile' or 'rngTile' non-terminal.
     * @param regEx the regular expression that has to be manipulated.
     * @param gen a random number generator.
     */
    void substituteStartSymbolWithBinOrRng(std::string &regEx, std::mt19937 &gen)
    {
        // Uniform distribution used to select which non-terminal to use: either 'binTile' or 'rngTile'.
        std::uniform_int_distribution<int> bool_dist(0, 1);

        while (regEx.find(startSymbol) != std::string::npos)
        {
            bool isBinOrRng = bool_dist(gen);

            regEx = subSinS(regEx, startSymbol, isBinOrRng ? binTile : rngTile);

#ifdef PRINT_DERIVATIONS
            std::cout << regEx << '\n';
#endif
        }
    }


    /**
     * Method used to perform productions for every non-terminal contained in the 'nonStartingNonTerminals' vector.
     * @param regEx the regular expression that has to be manipulated.
     * @param gen a random number generator.
     */
    void substituteNonTerminals(std::string &regEx, std::mt19937 &gen)
    {
        // Sorting the 'nonStartingNonTerminals' strings in descending order of length in order to
        // avoid collision due to shorter named contained in longer names during substitution.
        std::sort(nonStartingNonTerminals.begin(), nonStartingNonTerminals.end(), sortVectorByElementLength);

        for (const std::string &nonTerminal: nonStartingNonTerminals)
            while (regEx.find(nonTerminal) != std::string::npos)
            {
                regEx = subSinS(regEx, nonTerminal, pickRandomExpansion(nonTerminal, gen));

#ifdef PRINT_DERIVATIONS
                std::cout << regEx << '\n';
#endif
            }
    }


    /**
     * Method used to generate a random integer number for every 'integer' non-terminal instance contained in the regex.
     * @param regEx the regular expression that has to be manipulated.
     * @param gen a random number generator.
     */
    void substituteInteger(std::string &regEx, std::mt19937 &gen)
    {
        // Uniform distribution used to draw a random number that will determine the value of the current integer.
        std::uniform_int_distribution<int> int_dist(0, maxNumOfRandomStates);

        while (regEx.find(integer) != std::string::npos)
        {
            regEx = subSinS(regEx, integer, std::to_string(int_dist(gen)));

#ifdef PRINT_DERIVATIONS
            std::cout << regEx << '\n';
#endif
        }
    }


public:
    explicit TATileRegExGeneratorStrict(int maxIter, int maxNumOfRandomStates) :
            TATileRegExGenerator(maxIter), maxNumOfRandomStates(maxNumOfRandomStates)
    {
        expansionRules = {
                { startSymbol, { binTile + " " + binOp + " " + startSymbol,
                                       rngTile + " " + binOp + " " + startSymbol,
                                       triTile + " " + triOp + " ( " + startSymbol + " ) " + " ( " + startSymbol + " ) ",
                                       "( " + startSymbol + " )" }},

                { binOp,       { "+" }},

                { triOp,       { "++" }},

                { binTile,     { "t1",  // tile_0_5.
                                       "t2" }}, // tile_3_inf.

                { triTile,     { "t4" }}, // tile_double_out

                { rngTile,     { "t:BA",
                                       "t:BA[" + integer + "]" }}
        };
        nonStartingNonTerminals = { binOp, triOp, binTile, triTile, rngTile };
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
        performStartSymbolProductions(regEx, gen);

        // Now each remaining 'startingSymbol' occurrence must be transformed into a 'binTile' or 'rngTile' non-terminal,
        // in order to subsequently transform it into a proper tile name.
        // This step could be simplified by directly transforming 'startingSymbol' into a 'binTile' or 'rngTile' non-terminal,
        // but it has been done this way to be coherent with the final step involving all the non-starting non-terminals.
        substituteStartSymbolWithBinOrRng(regEx, gen);

        // Finally, each non-terminal must be substituted with a proper terminal symbol, since at
        // this level no recursive productions can happen, given that no more 'startingSymbol' occurrence
        // is present, that is, no recursive productions are available.
        // The 'nonStartingNonTerminals' vector must be sorted to avoid name clashes (e.g. between 'Bin' and 'BinTile')
        substituteNonTerminals(regEx, gen);

        // Now each occurrence of the 'Integer' non-terminal must be substituted by a randomly-generated integer.
        substituteInteger(regEx, gen);

        // Adding the accepting tile at the end since tiles may not have one accepting location inside them.
        // Using the '+1' operator since it ensures that the accepting_tile will be connected to at least
        // one location of the randomly-generated tiled TA.
        regEx = "( " + regEx + " )" + " +1 " + acceptingTile;

        return regEx;
    }

};


#endif //UTOTPARSER_TATILEREGEXGENERATORSTRICT_H
