#ifndef UTOTPARSER_UTILS_H
#define UTOTPARSER_UTILS_H

#include <sstream>

#include "Exceptions.h"
#include "XMLtoJSONInclude/xml2json.hpp"

using json = nlohmann::json;


/**
 * Function used to return a json representation starting from a file. Notice that the file must be an .xml file.
 * @param file the file from which to get a json representation.
 * @return a json representation of the given file.
 */
json getJsonFromFileStream(std::ifstream &file, bool isXml = true) {
    if (isXml) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        std::string tmp = buffer.str();
        const char *xml_str = tmp.c_str();

        // We obtain the json representation of a TA from the xml generated by UPPAAL.
        return json::parse(xml2json(xml_str));
    } else throw NotXMLFormatException("Provided file should be of .xml type!");
}

/**
 * Function used to collect inside a vector all the integer occurrences inside a string.
 * @param str the string into which we're looking for integers occurrences.
 * @return a vector containing all integer numbers present in the string parameter.
 */
static std::vector<int> extractIntegersFromString(const std::string &str) {
    std::vector<int> result;
    std::stringstream ss(str);
    std::string temp;

    while (ss >> temp) {
        try {
            int found = std::stoi(temp);
            result.emplace_back(found);
        }
        catch (const std::invalid_argument &) {}
        catch (const std::out_of_range &) {}
    }
    return result;
}

/**
 * Function that returns the maximum integer value that appears inside a string.
 * @param str the string into which we're looking for integers occurrences.
 * @return the maximum integer value appearing in the string parameter.
 */
static int getMaxIntFromStr(const std::string &str) {
    std::vector<int> tmp = extractIntegersFromString(str);
    return (!tmp.empty()) ? *max_element(tmp.begin(), tmp.end()) : 0;
}

/**
 * Function used to delete all contents of a given directory.
 * @param dir the path to the directory to clear.
 */
void deleteDirectoryContents(const std::filesystem::path &dir) {
    for (const auto &entry: std::filesystem::directory_iterator(dir))
        std::filesystem::remove_all(entry.path());
}

/**
 * Function used to check if an element is contained inside a vector and to get its position inside such vector.
 * @tparam T the type of the vector and of the element to find.
 * @param vec the vector in which the element may be contained.
 * @param elem the element to find.
 * @return a pair containing: (1) true if elem is contained inside vec and (2) its position, otherwise (1) false and (2) -1 as sentinel value.
 */
template<typename T>
std::pair<bool, int> isElementInVector(const std::vector<T> &vec, const T &elem) {
    auto iter = std::find(vec.begin(), vec.end(), elem);
    if (iter != vec.end())
        return {true, std::distance(vec.begin(), iter)};
    else
        return {false, -1};
}

/**
 * Function that, given a string containing some symbols, returns the last word after the last occurrence of such symbol.
 * @param str the string in which we look for the last word.
 * @param symbol the token appearing in the string.
 * @return the last word following the last occurrence of symbol.
 */
std::string getWordAfterLastSymbol(const std::string &str, char symbol) {
    size_t lastSlashPos = str.find_last_of(symbol);
    if (lastSlashPos != std::string::npos)
        return str.substr(lastSlashPos + 1);
    else
        return "";
}

/**
 * Function used to get a specific string obtained by splitting the original one.
 * @param str the string to split.
 * @param symbol the token used for splitting the string.
 * @param pos the position of the string to return. The position is considered on the result of the splitting operation.
 * @return the string at position pos in the split original string.
 */
std::string getStringGivenPosAndToken(const std::string &str, char symbol, int pos) {
    std::istringstream stream(str);
    std::string token;
    std::vector<std::string> tokens;

    while (std::getline(stream, token, symbol))
        tokens.push_back(token);

    return tokens[pos];
}

/**
 * Function used to get all the entries in a directory sorted in alphabetical order.
 * @param directory_path the path to the directory in which to collect all entries.
 * @return a vector containing the alphabetically ordered entries.
 */
std::vector<std::filesystem::directory_entry> getEntriesInAlphabeticalOrder(const std::filesystem::path &directory_path) {
    std::vector<std::filesystem::directory_entry> entries;

    // Iterate over the directory and collect the entries.
    for (const auto &entry: std::filesystem::directory_iterator(directory_path))
        entries.push_back(entry);

    // Sort the entries alphabetically.
    std::sort(entries.begin(), entries.end(), [](const auto &a, const auto &b) {
        // We have to provide sort a sorting criterion by using return sorting_criterion.
        return a.path().filename() < b.path().filename();
    });
    return entries;
}

/**
 * Function used to substitute all occurrences of a given character inside a string with another string.
 * @param originalString the string in which we want to substitute a character.
 * @param charToReplace the character to replace.
 * @param replacementString the string with which we want to replace the character.
 * @return a string in which, starting from originalString, we replaced every charToReplace with replacementString.
 */
std::string substituteCharInString(std::string originalString, char charToReplace, const std::string &replacementString) {
    size_t pos = 0;
    while ((pos = originalString.find(charToReplace, pos)) != std::string::npos) {
        originalString.replace(pos, 1, replacementString);
        pos += replacementString.length();
    }
    return originalString;
}

/**
 * Method used to insert a whitespace between all strings contained in a std::vector.
 * @param args a vector of strings in which we have to insert whitespaces between them.
 * @return a string obtained by connecting all the strings in the vector with whitespaces.
 */
std::string spaceStr(const std::vector<std::string> &args) {
    std::string res {};

    for (const std::string &str: args)
        res += str + " ";

    // Remove the trailing whitespace
    if (!res.empty())
        res.pop_back();

    return res;
}

#endif //UTOTPARSER_UTILS_H
