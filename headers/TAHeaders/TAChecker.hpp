#ifndef UTOTPARSER_TACHECKER_H
#define UTOTPARSER_TACHECKER_H

#include "CommandReader.h"

#define liveness "/tck-liveness"
#define gt2C "/gt2C.sh"
#define tckLiveness "/tCheckerLiveness.sh"


class TAChecker {

public:
    /**
     * Method used to check if the nrtTA admits a Büchi acceptance condition with a parameter which value is mu > 2C.
     * @param shellScriptPath a path leading to the shell scripts folder.
     * @param inputFilePath a path leading to the file that will be used as input.
     * @param outputFilePath a path leading to the file that will be used as output.
     * @param tCheckerBinPath the path leading to the bin folder of tChecker.
     * @return true if the nrtTA admits a Büchi acceptance condition with a parameter which value is mu > 2C, false otherwise.
     */
    static bool checkMuGreaterThan2C(
            const std::string &shellScriptPath,
            const std::string &inputFilePath,
            const std::string &outputFilePath,
            const std::string &tCheckerBinPath
    ) {
        // We first use this script to substitute all occurrences of a parameter keyword inside the TA translation.
        system(std::string(shellScriptPath + gt2C + " " + inputFilePath + " " + outputFilePath).c_str());
        // Then we call tChecker and collect the result of the liveness analysis.
        std::string tCheckerResult =
                Command::exec(std::string(shellScriptPath + tckLiveness + " " + outputFilePath + " " + tCheckerBinPath + liveness));
        // We get rid of '\n' characters to compare the result with the string "true".
        tCheckerResult.erase(std::remove(tCheckerResult.begin(), tCheckerResult.end(), '\n'), tCheckerResult.cend());

        if (tCheckerResult == "true")
            return true;
        else
            return false;
    }

};

#endif //UTOTPARSER_TACHECKER_H
