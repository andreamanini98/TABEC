#ifndef UTOTPARSER_TACHECKER_H
#define UTOTPARSER_TACHECKER_H

#include "utilities/CommandReader.hpp"
#include "utilities/Utils.hpp"
#include "utilities/Logger.hpp"
#include "utilities/CliHandler.hpp"
#include "utilities/StringsGetter.hpp"
#include "defines/UPPAALxmlAttributes.h"

#define liveness "/tck-liveness"
#define tckLiveness "/tCheckerLiveness.sh"
#define gt2C "/gt2C.sh"
#define lt2CScale "/lt2CScale.sh"
#define lt2CCycle "/lt2CCycle.sh"

// TODO up to now, even if a TA is not parametric, both these checks are performed, with the loop execution on the second case as well.
//      You should optimize that, maybe putting a check that controls if a file has the keyword "param" in it.


class TAChecker {

private:
    // A path leading to the shell scripts' folder.
    std::string shellScriptPath{};

    // A path leading to the file that will be used as input.
    std::string inputFilePath{};

    // A path leading to the file that will be used as output when checking parameter being < 2C.
    std::string lt2COutputFilePath{};

    // A path leading to the file that will be used as output when checking parameter being > 2C.
    std::string gt2COutputFilePath{};

    // A path leading to the tChecker's bin folder.
    std::string tCheckerBinPath{};

    // A path leading to the file that will be used as temporary output when checking parameter being < 2C.
    std::string outputTmpFilePath{};

    StringsGetter &stringsGetter;

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

    /**
     * Method used to set the attributes not initialized by the constructor.
     * @param outputFileName the name of the file that is going to be written.
     */
    void setAttributesForChecking(const std::string &outputFileName) {
        this->inputFilePath = stringsGetter.getOutputDirPath() + "/" += outputFileName;
        this->lt2COutputFilePath = stringsGetter.getOutputDirForCheckingPath() + "/lt2C_" += outputFileName;
        this->gt2COutputFilePath = stringsGetter.getOutputDirForCheckingPath() + "/gt2C_" += outputFileName;
        this->outputTmpFilePath = stringsGetter.getOutputDirForCheckingPath() + "/lt2C_tmp_" += outputFileName;
    }

    /**
     * Method used to check if the TA admits a Büchi acceptance condition with a parameter which value is mu > 2C.
     * @return true if the TA admits a Büchi acceptance condition with a parameter which value is mu > 2C, false otherwise.
     */
    bool checkMuGreaterThan2C() {
        std::cout << getWordAfterLastSymbol(inputFilePath, '/') << ": trying mu > 2C." << std::endl;

        // We first use this script to substitute all occurrences of a parameter keyword inside the TA translation.
        system(
                spaceStr({
                                 shellScriptPath + gt2C,
                                 inputFilePath,
                                 gt2COutputFilePath}
                ).c_str());

        // Then we call tChecker and collect the result of the liveness analysis.
        std::string tRes =
                Command::exec(
                        spaceStr({
                                         shellScriptPath + tckLiveness,
                                         gt2COutputFilePath,
                                         tCheckerBinPath + liveness}
                        ));

        // We get rid of eventual '\n' characters to compare the result with the string "true".
        tRes.erase(std::remove(tRes.begin(), tRes.end(), '\n'), tRes.cend());

        return getFinalResult(tRes);
    }

    /**
     * Method used to check if the TA admits a Büchi acceptance condition with a parameter which value is mu < 2C.
     * @param nameTA the name of the TA under analysis.
     * @param logger a logger that will write some notes about the analysis.
     * @return true if the TA admits a Büchi acceptance condition with a parameter which value is mu < 2C, false otherwise.
     */
    bool checkMuLessThan2C(const std::string &nameTA, Logger &logger) {
        std::cout << getWordAfterLastSymbol(inputFilePath, '/') << ": language may be empty, now trying mu < 2C." << std::endl;

        // The multiplicative factor that will resize constants inside the TA.
        std::string alpha_mag = std::to_string(ALPHA_MAG);

        // We first use this script to scale all occurrences of integers constants inside the TA translation.
        system(
                spaceStr({
                                 shellScriptPath + lt2CScale,
                                 inputFilePath,
                                 lt2COutputFilePath,
                                 alpha_mag.substr(1)}
                ).c_str());

        // Then we call tChecker and collect the result of the liveness analysis. This will end up in a log file with details about the execution.
        std::string log =
                Command::exec(
                        spaceStr({
                                         shellScriptPath + lt2CCycle,
                                         lt2COutputFilePath,
                                         tCheckerBinPath + liveness,
                                         alpha_mag,
                                         outputTmpFilePath}
                        ));

        // Log is then written into a file.
        logger.writeLog(log, 2);

        // Next, the last line of such log is retrieved in order to get the tChecker's result of the liveness analysis.
        std::string tRes = Command::exec("tail -n 1 " + stringsGetter.getOutputDirForCheckingPathLogs() + "/" + nameTA + ".txt");

        // We get rid of eventual '\n' characters to compare the result with the string "true".
        tRes.erase(std::remove(tRes.begin(), tRes.end(), '\n'), tRes.cend());

        return getFinalResult(tRes);
    }


public:
    explicit TAChecker(StringsGetter &stringsGetter) : stringsGetter(stringsGetter) {
        this->shellScriptPath = stringsGetter.getScriptsDirPath();
        this->tCheckerBinPath = stringsGetter.getTCheckerBinPath();
    }

    /**
     * Method used to check if the TA has a Büchi acceptance condition or if it is empty.
     * @param nameTA the name of the TA under analysis.
     * @param outputFileName the name of the file that is going to be written.
     * @return true if TA's language is not empty, false otherwise.
     */
    bool checkTA(const std::string &nameTA, const std::string &outputFileName) {
        setAttributesForChecking(outputFileName);

        Logger logger(stringsGetter.getOutputDirForCheckingPathLogs(), nameTA + ".txt");

        // We first try to see if the TA admits an acceptance condition with a parameter mu > 2C.
        bool isThereAnAcceptanceCondition = checkMuGreaterThan2C();

        if (isThereAnAcceptanceCondition) {
            std::cout << BHGRN << outputFileName << "'s language is not empty!" << reset << std::endl;
            return true;
        } else {
            // If the previous check fails, we try to see if the TA admits an acceptance condition with a parameter mu < 2C.
            isThereAnAcceptanceCondition = checkMuLessThan2C(nameTA, logger);

            if (isThereAnAcceptanceCondition) {
                std::cout << BHGRN << outputFileName << "'s language is not empty!" << reset << std::endl;
                return true;
            } else {
                std::cout << BHRED << outputFileName << "'s language is empty!" << reset << std::endl;
                return false;
            }
        }
    }

};

#endif //UTOTPARSER_TACHECKER_H
