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


class TAChecker {

private:
    // A path leading to the shell scripts' folder.
    std::string shellScriptPath {};

    // A path leading to the file that will be used as input.
    std::string inputFilePath {};

    // A path leading to the file that will be used as output when checking parameter being < 2C.
    std::string lt2COutputFilePath {};

    // A path leading to the file that will be used as output when checking parameter being > 2C.
    std::string gt2COutputFilePath {};

    // A path leading to the tChecker's bin folder.
    std::string tCheckerBinPath {};

    // A path leading to the file that will be used as temporary output when checking parameter being < 2C.
    std::string outputTmpFilePath {};

    // The multiplicative factor that will resize constants inside the TA.
    std::string alphaMag = std::to_string(ALPHA_MAG);

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
     * Method used to print a string representing the result of the analysis.
     * @param nameTA the name of the TA under analysis.
     * @param isAccepting a boolean value telling if the TA has an accepting condition or not.
     * @return true if the TA has an accepting condition, false otherwise.
     */
    static bool printAndGetRes(const std::string &nameTA, bool isAccepting) {
        std::string color = (isAccepting) ? BHGRN : BHRED;
        std::string acceptance = (isAccepting) ? "not" : "";
        std::cout << color << nameTA << "'s language is " << acceptance << " empty!" << reset << std::endl;
        return isAccepting;
    }

