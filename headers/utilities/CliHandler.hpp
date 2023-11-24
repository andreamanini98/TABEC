#ifndef UTOTPARSER_CLIHANDLER_H
#define UTOTPARSER_CLIHANDLER_H

#include "utilities/Utils.hpp"
#include "defines/ANSI-color-codes.h"
#include "Exceptions.h"

// List of available commands. Remember to update the vector availableCommands below as well.
// ------------------------------------------------------------------------------------

// Specify source directory path. This will require a path as subsequent argument.
const std::string src { "-src" };

// Specify destination directory path. This will require a path as subsequent argument.
const std::string dst { "-dst" };

// The analysis will be carried out only on nrtTAs.
const std::string nrt { "-nrt" };

// A Json representation of the TAs will be printed in stdout.
const std::string jsn { "-jsn" };

// Specify tChecker's liveness tool directory path. This will require a path as subsequent argument.
const std::string lns { "-lns" };

// Prints in the logs all the tests executed with the parameters (it doesn't stop at the first acceptance condition).
const std::string all { "-all" };

// Allows to save a .dot representation of the tile obtained by executing the constructor.
const std::string tdt { "-tdt" };

// If set, the grapher will only translate all the files contained in the 'outputDOTs' directory.
const std::string rfd { "-rfd" };

// Allows to save a .tck representation of the tile obtained by executing the constructor.
const std::string ttt {"-ttt"};

// ------------------------------------------------------------------------------------


class CliHandler {

private:
    // A vector containing all the available commands that can be inserted in the cli.
    std::vector<std::string> availableCommands = { src, dst, nrt, jsn, lns, all, tdt, rfd, ttt};

    // A pointer to main's argc.
    int *argc_p;

    // A pointer to main's argv.
    char ***argv_p;

    // A map containing an entry for each command, where the entry keeps track of:
    // - The fact that the command is actually among the ones inserted when launching the program.
    // - The position of the command (if present).
    std::map<std::string, std::pair<bool, int>> commandsAndPositions;


public:
    /**
     * Default parametric constructor.
     * @param argc_p a pointer to the main's argc.
     * @param argv_p a pointer to the main's argv.
     * @param debug true if some debug information has to be printed, false otherwise.
     */
    CliHandler(int *argc_p, char ***argv_p, bool debug = false) : argc_p(argc_p), argv_p(argv_p)
    {
        commandsAndPositions.clear();

        // We transform the triple pointer ***arv_p in a vector of strings.
        std::vector<std::string> cliArguments(*(this->argv_p), *(this->argv_p) + *(this->argc_p));

        // For each available command, we populate the map.
        for (std::string &cmd: availableCommands)
            commandsAndPositions.emplace(cmd, isElementInVector<std::string>(cliArguments, cmd));

        if (debug)
        {
            // Remember that find() on a map returns an iterator, hence to access the actual element we have to access second on it with ->.
            for (std::string &cmd: availableCommands)
            {
                std::cout << "Command: " << cmd << " is present "
                          << (commandsAndPositions.find(cmd)->second).first << " at position "
                          << (commandsAndPositions.find(cmd)->second).second << std::endl;
            }
        }
    }


    /**
     * Method used to tell if a given command has been inserted when launching the program.
     * @param cmd the command to know if it has been inserted or not.
     * @return true if the command has been inserted, false otherwise.
     */
    bool isCmd(const std::string &cmd)
    {
        return (commandsAndPositions.find(cmd)->second).first;
    }


    /**
     * Method used to get the position of the command in the list of arguments.
     * @param cmd the command to get the position.
     * @return the position of the command if present, a sentinel value otherwise.
     */
    int getPos(const std::string &cmd)
    {
        return (commandsAndPositions.find(cmd)->second).second;
    }


    /**
     * Method used to get the argument relative to the specified cmd option.
     * An argument is the next element given after the cmd option in the command line.
     * @param cmd the command option for which to get the relative argument.
     * @return the argument of the cmd option.
     * @throw CommandNotProvidedException if the command was not provided when launching the program.
     */
    std::string getCmdArgument(const std::string &cmd)
    {
        if (isCmd(cmd))
            return (*argv_p)[getPos(cmd) + 1];
        else
            throw CommandNotProvidedException(std::string("Exception: command " + cmd + " not provided.").c_str());
    }

};


#endif //UTOTPARSER_CLIHANDLER_H
