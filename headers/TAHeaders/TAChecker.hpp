#ifndef UTOTPARSER_TACHECKER_H
#define UTOTPARSER_TACHECKER_H

#include "utilities/CommandReader.hpp"
#include "utilities/Utils.hpp"
#include "defines/UPPAALxmlAttributes.h"

#define liveness "/tck-liveness"
#define tckLiveness "/tCheckerLiveness.sh"
#define gt2C "/gt2C.sh"
#define lt2CScale "/lt2CScale.sh"
#define lt2CCycle "/lt2CCycle.sh"


class TAChecker {

private:
    /**
     * Helper method used to conveniently get the result from the given string.
     * @param result the string from which to get the result.
     * @return true if the string is equal to "true", false otherwise.
     */
    static bool getFinalResult(const std::string &result) {
        if (result == "true")
            return true;
        else
            return false;
    }


public:
    // TODO up to now, even if a TA is not parametric, both these checks are performed, with the loop execution on the second case as well.
    //      You should optimize that, maybe putting a check that controls if a file has the keyword "param" in it.

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
        std::cout << getWordAfterLastSymbol(inputFilePath, '/') << ": trying mu > 2C." << std::endl;

        // We first use this script to substitute all occurrences of a parameter keyword inside the TA translation.
        system((shellScriptPath + gt2C + " " + inputFilePath + " " + outputFilePath).c_str());

        // Then we call tChecker and collect the result of the liveness analysis.
        std::string tCheckerResult = Command::exec(shellScriptPath + tckLiveness + " " + outputFilePath + " " + tCheckerBinPath + liveness);

        // We get rid of eventual '\n' characters to compare the result with the string "true".
        tCheckerResult.erase(std::remove(tCheckerResult.begin(), tCheckerResult.end(), '\n'), tCheckerResult.cend());

        return getFinalResult(tCheckerResult);
    }

    /**
     * Method used to check if the nrtTA admits a Büchi acceptance condition with a parameter which value is mu < 2C.
     * @param shellScriptPath a path leading to the shell scripts folder.
     * @param inputFilePath a path leading to the file that will be used as input.
     * @param outputFilePath a path leading to the file that will be used as output.
     * @param tCheckerBinPath the path leading to the bin folder of tChecker.
     * @param outputTmpFilePath a path leading to the file that will be used as temporary output.
     * @return true if the nrtTA admits a Büchi acceptance condition with a parameter which value is mu < 2C, false otherwise.
     */
    static bool checkMuLessThan2C(
            const std::string &shellScriptPath,
            const std::string &inputFilePath,
            const std::string &outputFilePath,
            const std::string &tCheckerBinPath,
            const std::string &outputTmpFilePath
    ) {
        std::string alpha_mag = std::to_string(ALPHA_MAG);
        std::cout << getWordAfterLastSymbol(inputFilePath, '/') << ": language may be empty, now trying mu < 2C." << std::endl;

        // We first use this script to scale all occurrences of integers constants inside the TA translation.
        system((shellScriptPath + lt2CScale + " " + inputFilePath + " " + outputFilePath + " " + alpha_mag.substr(1)).c_str());

        // Then we call tChecker and collect the result of the liveness analysis.
        std::string tCheckerResult = Command::exec(shellScriptPath + lt2CCycle + " " + outputFilePath + " " + tCheckerBinPath + liveness + " " + alpha_mag + " " + outputTmpFilePath);

        // We get rid of eventual '\n' characters to compare the result with the string "true".
        tCheckerResult.erase(std::remove(tCheckerResult.begin(), tCheckerResult.end(), '\n'), tCheckerResult.cend());

        return getFinalResult(tCheckerResult);
    }

};

#endif //UTOTPARSER_TACHECKER_H
