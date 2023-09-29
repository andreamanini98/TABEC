#ifndef UTOTPARSER_TACONTENTEXTRACTOR_HPP
#define UTOTPARSER_TACONTENTEXTRACTOR_HPP


class TAContentExtractor {
public:
    /**
     * Method used to retrieve the identifiers of the clocks used in the UPPAAL TA.
     * @param declaration the string containing the clocks declaration.
     * @param clockKeywordPos the position in the declaration parameter of the keyword "clock".
     * @return a vector containing all the TA's clocks.
     */
    static std::vector<std::string> getClocks(std::string declaration) {
        size_t clockPos = declaration.find(CLOCK);

        // We keep only the portion of string after the "clock" word.
        declaration = declaration.substr(clockPos + 6);
        // We remove the trailing semicolon ';'.
        declaration.pop_back();
        // Remove all whitespaces from the string.
        declaration.erase(std::remove_if(declaration.begin(), declaration.end(), ::isspace), declaration.end());

        std::stringstream ss(declaration);
        std::vector<std::string> clocks;
        std::string s;

        // We now split the string for each encountered ',' storing the resulting token in a vector.
        while (getline(ss, s, ','))
            clocks.push_back(s);

        return clocks;
    }

    static std::string getClocksDeclaration(json inFile) {
        return static_cast<std::string>(inFile.at(NTA).at(TEMPLATE).at(INIT).at(REF));
    }

    static std::string getInitialLocationName(json inFile) {
        return static_cast<std::string>(inFile.at(NTA).at(TEMPLATE).at(INIT).at(REF));
    }

    static json getLocations(json inFile) {
        return inFile.at(NTA).at(TEMPLATE).at(LOCATION);
    }

    static json getTransitions(json inFile) {
        return inFile.at(NTA).at(TEMPLATE).at(TRANSITION);
    }


};

#endif //UTOTPARSER_TACONTENTEXTRACTOR_HPP
