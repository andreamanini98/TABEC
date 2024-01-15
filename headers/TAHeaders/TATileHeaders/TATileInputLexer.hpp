#ifndef UTOTPARSER_TATILEINPUTLEXER_H
#define UTOTPARSER_TATILEINPUTLEXER_H

#include "utilities/Utils.hpp"
#include "utilities/StringsGetter.hpp"

// Context-free grammar describing our compositional language:
// -----------------------------------------------------------
// TiledTA -> Tile | TiledTA (Bin TiledTA | Tri TiledTA TiledTA)* | '(' TiledTA ')'
// Bin -> '+' | '+1'
// Tri -> '++'
// Tile -> t (where t is a tile derived from an .xml file or a randomly-generated tile)

// Example of valid compositional strings:
// t4 ++ (t1 + t3) (t2 + t3)
// t4 ++ (t4 ++ t3 t3) (t4 ++ t3 t3)
// t4 ++ (t4 ++ (t1 + t2 + t3) (t1 + t1 + t3)) (t4 ++ (t1 + t1 + t3) (t2 + t2 + t3))

class TATileInputLexer {

private:
    StringsGetter &stringsGetter;

    // A vector of pairs defining in the second element the actual symbol appearing in the
    // compositional tile string, while in the first element the token corresponding to that symbol.
    std::vector<std::pair<std::string, std::string>> operatorTokens
            {
                    { "lparen",           "(" },
                    { "rparen",           ")" },
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

    // A vector of pairs defining in the second element the actual tile identifier appearing in the
    // compositional tile string, while in the first element the tile token corresponding to that symbol.
    std::vector<std::pair<std::string, std::string>> tileTokens
            {
                    { "t_barabasi_albert", "t:BA" } // Random-generated tile.
            };

    // A vector comprising both operator and tile tokens.
    std::vector<std::pair<std::string, std::string>> tokens {};


    /**
     * Method used to get the names of all the files contained in a specified directory.
     * @param inputDirPath a path to the directory from which to take the file names
     * @return a vector containing the name of all the files in the specified directory.
     */
    void getAvailableTiles()
    {
        int tileId = 1;
        for (const auto &entry: getEntriesInAlphabeticalOrder(stringsGetter.getInputTilesDirPath()))
        {
            std::string tileName = getStringGivenPosAndToken(getWordAfterLastSymbol(entry.path(), '/'), '.', 0);
            tileTokens.emplace_back(tileName, "t" + std::to_string(tileId));
            tileId++;
        }
    }


    /**
     * Method used to display the available tiles contained in the tileTokens vector.
     */
    void displayAvailableTiles()
    {
        std::cout << "The following tiles are available:\n";
        for (auto &token: tileTokens)
            std::cout << "\u25CF " << token.second << " - " << token.first << '\n';
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
        tokens.insert(tokens.end(), tileTokens.begin(), tileTokens.end());
    };


    /**
     * Method used to get, from the user, a string representing the way in which tiles will be merged together.
     */
    std::string getTokenizedCompositionalString()
    {
        displayAvailableTiles();

        // TODO: make this help information pop-up only with an appropriate cli command.
        std::cout << '\n';
        displayAvailableOperators();

        displayExampleSyntax();

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


    std::vector<std::pair<std::string, std::string>> getTileTokens()
    {
        return tileTokens;
    }


    std::vector<std::pair<std::string, std::string>> getTokens()
    {
        return tokens;
    }

};


#endif //UTOTPARSER_TATILEINPUTLEXER_H
