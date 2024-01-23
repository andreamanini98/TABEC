#ifndef UTOTPARSER_TATILEINPUTLEXER_H
#define UTOTPARSER_TATILEINPUTLEXER_H

#include "utilities/Utils.hpp"
#include "utilities/StringsGetter.hpp"
#include "TAHeaders/TATileHeaders/tileEnums/TileTypeEnum.h"

// Context-free grammar describing our compositional language:
// -----------------------------------------------------------
// TiledTA -> Tile | TiledTA (Bin TiledTA | Tri TiledTA TiledTA)* | '(' TiledTA ')'
// Bin -> '+' | '+1'
// Tri -> '++'
// Tile -> t | t '[' Integer ']'
// Integer -> epsilon | (1..9)(0..9)*

// How to extend the syntax:
// 1) Add the new tokens in the enum in TileTokens.h.
// 2) Update accordingly the fromStrTileTokenEnum method in TileTokens.h.
// 3) Add the tokens and their respective symbol in the operatorTokens vector in TATileInputLexer.hpp.
// 4) Add the corresponding actions creating a new Action in the parserActionFactory folder or modify existing ones.

class TATileInputLexer {

private:
    StringsGetter &stringsGetter;

    // A vector comprising both operator and tile tokens.
    std::vector<std::pair<std::string, std::string>> tokens {};

    // A vector of pairs defining in the second element the actual symbol appearing in the
    // compositional tile string, while in the first element the token corresponding to that symbol.
    std::vector<std::pair<std::string, std::string>> operatorTokens
            {
                    { "lparen",           "(" },
                    { "rparen",           ")" },
                    { "lsqparen",         "[" },
                    { "rsqparen",         "]" },
                    { "only_one_out",     "+1" }, // t1 +1 t2
                    { "match_inout_size", "+" },  // t1 + t2
                    { "tree_op",          "++" }  // t1 ++ t2 t3
            };

    // A vector containing a description for each available operator.
    std::vector<std::string> operatorDescription
            {
                    { "Operator: +1\nSyntax:   t1 +1 t2\nExample:  Concatenates exactly one 'out' location from t1 to one 'in' location in t2." },
                    { "Operator: +\nSyntax:   t1 + t2\nExample:  Concatenates each 'out' location from t1 to one 'in' location in t2. Their number must match." },
                    { "Operator: ++\nSyntax:   t1 ++ t2 t3\nExample:  Creates a tree concatenating the 'in' location of t2 and t3 to one 'out' location of t1." }
            };

    // The following are vectors of pairs defining in the second element the actual tile identifier appearing in
    // the compositional tile string, while in the first element the tile token corresponding to that symbol.

    // Vector representing randomly-generated tiles.
    std::vector<std::pair<std::string, std::string>> rngTileTokens
            {
                    { "t_barabasi_albert", "t:BA" } // Random-generated tile.
            };

    // Vector representing acceptance tiles.
    // Those tiles must have one 'in' location, no 'out' locations and at least one accepting location.
    std::vector<std::pair<std::string, std::string>> accTileTokens {};

    // Vector representing binary tiles.
    // Those tiles must have one 'in' location and one 'out' location.
    std::vector<std::pair<std::string, std::string>> binTileTokens {};

    // Vector representing ternary tiles.
    // Those tiles must have one 'in' location and two 'out' locations.
    std::vector<std::pair<std::string, std::string>> triTileTokens {};


    /**
     * Method used to gather the name of the available tiles for a given directory path.
     * @param startId the integer identifier used to number the tiles' symbol used in the compositional string.
     * @param directoryPath the directory in which to look for the available tiles.
     * @param tileTokens the vector in which to store the available tiles.
     * @return an integer representing the number of tiles that has been found.
     */
    static int emplaceTile(int startId, const std::string &directoryPath, std::vector<std::pair<std::string, std::string>> &tileTokens)
    {
        int tileId = startId;
        for (const auto &entry: getEntriesInAlphabeticalOrder(directoryPath))
        {
            std::string tileName = getStringGivenPosAndToken(getWordAfterLastSymbol(entry.path(), '/'), '.', 0);
            tileTokens.emplace_back(tileName, "t" + std::to_string(tileId));
            tileId++;
        }
        return tileId;
    }


    /**
     * Method used to get the names of all the tiles available to use.
     */
    void getAvailableTiles()
    {
        // A progressive integer used to differentiate the name of the tiles to be used in the compositional string.
        int tileId { 1 };

        // For each accepting, binary and ternary type of tile, gather the available ones.
        tileId = emplaceTile(tileId, stringsGetter.getAccTilesDirPath(), accTileTokens);
        tileId = emplaceTile(tileId, stringsGetter.getBinTilesDirPath(), binTileTokens);
        emplaceTile(tileId, stringsGetter.getTriTilesDirPath(), triTileTokens);
    }


