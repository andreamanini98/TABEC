#ifndef UTOTPARSER_TATILEINPUTLEXER_H
#define UTOTPARSER_TATILEINPUTLEXER_H

#include "utilities/Utils.hpp"
#include "utilities/StringsGetter.hpp"

// Example of a valid compositional string:
// t1 + (t2 + t3) +1 t4


class TATileInputLexer {

private:
    StringsGetter &stringsGetter;

    // A vector of pairs defining in the second element the actual symbol appearing in the
    // compositional tile string, while in the first element the token corresponding to that symbol.
    std::vector<std::pair<std::string, std::string>> operatorTokens
            {
                    { "lparen",           "(" },
                    { "rparen",           ")" },
                    { "only_one_out",     "+1" },
                    { "match_inout_size", "+" }
            };

    // A vector of pairs defining in the second element the actual tile identifier appearing in the
    // compositional tile string, while in the first element the tile token corresponding to that symbol.
    std::vector<std::pair<std::string, std::string>> tileTokens {};

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
        for (auto &token: tileTokens)
            std::cout << "\u25CF " << token.second << " - " << token.first << '\n';
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
        std::cout << "The following tiles are available:\n";

        displayAvailableTiles();

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

        std::cout << "The tokenized string is: " << tokenizeString(userInput) << std::endl;

        return tokenizeString(userInput);
    }


    std::vector<std::pair<std::string, std::string>> getOperatorTokens()
    {
        return operatorTokens;
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
