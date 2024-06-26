#ifndef UTOTPARSER_STRINGSGETTER_H
#define UTOTPARSER_STRINGSGETTER_H

#include <utility>

#include "utilities/CliHandler.hpp"

#define STRING(x) #x
#define XSTRING(x) STRING(x)


class StringsGetter {

private:
    // Path to the current working directory.
    std::string currentDirPath {};

    // Path to the input directory where files are going to be taken.
    std::string inputDirPath {};

    // Path to the output directory where files are going to be written.
    std::string outputDirPath {};

    // Path to the directory containing shell scripts used for checks.
    std::string scriptsDirPath {};

    // Path to the directory containing shell scripts used for various purposes but checking.
    std::string otherScriptsPath {};

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

    // Path to the directory containing acceptance tiles to be used by the constructor.
    std::string accTilesDirPath {};

    // Path to the directory containing binary tiles to be used by the constructor.
    std::string binTilesDirPath {};

    // Path to the directory containing ternary tiles to be used by the constructor.
    std::string triTilesDirPath {};

    // Path to the directory containing results derived from testing.
    std::string testingResultsDirPath {};

    // Path to the directory containing the resource usage results obtained from tests.
    std::string testingResourceUsageDirPath {};


public:
    explicit StringsGetter(CliHandler &cliHandler) : currentDirPath(XSTRING(SOURCE_ROOT))
    {
        inputDirPath = cliHandler.isCmd(src) ? cliHandler.getCmdArgument(src) : (currentDirPath + "/inputFiles");
        outputDirPath = cliHandler.isCmd(dst) ? cliHandler.getCmdArgument(dst) : (currentDirPath + "/outputs/outputFiles");
        scriptsDirPath = currentDirPath + "/scriptsForChecks";
        outputDirForCheckingPath = currentDirPath + "/outputs/outputFilesForChecking";
        tCheckerBinPath = cliHandler.isCmd(lns) ? cliHandler.getCmdArgument(lns) : XSTRING(TCHECKER_BIN);
        outputPDFsDirPath = currentDirPath + "/outputs/outputPDFs";
        outputDOTsDirPath = currentDirPath + "/outputs/outputDOTs";
        outputDirForCheckingPathLogs = currentDirPath + "/outputs/logs/outputFilesForCheckingLogs";
        inputTilesDirPath = currentDirPath + "/inputTiles";
        accTilesDirPath = inputTilesDirPath + "/accTiles";
        binTilesDirPath = inputTilesDirPath + "/binTiles";
        triTilesDirPath = inputTilesDirPath + "/triTiles";
        testingResultsDirPath = currentDirPath + "/outputs/logs/testingResults";
        otherScriptsPath = currentDirPath + "/scripts";
        testingResourceUsageDirPath = currentDirPath + "/outputs/logs/testingResourceUsage";
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


    [[nodiscard]] const std::string &getAccTilesDirPath() const
    {
        return accTilesDirPath;
    }


    [[nodiscard]] const std::string &getBinTilesDirPath() const
    {
        return binTilesDirPath;
    }


    [[nodiscard]] const std::string &getTriTilesDirPath() const
    {
        return triTilesDirPath;
    }


    [[nodiscard]] const std::string &getTestingResultsDirPath() const
    {
        return testingResultsDirPath;
    }


    [[nodiscard]] const std::string &getOtherScriptsPath() const
    {
        return otherScriptsPath;
    }


    [[nodiscard]] const std::string &getTestingResourceUsageDirPath() const
    {
        return testingResourceUsageDirPath;
    }

};


#endif //UTOTPARSER_STRINGSGETTER_H
