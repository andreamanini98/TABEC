#ifndef UTOTPARSER_TATILEINPUTPARSER_H
#define UTOTPARSER_TATILEINPUTPARSER_H

#include "utilities/StringsGetter.hpp"


class TATileInputParser {

private:
    // A string containing the user input describing how to merge the tiles.
    std::string compositionalTileString {};

    const StringsGetter &stringsGetter;


public:
    explicit TATileInputParser(const StringsGetter &stringsGetter) : stringsGetter(stringsGetter)
    {
        readCompositionalString();
    };


    /**
     * Method used to get from the user a string representing the way in which tiles will be merged together.
     */
    void readCompositionalString()
    {
        std::cout << "The following tiles are available:\n";

        std::vector<std::string> availableTiles = getAllFileNamesInDirectory(stringsGetter.getInputTilesDirPath());

        // Displaying the name of all the available tiles the user can choose from.
        for (std::string &availableTile: availableTiles)
            std::cout << "\u25CF " << availableTile << std::endl;

        char confirmFlag { 'n' };
        while (confirmFlag == 'n')
        {
            std::cout << "\nPlease insert the tile compositional string.\n";
            std::cout << "$> ";

            getline(std::cin, compositionalTileString);

            std::cout << "\nYou have given as input: \n";
            std::cout << "$> " + compositionalTileString << std::endl;
            std::cout << "Do you want to confirm? [y = yes, n = no]\n";
            std::cout << "$> ";
            std::cin >> confirmFlag;
            std::cin.ignore();
        }
    }


    /**
     * Method used to get a vector containing the names of all the tiles given in the compositionalTileString.
     * @return a vector containing the names of all the tiles needed for constructing the final Tiled TA.
     */
    // TODO: you'll have to differentiate them from commands in some way.
    std::vector<std::string> getNeededTilesNames()
    {
        std::vector<std::string> neededTiles;
        std::string tile;
        std::istringstream ss(compositionalTileString);

        while (getline(ss, tile, ' '))
            neededTiles.push_back(tile);

        return neededTiles;
    }

};


#endif //UTOTPARSER_TATILEINPUTPARSER_H
