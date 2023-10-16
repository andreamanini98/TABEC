#ifndef UTOTPARSER_STRINGSGETTER_H
#define UTOTPARSER_STRINGSGETTER_H

#include <utility>

#include "utilities/CliHandler.hpp"

#define STRING(x) #x
#define XSTRING(x) STRING(x)


class StringsGetter {

private:
    CliHandler cliHandler;

    std::string tChecker_bin = "/Users/echo/Desktop/PoliPrograms/tchecker-0.8/bin";

    std::string currentDirPath = XSTRING(SOURCE_ROOT);
    std::string inputDirPath = cliHandler.isCmd(src) ? (*(cliHandler.getArgv_p())[cliHandler.getPos(src) + 1]) : (currentDirPath + "/inputFiles");
    std::string outputDirPath = cliHandler.isCmd(dst) ? (*(cliHandler.getArgv_p())[cliHandler.getPos(dst) + 1]) : (currentDirPath + "/outputFiles");

    // Path to the directory containing shell scripts.
    std::string scriptsDirPath = currentDirPath + "/scriptsForChecks";
    // Path to the directory containing TA descriptions where parameters have been substituted with appropriate values.
    std::string outputDirForCheckingPath = currentDirPath + "/outputFilesForChecking";
    // Path to the bin folder of the installed tChecker tool (set during build with cmake -D).
    std::string tCheckerBinPath = XSTRING(TCHECKER_BIN);

public:
    explicit StringsGetter(CliHandler cliHandler) : cliHandler(std::move(cliHandler)) {}

    [[nodiscard]] const std::string &getInputDirPath() const {
        return inputDirPath;
    }

    [[nodiscard]] const std::string &getOutputDirPath() const {
        return outputDirPath;
    }

    [[nodiscard]] const std::string &getScriptsDirPath() const {
        return scriptsDirPath;
    }

    [[nodiscard]] const std::string &getOutputDirForCheckingPath() const {
        return outputDirForCheckingPath;
    }

    [[nodiscard]] const std::string &getTCheckerBinPath() const {
        return tCheckerBinPath;
    }

    [[nodiscard]] const std::string &getTChecker_bin() const {
        return tChecker_bin;
    }

};


#endif //UTOTPARSER_STRINGSGETTER_H
