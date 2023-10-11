#ifndef UTOTPARSER_UTILS_H
#define UTOTPARSER_UTILS_H


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
 * Function used to check if an element is contained inside a vector.
 * @tparam T the type of the vector and of the element to find.
 * @param vec the vector in which the element may be contained.
 * @param elem the element to find.
 * @return true if elem is contained inside vec, false otherwise.
 */
template<typename T>
bool isElementInVector(const std::vector<T> &vec, const T &elem) {
    auto iter = std::find(vec.begin(), vec.end(), elem);
    return (iter != vec.end());
}

#endif //UTOTPARSER_UTILS_H
