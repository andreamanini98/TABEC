#include "nlohmann/json.hpp"

using json = nlohmann::json;


/**
 * Function used to ensure the given json parameter is returned as an Array.
 * This is useful for: locations, transitions, labels.
 * @param inFile the json parameter we want to return as an array.
 * @return the array version of the given json parameter.
 */
static json getJsonAsArray(json inFile)
{
    if (inFile.is_array())
        return inFile;
    else {
        json inFileToArray = json::array();
        inFileToArray.push_back(inFile);
        return inFileToArray;
    }
}