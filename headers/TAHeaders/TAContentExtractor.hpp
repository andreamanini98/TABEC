#ifndef UTOTPARSER_TACONTENTEXTRACTOR_HPP
#define UTOTPARSER_TACONTENTEXTRACTOR_HPP

#include <sstream>
#include "nlohmann/json.hpp"

#include "defines/UPPAALxmlAttributes.h"

using json = nlohmann::json;


class TAContentExtractor {

public:
    /**
     * Method used to retrieve the identifiers of the clocks used in the UPPAAL TA.
     * @param declaration the string containing the clocks declaration.
     * @param clockKeywordPos the position in the declaration parameter of the keyword "clock".
     * @return a vector containing all the TA's clocks.
     */
    static std::vector<std::string> getClocks(std::string declaration)
    {
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
    static std::string getClocksDeclaration(const json &inFile)
    {
        return static_cast<std::string>(inFile.at(NTA).at(TEMPLATE).at(DECLARATION));
    }


    /**
     * Method used to return the initial location name in the given TA.
     * @param inFile the json representation of the TA.
     * @return a string containing the initial location name.
     */
    static std::string getInitialLocationName(const json &inFile)
    {
        return static_cast<std::string>(inFile.at(NTA).at(TEMPLATE).at(INIT).at(REF));
    }


    /**
     * Method used to return a pointer to the initial location name in the given TA.
     * @param inFile the json representation of the TA.
     * @return a pointer to the string containing the initial location name.
     */
    static json *getInitialLocationNamePtr(json &inFile)
    {
        return &(inFile.at(NTA).at(TEMPLATE).at(INIT).at(REF));
    }


    /**
     * Method used to return the locations declarations in the given TA.
     * @param inFile the json representation of the TA.
     * @return a json containing the locations declaration.
     */
    static json getLocations(const json &inFile)
    {
        return inFile.at(NTA).at(TEMPLATE).at(LOCATION);
    }


    /**
     * Method used to return a pointer to the locations declarations in the given TA.
     * @param inFile the json representation of the TA.
     * @return a pointer pointing to a json containing the locations declaration.
     */
    static json *getLocationsPtr(json &inFile)
    {
        return &(inFile.at(NTA).at(TEMPLATE).at(LOCATION));
    }


    /**
     * Method used to return the transitions declarations in the given TA.
     * @param inFile the json representation of the TA.
     * @return a json containing the transitions declaration.
     */
    static json getTransitions(const json &inFile)
    {
        return inFile.at(NTA).at(TEMPLATE).at(TRANSITION);
    }


    /**
     * Method used to return a pointer to the transitions declarations in the given TA.
     * @param inFile the json representation of the TA.
     * @return a pointer pointing to a json containing the transitions declaration.
     */
    static json *getTransitionsPtr(json &inFile)
    {
        return &(inFile.at(NTA).at(TEMPLATE).at(TRANSITION));
    }


    // TODO: maybe raise an exception when no named location has been found.
    /**
     * Method used to extract the locations ids. The considered locations must have been defined in UPPAAL with a name.
     * @param inFile the json file from which to take the named locations.
     * @param name the string to match in the location's name in order to get it as a result.
     * @return a vector containing all the locations whose name equals parameter 'name'.
     */
    static std::vector<std::string> getNamedLocations(const json &inFile, const std::string &name)
    {
        std::vector<std::string> result;

        // First, get all the locations from the given json file.
        json locations = TAContentExtractor::getLocations(inFile);

        // For each such location, we get only those whose name equals parameter name.
        for (auto &loc: locations)
            if (loc.contains(NAME) && static_cast<std::string>(loc.at(NAME).at(TEXT)) == name)
                result.push_back(loc.at(ID));

        return result;
    }


    /**
     * Method used to extract the text section of a location's label.
     * @param inFile the json file in which the text is going to be extracted.
     * @param locationName the name of the location to extract the label text.
     * @param kind the kind of label to extract.
     * @param replaceColon true if one wants to replace all occurrences of '.' characters in the extracted text with ';' characters.
     * @return the text corresponding to the location label. If the label's kind doesn't match the one given as 'kind' parameter,
     *         an empty string will be returned instead.
     */
    static std::string getLocationLabelText(const json &inFile, const std::string &locationName, const std::string &kind, bool replaceColon = true)
    {
        std::string resultLabel {};
        json locations = TAContentExtractor::getLocations(inFile);

        for (auto &loc: locations)
        {
            if (static_cast<std::string>(loc.at(ID)) == locationName)
            {
                if (loc.contains(LABEL) && static_cast<std::string>(loc.at(LABEL).at(KIND)) == kind)
                {
                    resultLabel = static_cast<std::string>(loc.at(LABEL).at(TEXT));
                    if (replaceColon)
                        std::replace(resultLabel.begin(), resultLabel.end(), ',', ';');
                }
            }
        }
        return resultLabel;
    }


    /**
     * Method used to extract the declaration (representing the bounds) in the given TA.
     * @param inFile the json file in which the text is going to be extracted.
     * @return the declaration (representing the bounds) in the given TA.
     */
    static std::string getDeclaration(const json &inFile)
    {
        return static_cast<std::string>(inFile.at(NTA).at(DECLARATION));
    }

};


#endif //UTOTPARSER_TACONTENTEXTRACTOR_HPP