    /**
     * Method used to display the available tiles, printing the symbol and the name of such tiles.
     * @param message a string to be displayed before printing the tiles.
     * @param vector the vector containing the available tiles to print.
     */
    static void displayVectorOfTiles(const std::string &message, const std::vector<std::pair<std::string, std::string>> &vector)
    {
        std::cout << message;
        for (auto &token: vector)
            std::cout << "\u25CF " << token.second << " - " << token.first << '\n';
    }


    /**
     * Method used to display the available tiles contained in the tileTokens vector.
     */
    void displayAvailableTiles()
    {
        std::cout << "The following tiles are available:\n";
        displayVectorOfTiles("\nAccepting tiles:\n", accTileTokens);
        displayVectorOfTiles("\nBinary tiles:\n", binTileTokens);
        displayVectorOfTiles("\nTernary tiles:\n", triTileTokens);
        displayVectorOfTiles("\nRandomly-generated tiles:\n", rngTileTokens);
    }


    /**
     * Method used to display the available operators' description.
     */
    void displayAvailableOperators()
    {
        std::cout << "The following operators are available:\n";
        for (auto &desc: operatorDescription)
            std::cout << desc << "\n\n";
    }


    /**
     * Method used to display some examples to the user.
     */
    static void displayExampleSyntax()
    {
        std::cout << "An example of a syntactically-valid compositional string is as follows:\n";
        std::cout << "t4 ++ (t4 ++ (t1 + t2 + t3) (t1 + t1 + t3)) (t4 ++ (t1 + t1 + t3) (t2 + t2 + t3))\n";
        std::cout << "\nTry also this to have fun:\n";
        std::cout << "t4 ++ (t4 ++ (t4 ++ (t1 + t2 + t3) (t1 + t1 + t3)) (t4 ++ (t1 + t1 + t3) (t2 + t2 + t3))) (t4 ++ (t4 ++ (t1 + t2 + t3) (t1 + t1 + t3)) (t4 ++ (t1 + t1 + t3) (t2 + t2 + t3)))\n";
    }


    /**
     * Method used to get a tokenized representation of the given string.
     * @param str the string to tokenize.
     * @return a tokenized version of the string.
     */
    std::string tokenizeString(std::string &str)
    {
        // Sort the 'tokens' vector based on the length of the second element. This ensures correctness while tokenizing the string.
        std::sort(tokens.begin(), tokens.end(), sortPairBySecondElementLength);

        // For each token, check all its occurrences inside the string and convert it.
        for (auto &token: tokens)
        {
            // First, find the position of the token in the string.
            size_t pos = str.find(token.second);

            // Then, replace all occurrences of the token in the string.
            while (pos != std::string::npos)
            {
                // Replace token with its corresponding key.
                str.replace(pos, token.second.length(), token.first);

                // Find the next occurrence of the token.
                pos = str.find(token.second, pos + token.first.length());
            }
        }
        return str;
    }


public:
    explicit TATileInputLexer(StringsGetter &stringsGetter) : stringsGetter(stringsGetter)
    {
        getAvailableTiles();
        tokens.insert(tokens.end(), operatorTokens.begin(), operatorTokens.end());
        tokens.insert(tokens.end(), accTileTokens.begin(), accTileTokens.end());
        tokens.insert(tokens.end(), binTileTokens.begin(), binTileTokens.end());
        tokens.insert(tokens.end(), triTileTokens.begin(), triTileTokens.end());
        tokens.insert(tokens.end(), rngTileTokens.begin(), rngTileTokens.end());
    };


    /**
     * Method used to show relevant information when starting the constructor.
     */
    void showHelpMessage()
    {
        displayAvailableTiles();

        std::cout << '\n';
        displayAvailableOperators();

        displayExampleSyntax();
    }


    /**
     * Method used to get, from the user, a string representing the way in which tiles will be merged together.
     */
    std::string getTokenizedCompositionalString(bool showHelp)
    {
        if (showHelp)
            showHelpMessage();

        std::string userInput {};
        char confirmFlag { 'n' };

        while (confirmFlag == 'n')
        {
            std::cout << "\nPlease insert the tile compositional string.\n";
            std::cout << "$> ";

            getline(std::cin, userInput);

            std::cout << "\nYou have given as input: \n";
            std::cout << "$> " + userInput << std::endl;
            std::cout << "\nDo you want to confirm? [y = yes, n = no]\n";
            std::cout << "$> ";
            std::cin >> confirmFlag;
            std::cin.ignore();
        }

        std::string tokenizedString { tokenizeString(userInput) };
        std::cout << "The tokenized string is: " << tokenizedString << std::endl;

        return tokenizedString;
    }


