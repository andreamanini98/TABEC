#ifndef UTOTPARSER_STRINGSGETTER_H
#define UTOTPARSER_STRINGSGETTER_H

#include <utility>

#include "utilities/CliHandler.hpp"

#define STRING(x) #x
#define XSTRING(x) STRING(x)


class StringsGetter {

private:
    CliHandler &cliHandler;

    // Path to the current working directory.
    std::string currentDirPath = XSTRING(SOURCE_ROOT);
    // Path to the input directory where files are going to be taken.
    std::string inputDirPath = cliHandler.isCmd(src) ? ((*(cliHandler.getArgv_p()))[cliHandler.getPos(src) + 1]) : (currentDirPath + "/inputFiles");
    // Path to the output directory where files are going to be written.
    std::string outputDirPath = cliHandler.isCmd(dst) ? ((*(cliHandler.getArgv_p()))[cliHandler.getPos(dst) + 1]) : (currentDirPath + "/outputFiles");

    // Path to the directory containing shell scripts.
    std::string scriptsDirPath = currentDirPath + "/scriptsForChecks";
    // Path to the directory containing TA descriptions where parameters have been substituted with appropriate values.
    std::string outputDirForCheckingPath = currentDirPath + "/outputFilesForChecking";
    // Path to the bin folder of the installed tChecker tool (set during build with cmake -D).
    std::string tCheckerBinPath = cliHandler.isCmd(lns) ? ((*(cliHandler.getArgv_p()))[cliHandler.getPos(lns) + 1]) : XSTRING(TCHECKER_BIN);

    // Path to the directory where PDF files will be generated starting from a .dot representation.
    std::string outputPDFsDirPath = currentDirPath + "/outputPDFs";
    // Path to the directory where the .dot files will be written.
    std::string outputDOTsDirPath = currentDirPath + "/outputDOTs";

public:
    explicit StringsGetter(CliHandler &cliHandler) : cliHandler(cliHandler) {}

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

    [[nodiscard]] const std::string &getOutputPDFsDirPath() const {
        return outputPDFsDirPath;
    }

    [[nodiscard]] const std::string &getOutputDOTsDirPath() const {
        return outputDOTsDirPath;
    }

};


#endif //UTOTPARSER_STRINGSGETTER_H
