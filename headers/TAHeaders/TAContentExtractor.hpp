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

    /**
     * Method used to return the clocks declarations in the given TA.
     * @param inFile the json representation of the TA.
     * @return a string containing the clocks declaration.
     */
    static std::string getClocksDeclaration(json inFile) {
        return static_cast<std::string>(inFile.at(NTA).at(TEMPLATE).at(DECLARATION));
    }

    /**
     * Method used to return the initial location name in the given TA.
     * @param inFile the json representation of the TA.
     * @return a string containing the initial location name.
     */
    static std::string getInitialLocationName(json inFile) {
        return static_cast<std::string>(inFile.at(NTA).at(TEMPLATE).at(INIT).at(REF));
    }

    /**
     * Method used to return the locations declarations in the given TA.
     * @param inFile the json representation of the TA.
     * @return a json containing the locations declaration.
     */
    static json getLocations(json inFile) {
        return inFile.at(NTA).at(TEMPLATE).at(LOCATION);
    }

    /**
     * Method used to return the transitions declarations in the given TA.
     * @param inFile the json representation of the TA.
     * @return a json containing the transitions declaration.
     */
    static json getTransitions(json inFile) {
        return inFile.at(NTA).at(TEMPLATE).at(TRANSITION);
    }

};

#endif //UTOTPARSER_TACONTENTEXTRACTOR_HPP
