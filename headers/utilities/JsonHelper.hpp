#ifndef UTOTPARSER_JSONHELPER_H
#define UTOTPARSER_JSONHELPER_H

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
    else
    {
        json inFileToArray = json::array();
        inFileToArray.push_back(inFile);
        return inFileToArray;
    }
}


/**
 * Helper method used to get a json pointer pointing to a json array.
 * @param jsonPtr a json for which to get a pointer pointing to a json array.
 * @return a pointer pointing to a json array.
 */
static json *getJsonPtrAsArray(json *jsonPtr)
{
    // If the tile pointer's content is not an array, we have to make it an array.
    if (!jsonPtr->is_array())
        *jsonPtr = getJsonAsArray(*jsonPtr);

    return jsonPtr;
}


#endif // UTOTPARSER_JSONHELPER_H
