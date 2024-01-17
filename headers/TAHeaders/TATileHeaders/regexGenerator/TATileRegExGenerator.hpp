#ifndef UTOTPARSER_TATILEREGEXGENERATOR_H
#define UTOTPARSER_TATILEREGEXGENERATOR_H

#include <random>
#include "utilities/Utils.hpp"

#define PRINT_DERIVATIONS


// Prefer using the TATileRegExGeneratorStrict class instead!

// Context-free grammar describing our compositional language:
// -----------------------------------------------------------
// TiledTA -> Tile | TiledTA (Bin TiledTA | Tri TiledTA TiledTA)* | '(' TiledTA ')'
// Bin -> '+' | '+1'
// Tri -> '++'
// Tile -> t | t '[' Integer ']'
// Integer -> epsilon | (1..9)(0..9)*

class TATileRegExGenerator {

private:
    // The non-terminal representing a tile (which will then be identified by its name).
    const std::string tile { "Tile" };


protected:
    // The starting non-terminal symbol representing a general TiledTA
    const std::string startSymbol { "TiledTA" };

    // The non-terminal representing a binary operator.
    const std::string binOp { "Bin" };

    // The non-terminal representing a ternary operator
    const std::string triOp { "Tri" };

    // The maximum iterations performed when building the structure of the regular expression.
    int maxIter {};

    // This map contains as keys the non-terminals, while as values the possible expansions
    // rules relative to the specific non-terminal, contained inside a vector of strings.
    std::map<std::string, std::vector<std::string>> expansionRules {};

    // Vector containing the non-terminals different from the starting one.
    std::vector<std::string> nonStartingNonTerminals {};


    /**
     * Method used to return a random expansion rule for a given non-terminal.
     * The available expansion rules are specified inside the 'expansionRules' map.
     * @param nonTerminal the non-terminal for which to return an expansion rule.
     * @param gen a random number generator.
     * @return a randomly-chosen expansion rule for the given non-terminal.
     */
    std::string pickRandomExpansion(const std::string &nonTerminal, std::mt19937 &gen)
    {
        // Vector containing the possible expansion rules for the given 'nonTerminal' string.
        std::vector<std::string> expansions = expansionRules.find(nonTerminal)->second;

        // Distribution used to extract a random value from the 'expansions' vector.
        std::uniform_int_distribution<int> int_dist(0, static_cast<int>(expansions.size()) - 1);

        // Returning a random string from the 'expansions' vector.
        return expansions[int_dist(gen)];
    }


public:
    explicit TATileRegExGenerator(int maxIter) : maxIter(maxIter)
    {
        expansionRules = {
                { startSymbol, { tile,
                                       startSymbol + " " + binOp + " " + startSymbol,
                                       startSymbol + " " + triOp + " " + startSymbol + " " + startSymbol,
                                       "( " + startSymbol + " )" }},

                { binOp,       { "+",
                                       "+1" }},

                { triOp,       { "++" }},

                { tile,        { "t1", // tile_0_5.
                                       "t2", // tile_3_inf.
                                       "t3", // tile_accepting.
                                       "t4", // tile_double_out.
                                       "t:BA" }}
        };
        nonStartingNonTerminals = { binOp, triOp, tile };
    }


    /**
     * Method used to print the available expansion rules as specified inside the 'expansionRules' map.
     */
    [[maybe_unused]] void printExpansionRules()
    {
        std::cout << "Available expansion rules:\n";
        for (auto &expRule: expansionRules)
        {
            std::cout << expRule.first << " -> ";
            for (auto &rule: expRule.second)
                std::cout << " " << rule << " | ";
            std::cout << std::endl;
        }
    }


    /**
     * Method used to create a random string corresponding to the context-free grammar specified inside the 'expansionRules' map.
     * The idea is to first lay out the structure of the string by expanding the 'startSymbol' non-terminals.
     * Then, after 'maxIter' iterations, all the 'startSymbol' non-terminals are transformed into 'tile' non-terminals.
     * Finally, for each non-terminal inside the 'nonStartingNonTerminals' vector, pick a random terminal
     * for it from its respective expansion rules.
     * @return a string corresponding to the context-free grammar specified inside the 'expansionRules' map.
     */
    virtual std::string generateRegEx()
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

        // Now each remaining 'startingSymbol' occurrence must be transformed into a 'tile' non-terminal,
        // in order to subsequently transform it into a proper tile name.
        // This step could be simplified by directly transforming 'startingSymbol' into a 'tile' non-terminal,
        // but it has been done this way to be coherent with the final step involving all the non-starting non-terminals.
        while (regEx.find(startSymbol) != std::string::npos)
            regEx = subSinS(regEx, startSymbol, tile);
#ifdef PRINT_DERIVATIONS
        std::cout << regEx << '\n';
#endif

        // Finally, each non-terminal must be substituted with a proper terminal symbol, since at
        // this level no recursive productions can happen, given that no more 'startingSymbol' occurrence
        // is present, that is, no recursive productions are available.
        for (const std::string &nonTerminal: nonStartingNonTerminals)
            while (regEx.find(nonTerminal) != std::string::npos)
            {
                regEx = subSinS(regEx, nonTerminal, pickRandomExpansion(nonTerminal, gen));
#ifdef PRINT_DERIVATIONS
                std::cout << regEx << '\n';
#endif
            }

        return regEx;
    }

};


#endif //UTOTPARSER_TATILEREGEXGENERATOR_H
