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

    CliHandler &cliHandler;


    /**
     * Helper method used to conveniently get the result from the given string.
     * @param result the string from which to get the result.
     * @return true if the string is equal to "true", false otherwise.
     */
    static bool getFinalResult(const std::string &result)
    {
        if (result == "true")
            return true;
        else
            return false;
    }


    /**
     * Method used to print a string representing the result of the analysis.
     * @param isAccepting a boolean value telling if the TA has an accepting condition or not.
     * @return true if the TA has an accepting condition, false otherwise.
     */
    static bool printAndGetRes(bool isAccepting)
    {
        std::string color = (isAccepting) ? BHGRN : BHRED;
        std::string acceptance = (isAccepting) ? "not" : "";
        std::cout << color << "Language is " << acceptance << " empty!" << rstColor << std::endl;
        return isAccepting;
    }


    /**
     * Method used to tell if a given TA is parametric or not.
     * @param parKwd the keyword that is present in the TA if it is parametric (e.g. a guard like x == parKwd).
     * @return true if the TA is parametric, false otherwise.
     */
    bool isTAParametric(const std::string &parKwd)
    {
        // This string takes into consideration how many occurrences of parKwd appear inside the TA description.
        // When at least one of them is present, it means that the TA is parametric: thanks to -cm1 grep returns immediately.
        std::string parKwdOccurrences = Command::exec("grep -cm1 \"" + parKwd + "\" " + inputFilePath);
        // We pop the last element since it is a '\n' character (since we used grep before).
        parKwdOccurrences.pop_back();
        // If more than 0 elements have been found, the TA is parametric.
        return parKwdOccurrences != "0";
    }


    /**
     * Method used to set the attributes not initialized by the constructor.
     * @param outputFileName the name of the file that is going to be written.
     */
    void setAttributesForChecking(const std::string &outputFileName)
    {
        this->inputFilePath = stringsGetter.getOutputDirPath() + "/" += outputFileName;
        this->lt2COutputFilePath = stringsGetter.getOutputDirForCheckingPath() + "/lt2C_" += outputFileName;
        this->gt2COutputFilePath = stringsGetter.getOutputDirForCheckingPath() + "/gt2C_" += outputFileName;
        this->outputTmpFilePath = stringsGetter.getOutputDirForCheckingPath() + "/lt2C_tmp_" += outputFileName;
    }


    /**
     * Method used to call the script that will substitute the param keywords occurrences in order to subsequently call another
     * script that will check if the TA admits an acceptance condition in the case where the parameter has a value greater than 2C.
     */
    std::string c_gt2C()
    {
        return Command::exec(
                spaceStr({
                                 shellScriptPath + gt2C,    // Script name
                                 inputFilePath,             // $1
                                 gt2COutputFilePath,        // $2
                                 tCheckerBinPath + liveness // $3
                         }));
    }


    /**
     * Method used to call tChecker's liveness tool and return it's result when the TA is not parametric.
     * @return the result obtained by tChecker.
     */
    std::string c_tckLiveness_noPar()
    {
        return Command::exec(
                spaceStr({
                                 shellScriptPath + tckLiveness, // Script name
                                 inputFilePath,                 // $1
                                 tCheckerBinPath + liveness     // $2
                         }));
    }


    /**
     * Method used to run a script that will resize the constants inside the TA in order to subsequently call another script
     * that will check if the TA admits an acceptance condition when the parameter has a value less than 2C.
     * Notice that in this case we take the digits of alpha_mag starting in the second position: thus we consider only its zeros.
     */
    void s_lt2CScale()
    {
        system(
                spaceStr({
                                 shellScriptPath + lt2CScale,                  // Script name
                                 inputFilePath,                                // $1
                                 lt2COutputFilePath,                           // $2
                                 alphaMag.substr(ALPHA_MAG_IGNORE_DIGITS) // $3
                         }).c_str());
    }


    /**
     * Method used to check if the TA admits an acceptance condition in the case of parameter being less than 2C.
     * @return a string containing the result obtained by tChecker.
     */
    std::string c_lt2CCycle()
    {
        return Command::exec(
                spaceStr({
                                 shellScriptPath + lt2CCycle,               // Script name
                                 lt2COutputFilePath,                        // $1
                                 tCheckerBinPath + liveness,                // $2
                                 alphaMag,                                  // $3
                                 outputTmpFilePath,                         // $4
                                 cliHandler.isCmd(all) ? "all" : "lla" // $5
                         }));
    }


    /**
     * Method used to write a given log and, gathering it's last line, return either true or false.
     * @param logger the logger that will write the log.
     * @param log the log to be written. As a convention, its last line should contain only the word 'true' or 'false'.
     * @param nameTA the name of the TA under analysis.
     * @return true if the last line of the log is 'true', false if the last line of the log is 'false'.
     */
    bool writeLogAndGetFinalResult(Logger &logger, const std::string &log, const std::string &nameTA)
    {
        // Start by writing the log.
        logger.writeLog(log, 3);

        // TODO find a better way than using tail to get the last line of the log.
        // Next, the last line of such log is retrieved in order to get the tChecker's result of the liveness analysis.
        std::string tRes = Command::exec("tail -n 1 " + stringsGetter.getOutputDirForCheckingPathLogs() + "/" + nameTA + ".txt");

        // We get rid of eventual '\n' characters to compare the result with the string "true".
        tRes = deleteTrailingNewlines(tRes);

        // At the end, return the result of the analysis from the last word of the log as obtained above.
        return getFinalResult(tRes);
    }


    /**
     * Method used to check if the TA admits a Büchi acceptance condition with a parameter which value is mu > 2C.
     * @param nameTA the name of the TA under analysis.
     * @param logger a logger that will write some notes about the analysis.
     * @return true if the TA admits a Büchi acceptance condition with a parameter which value is mu > 2C, false otherwise.
     */
    bool checkMuGreaterThan2C(const std::string &nameTA, Logger &logger)
    {
        std::cout << "Trying mu > 2C." << std::endl;
        return writeLogAndGetFinalResult(logger, c_gt2C(), nameTA);
    }


    /**
     * Method used to check if the TA admits a Büchi acceptance condition with a parameter which value is mu < 2C.
     * @param nameTA the name of the TA under analysis.
     * @param logger a logger that will write some notes about the analysis.
     * @return true if the TA admits a Büchi acceptance condition with a parameter which value is mu < 2C, false otherwise.
     */
    bool checkMuLessThan2C(const std::string &nameTA, Logger &logger)
    {
        std::cout << "Language may be empty, now trying mu < 2C." << std::endl;
        // We first use this script to scale all occurrences of integers constants inside the TA translation.
        s_lt2CScale();
        return writeLogAndGetFinalResult(logger, c_lt2CCycle(), nameTA);
    }


    /**
     * Method used to check if the TA admits an acceptance condition when it is not parametric.
     * @return true if the TA admits an acceptance condition, false otherwise.
     */
    bool noParCheck()
    {
        // We simply call tChecker and get its result.
        std::cout << "Simply calling tChecker since the TA is not parametric.\n";

        std::string tRes = c_tckLiveness_noPar();

        // We get rid of eventual '\n' characters to compare the result with the string "true".
        tRes = deleteTrailingNewlines(tRes);

        if (getFinalResult(tRes))
            return printAndGetRes(true);
        else
            return printAndGetRes(false);
    }


    /**
     * Method used to check if the TA admits an acceptance condition when it is parametric.
     * @param nameTA the name of the TA under analysis.
     * @return true if the TA admits an acceptance condition, false otherwise.
     */
    bool parCheck(const std::string &nameTA)
    {
        // Creating logger to save information about TA analysis.
        Logger logger(stringsGetter.getOutputDirForCheckingPathLogs(), nameTA + ".txt");

        // We first try to see if the TA admits an acceptance condition with a parameter mu > 2C.
        bool isAccepting = checkMuGreaterThan2C(nameTA, logger);

        if (isAccepting)
            return printAndGetRes(true);
        else
        {
            // If the previous check fails, we try to see if the TA admits an acceptance condition with a parameter mu < 2C.
            isAccepting = checkMuLessThan2C(nameTA, logger);

            if (isAccepting)
                return printAndGetRes(true);
            else
                return printAndGetRes(false);
        }
    }


public:
    TAChecker(StringsGetter &stringsGetter, CliHandler &cliHandler) : stringsGetter(stringsGetter), cliHandler(cliHandler)
    {
        this->shellScriptPath = stringsGetter.getScriptsDirPath();
        this->tCheckerBinPath = stringsGetter.getTCheckerBinPath();
    }


    /**
     * Method used to check if the TA has a Büchi acceptance condition or if it is empty.
     * @param nameTA the name of the TA under analysis.
     * @param outputFileName the name of the file that is going to be written.
     * @return true if TA's language is not empty, false otherwise.
     */
    bool checkTA(const std::string &nameTA)
    {
        setAttributesForChecking(nameTA + ".tck");

        if (isTAParametric("param"))
            return parCheck(nameTA);
        else
            return noParCheck();
    }

};


#endif //UTOTPARSER_TACHECKER_H