    /**
     * Method used to tell if a given TA is parametric or not.
     * @param parKwd the keyword that is present in the TA if it is parametric (e.g. a guard like x == parKwd).
     * @return true if the TA is parametric, false otherwise.
     */
    bool isTAParametric(const std::string &parKwd) {
        // This string takes into consideration how many occurrences of parKwd appear inside the TA description.
        // When at least one of them is present, it means that the TA is parametric: thanks to -cm1 grep returns immediately.
        std::string parKwdOccurrences = Command::exec("grep -cm1 \"" + parKwd + "\" " + inputFilePath);
        // We pop the last element since it is a '\n' character.
        parKwdOccurrences.pop_back();
        // If more than 0 elements have been found, the TA is parametric.
        return parKwdOccurrences != "0";
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

    // TODO you can unify s_gt2C and c_tckLiveness_gt2C in a single script by looking at how you did in the cycle checking.

    /**
     * Method used to call the script that will substitute the param keywords occurrences in order to subsequently call another
     * script that will check if the TA admits an acceptance condition in the case where the parameter has a value greater than 2C.
     */
    void s_gt2C() {
        system(
                spaceStr({
                                 shellScriptPath + gt2C,
                                 inputFilePath,
                                 gt2COutputFilePath
                         }).c_str());
    }

    /**
     * Method used to call tChecker's liveness tool and return it's result when the parameter has a value greater than 2C.
     * @return the result obtained by tChecker.
     */
    std::string c_tckLiveness_gt2C() {
        return Command::exec(
                spaceStr({
                                 shellScriptPath + tckLiveness,
                                 gt2COutputFilePath,
                                 tCheckerBinPath + liveness
                         }));
    }

    /**
     * Method used to call tChecker's liveness tool and return it's result when the TA is not parametric.
     * @return the result obtained by tChecker.
     */
    std::string c_tckLiveness_noPar() {
        return Command::exec(
                spaceStr({
                                 shellScriptPath + tckLiveness,
                                 inputFilePath,
                                 tCheckerBinPath + liveness
                         }));
    }

    /**
     * Method used to run a script that will resize the constants inside the TA in order to subsequently call another script
     * that will check if the TA admits an acceptance condition when the parameter has a value less than 2C.
     * Notice that in this case we take the digits of alpha_mag starting in the second position: thus we consider only its zeros.
     */
    void s_lt2CScale() {
        system(
                spaceStr({
                                 shellScriptPath + lt2CScale,
                                 inputFilePath,
                                 lt2COutputFilePath,
                                 alphaMag.substr(ALPHA_MAG_IGNORE_DIGITS)
                         }).c_str());
    }

    /**
     * Method used to check if the TA admits an acceptance condition in the case of parameter being less than 2C.
     * @return a string containing the result obtained by tChecker.
     */
    std::string c_lt2CCycle() {
        return Command::exec(
                spaceStr({
                                 shellScriptPath + lt2CCycle,
                                 lt2COutputFilePath,
                                 tCheckerBinPath + liveness,
                                 alphaMag,
                                 outputTmpFilePath
                         }));
    }

    /**
     * Method used to check if the TA admits a Büchi acceptance condition with a parameter which value is mu > 2C.
     * @param nameTA the name of the TA under analysis.
     * @return true if the TA admits a Büchi acceptance condition with a parameter which value is mu > 2C, false otherwise.
     */
    bool checkMuGreaterThan2C(const std::string &nameTA) {
        std::cout << nameTA << ": trying mu > 2C." << std::endl;

        // We first use this script to substitute all occurrences of a parameter keyword inside the TA translation.
        s_gt2C();

        // Then we call tChecker and collect the result of the liveness analysis.
        std::string tRes = c_tckLiveness_gt2C();

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
        std::cout << nameTA << ": language may be empty, now trying mu < 2C." << std::endl;

        // We first use this script to scale all occurrences of integers constants inside the TA translation.
        s_lt2CScale();

        // Then we call tChecker and collect the result of the liveness analysis.
        // This will end up in a log file with details about the execution.
        std::string log = c_lt2CCycle();

        // Log is then written into a file.
        logger.writeLog(log, 3);

        // Next, the last line of such log is retrieved in order to get the tChecker's result of the liveness analysis.
        std::string tRes = Command::exec("tail -n 1 " + stringsGetter.getOutputDirForCheckingPathLogs() + "/" + nameTA + ".txt");

        // We get rid of eventual '\n' characters to compare the result with the string "true".
        tRes.erase(std::remove(tRes.begin(), tRes.end(), '\n'), tRes.cend());

        return getFinalResult(tRes);
    }

    /**
     * Method used to check if the TA admits an acceptance condition when it is not parametric.
     * @param nameTA the name of the TA under analysis
     * @return true if the TA admits an acceptance condition, false otherwise.
     */
    bool noParCheck(const std::string &nameTA) {
        // We simply call tChecker and get its result.
        std::cout << "Simply calling tChecker since the TA is not parametric.\n";

        std::string tRes = c_tckLiveness_noPar();
        // We get rid of eventual '\n' characters to compare the result with the string "true".
        // Since we're calling directly tChecker, pop_back() would suffice.
        tRes.pop_back();

        if (getFinalResult(tRes))
            return printAndGetRes(nameTA, true);
        else
            return printAndGetRes(nameTA, false);
    }

    /**
     * Method used to check if the TA admits an acceptance condition when it is parametric.
     * @param nameTA the name of the TA under analysis.
     * @return true if the TA admits an acceptance condition, false otherwise.
     */
    bool parCheck(const std::string &nameTA) {
        // Creating logger to save information about TA analysis.
        Logger logger(stringsGetter.getOutputDirForCheckingPathLogs(), nameTA + ".txt");

        // We first try to see if the TA admits an acceptance condition with a parameter mu > 2C.
        bool isAccepting = checkMuGreaterThan2C(nameTA);

        if (isAccepting) {
            return printAndGetRes(nameTA, true);
        } else {
            // If the previous check fails, we try to see if the TA admits an acceptance condition with a parameter mu < 2C.
            isAccepting = checkMuLessThan2C(nameTA, logger);

            if (isAccepting)
                return printAndGetRes(nameTA, true);
            else
                return printAndGetRes(nameTA, false);
        }
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
    bool checkTA(const std::string &nameTA) {
        setAttributesForChecking(nameTA + ".tck");

        if (isTAParametric("param"))
            return parCheck(nameTA);
        else
            return noParCheck(nameTA);
    }

};

#endif //UTOTPARSER_TACHECKER_H
