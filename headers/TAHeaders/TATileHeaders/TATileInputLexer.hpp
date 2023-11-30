#ifndef UTOTPARSER_TATILEINPUTLEXER_H
#define UTOTPARSER_TATILEINPUTLEXER_H

#include "utilities/Utils.hpp"


class TATileInputLexer {

private:
    // A vector of pairs defining in the second element the actual symbol appearing in the compositional tile string,
    // while in the first element the token corresponding to that symbol.
    std::vector<std::pair<std::string, std::string>> tokens
            {
                    { "lparen",           "(" },
                    { "rparen",           ")" },
                    { "only_one_out",     "+1" },
                    { "match_inout_size", "+" }
            };


public:
    /**
     * Method used to get a tokenized representation of the given string.
     * @param str the string to tokenize.
     * @return a tokenized version of the string.
     */
    std::string tokenizeString(std::string str)
    {
        // Sort the vector based on the length of the second element. This ensures correctness while tokenizing the string.
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

};


#endif //UTOTPARSER_TATILEINPUTLEXER_H
