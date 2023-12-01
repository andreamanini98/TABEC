#ifndef UTOTPARSER_STRINGSGETTER_H
#define UTOTPARSER_STRINGSGETTER_H

#include <utility>

#include "utilities/CliHandler.hpp"

#define STRING(x) #x
#define XSTRING(x) STRING(x)


class StringsGetter {

    // TODO: maybe here you can initialize all the attributes directly in the initializer list.

private:
    // Path to the current working directory.
    std::string currentDirPath {};

    // Path to the input directory where files are going to be taken.
    std::string inputDirPath {};

    // Path to the output directory where files are going to be written.
    std::string outputDirPath {};

    // Path to the directory containing shell scripts.
    std::string scriptsDirPath {};

    // Path to the directory containing TA descriptions where parameters have been substituted with appropriate values.
    std::string outputDirForCheckingPath {};

    // Path to the bin folder of the installed tChecker tool (set during build with cmake -D).
    std::string tCheckerBinPath {};

    // Path to the directory where PDF files will be generated starting from a .dot representation.
    std::string outputPDFsDirPath {};

    // Path to the directory where the .dot files will be written.
    std::string outputDOTsDirPath {};

    // Path to the directory containing logs derived from the execution of shell scripts.
    std::string outputDirForCheckingPathLogs {};

    // Path to the directory containing tiles to be used by the constructor.
    std::string inputTilesDirPath {};


public:
    explicit StringsGetter(CliHandler &cliHandler)
    {
        currentDirPath = XSTRING(SOURCE_ROOT);
        inputDirPath = cliHandler.isCmd(src) ? cliHandler.getCmdArgument(src) : (currentDirPath + "/inputFiles");
        outputDirPath = cliHandler.isCmd(dst) ? cliHandler.getCmdArgument(dst) : (currentDirPath + "/outputs/outputFiles");
        scriptsDirPath = currentDirPath + "/scriptsForChecks";
        outputDirForCheckingPath = currentDirPath + "/outputs/outputFilesForChecking";
        tCheckerBinPath = cliHandler.isCmd(lns) ? cliHandler.getCmdArgument(lns) : XSTRING(TCHECKER_BIN);
        outputPDFsDirPath = currentDirPath + "/outputs/outputPDFs";
        outputDOTsDirPath = currentDirPath + "/outputs/outputDOTs";
        outputDirForCheckingPathLogs = currentDirPath + "/outputs/logs/outputFilesForCheckingLogs";
        inputTilesDirPath = currentDirPath + "/inputTiles";
    }


    [[nodiscard]] const std::string &getInputDirPath() const
    {
        return inputDirPath;
    }


    [[nodiscard]] const std::string &getOutputDirPath() const
    {
        return outputDirPath;
    }


    [[nodiscard]] const std::string &getScriptsDirPath() const
    {
        return scriptsDirPath;
    }


    [[nodiscard]] const std::string &getOutputDirForCheckingPath() const
    {
        return outputDirForCheckingPath;
    }


    [[nodiscard]] const std::string &getTCheckerBinPath() const
    {
        return tCheckerBinPath;
    }


    [[nodiscard]] const std::string &getOutputPDFsDirPath() const
    {
        return outputPDFsDirPath;
    }


    [[nodiscard]] const std::string &getOutputDOTsDirPath() const
    {
        return outputDOTsDirPath;
    }


    [[nodiscard]] const std::string &getOutputDirForCheckingPathLogs() const
    {
        return outputDirForCheckingPathLogs;
    }


    [[nodiscard]] const std::string &getInputTilesDirPath() const
    {
        return inputTilesDirPath;
    }

};


#endif //UTOTPARSER_STRINGSGETTER_H