    /**
     * Method used to get a tokenized version of a given string.
     * @param str the string to tokenize.
     * @return a tokenized version of the 'str' parameter.
     */
    std::string getTokenizedCompositionalString(std::string &str)
    {
        std::string tokenizedString { tokenizeString(str) };
        std::cout << "The tokenized string is: " << tokenizedString << std::endl;

        return tokenizedString;
    }


    /**
     * Method used to return all the available tiles, grouped in a vector based on their type.
     * The returned vector has the following content:
     * 1) The type of the tile.
     * 2) A vector of pairs containing:
     *    1) The token of the tile.
     *    2) The symbol representing the tile used in the compositional string.
     * Randomly-generated tiles are not returned here, since they're handled separately in the parser.
     * @return a vector containing all the available tiles grouped by type.
     */
    std::vector<std::pair<TileTypeEnum, std::vector<std::pair<std::string, std::string>>>> getTileTokens()
    {
        std::vector<std::pair<TileTypeEnum, std::vector<std::pair<std::string, std::string>>>> tileTokens {};
        tileTokens.emplace_back(accTile, accTileTokens);
        tileTokens.emplace_back(binTile, binTileTokens);
        tileTokens.emplace_back(triTile, triTileTokens);
        return tileTokens;
    }


    std::vector<std::pair<std::string, std::string>> getTokens()
    {
        return tokens;
    }


    /**
     * Method used to get only the tokens associated with randomly-generated tiles.
     * The return is made by value, since otherwise the returned vector may be deallocated before being utilized.
     * @return the second elements of the 'rngTileTokens' vector.
     */
    std::vector<std::string> getRngTileTokens()
    {
        std::vector<std::string> result {};
        result.reserve(rngTileTokens.size());

        for (const auto &tile: rngTileTokens)
            result.emplace_back(tile.second);

        return result;
    }


    /**
     * Method used to get only the tokens associated with accepting tiles.
     * The return is made by value, since otherwise the returned vector may be deallocated before being utilized.
     * @return the second elements of the 'accTileTokens' vector.
     */
    std::vector<std::string> getAccTileTokens()
    {
        std::vector<std::string> result {};
        result.reserve(accTileTokens.size());

        for (const auto &tile: accTileTokens)
            result.push_back(tile.second);

        return result;
    }


    /**
     * Method used to get only the tokens associated with binary tiles.
     * The return is made by value, since otherwise the returned vector may be deallocated before being utilized.
     * @return the second elements of the 'binTileTokens' vector.
     */
    std::vector<std::string> getBinTileTokens()
    {
        std::vector<std::string> result {};
        result.reserve(binTileTokens.size());

        for (const auto &tile: binTileTokens)
            result.push_back(tile.second);

        return result;
    }


    /**
     * Method used to get only the tokens associated with ternary tiles.
     * The return is made by value, since otherwise the returned vector may be deallocated before being utilized.
     * @return the second elements of the 'triTileTokens' vector.
     */
    std::vector<std::string> getTriTileTokens()
    {
        std::vector<std::string> result {};
        result.reserve(triTileTokens.size());

        for (const auto &tile: triTileTokens)
            result.push_back(tile.second);

        return result;
    }


    /**
     * Method used to get only the symbols associated with binary tiles.
     * The return is made by value, since otherwise the returned vector may be deallocated before being utilized.
     * @return the second elements of the 'binTileTokens' vector.
     */
    std::vector<std::string> getBinTileSymbols()
    {
        std::vector<std::string> result {};
        result.reserve(binTileTokens.size());

        for (const auto &tile: binTileTokens)
            result.push_back(tile.first);

        return result;
    }


    /**
     * Method used to get only the symbols associated with ternary tiles.
     * The return is made by value, since otherwise the returned vector may be deallocated before being utilized.
     * @return the second elements of the 'triTileTokens' vector.
     */
    std::vector<std::string> getTriTileSymbols()
    {
        std::vector<std::string> result {};
        result.reserve(triTileTokens.size());

        for (const auto &tile: triTileTokens)
            result.push_back(tile.first);

        return result;
    }

};


#endif //UTOTPARSER_TATILEINPUTLEXER_H
