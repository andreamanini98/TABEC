#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"

#include "utilities/Utils.hpp"
#include "utilities/StringsGetter.hpp"
#include "utilities/CliHandler.hpp"
#include "TAHeaders/TADotConverter.hpp"
#include "TAHeaders/TAutotTranslator.hpp"
#include "TAHeaders/TATileHeaders/regexGenerator/TATileRegExGenerator.hpp"
#include "TAHeaders/TATileHeaders/regexGenerator/TATileRegExGeneratorStrict.hpp"
#include "TAHeaders/TATileHeaders/lexerAndParser/TATileInputParser.hpp"
#include "utilities/Logger.hpp"
#include "TAHeaders/TATileHeaders/TATileRenamer.hpp"
#include "utilities/CommandReader.hpp"

using json = nlohmann::json;


/**
 * Function used to print the TiledTA.
 * @param tiledTA the TiledTA to print.
 */
void printTiledTA(const json &tiledTA)
{
    std::cout << "\n\nResultingTiledTA:\n\n";
    std::cout << std::setw(4) << tiledTA << std::endl;
}


/**
 * Function used to convert a TA to a .dot file.
 * @param outputDOTDirPath the path to the directory where the .dot file will be written.
 * @param tiledTAName the name of the Tiled TA.
 * @param tiledTA a json representation of the Tiled TA to convert into .dot format.
 */
void convertTiledTAtoDOT(const std::string &outputDOTDirPath, const std::string &tiledTAName, const json &tiledTA)
{
    TADotConverter taDotConverter(outputDOTDirPath + "/" += (tiledTAName + ".dot"));
    taDotConverter.translateTAtoDot(tiledTAName, tiledTA);
}


/**
 * Function used to convert a TA to a .tck file.
 * @param outputDirPath the path to the directory where the .tck file will be written.
 * @param tiledTAName the name of the Tiled TA.
 * @param tiledTA a json representation of the Tiled TA to convert into .tck format.
 */
void convertTiledTAtoTCK(const std::string &outputDirPath, const std::string &tiledTAName, const json &tiledTA)
{
    Translator translator(outputDirPath + "/" += (tiledTAName + ".tck"));
    translator.translateTA(tiledTAName, tiledTA);
    std::cout << BHGRN << "Conversion successful" << rstColor << std::endl;
}


/**
 * Function used to clean directories before starting the automatic test generation.
 * @param stringsGetter a 'StringsGetter' that will retrieve the paths for the directories to clean.
 */
void cleanDirectories(StringsGetter &stringsGetter)
{
    deleteDirectoryContents(stringsGetter.getOutputDirForCheckingPathLogs());
    deleteDirectoryContents(stringsGetter.getOutputDOTsDirPath());
    deleteDirectoryContents(stringsGetter.getOutputDirPath());
    deleteDirectoryContents(stringsGetter.getOutputDirForCheckingPath());
    deleteDirectoryContents(stringsGetter.getOutputPDFsDirPath());
    deleteDirectoryContents(stringsGetter.getTestingResultsDirPath());
    deleteDirectoryContents(stringsGetter.getTestingResourceUsageDirPath());
}


/**
 * Function used to collect the name of all the used tiles inside the randomly-generated regular expression.
 * @param regEx the regular expression in which to look for the used tiles.
 * @param parser an instance of the parser.
 * @return a vector containing the name of all the used tiles.
 */
std::vector<std::string> gatherUsedTiles(const std::string &regEx, TATileInputParser &parser)
{
    std::vector<std::string> usedTiles {};

    for (const std::string &tile: parser.getBinTileSymbols())
        if (regEx.find(tile) != std::string::npos)
            usedTiles.push_back(tile);

    for (const std::string &tile: parser.getTriTileSymbols())
        if (regEx.find(tile) != std::string::npos)
            usedTiles.push_back(tile);

    return usedTiles;
}


/**
 * Function used to write additional information in the logs, useful for analyzing test results.
 * @param stringsGetter a string getter.
 * @param parser an instance of the parser.
 * @param TAName the name of the randomly-generated TA.
 * @param regEx the regular expression used to generate the TA.
 */
void writeLogs(StringsGetter &stringsGetter, TATileInputParser &parser, const std::string &TAName, const std::string &regEx)
{
    Logger logger(stringsGetter.getOutputDirForCheckingPathLogs(), TAName + ".txt");

    std::string logToWrite { "RegEx generated:\n" + regEx + "\n" };

    std::vector<std::string> usedTiles { gatherUsedTiles(regEx, parser) };
    if (!usedTiles.empty())
    {
        logToWrite.append("Used tiles (random and accepting tiles excluded):\n");
        for (const std::string &tile: usedTiles)
            logToWrite.append(tile + " ");
    }
    logger.writeLog(logToWrite.append("\n\n"));
}


/**
 * Function which, by calling a shell script, collects all the results in a single file.
 * @param stringsGetter a strings getter.
 */
void gatherResults(StringsGetter &stringsGetter, CliHandler &cliHandler)
{
    system(
            spaceStr({
                             stringsGetter.getOtherScriptsPath() + "/collectResults.sh", // Script name
                             stringsGetter.getOutputDirForCheckingPathLogs(),            // $1
                             stringsGetter.getTestingResultsDirPath(),                   // $2
                             stringsGetter.getTCheckerBinPath(),                         // $3
                             std::to_string(cliHandler.isCmd(atc)), // $4
                             std::to_string(cliHandler.isCmd(atp))  // $5
                     }).c_str());
}


/**
 * Function used to get the mean out of a set of values obtained by running the 'cmd' command.
 * @param totalRuns the total number of elements over which we compute the mean.
 * @param cmd the command used to get the set of values to average.
 * @return an average of the collected values.
 */
double getMeanResourceUsage(int totalRuns, const std::string &cmd)
{
    // TODO: you can merge this function with the peak one, use th same for loop and return a pair of doubles.
    // Collecting the set of values (as strings) to average.
    std::string resourceValues { Command::exec(cmd) };

    // Tokenizing the previous values in order to get single values.
    std::vector<std::string> tokenizedRV { getTokenizedString(resourceValues, '\n') };

    // The maximum value a double can get is 1.79769e+308. For this reason, no side check on overflows are made, since
    // either for memory or time it would be more than enough to fit the total inside while performing the sums below.
    double meanValue { 0 };
    for (const std::string &rv: tokenizedRV)
        meanValue += std::stod(rv);

    return meanValue / totalRuns;
}


/**
 * Function used to get the maximum out of a set of values obtained by running the 'cmd' command.
 * @param cmd the command used to get the set of values from which to get the maximum.
 * @return the maximum of the collected values.
 */
double getPeakResourceUsage(const std::string &cmd)
{
    // Collecting the set of values (as strings) to average.
    std::string resourceValues { Command::exec(cmd) };

    // Tokenizing the previous values in order to get single values.
    std::vector<std::string> tokenizedRV { getTokenizedString(resourceValues, '\n') };

    // Getting the maximum out of the set of collected values.
    double maxValue { 0 };
    for (const std::string &rv: tokenizedRV)
        maxValue = (std::stod(rv) > maxValue) ? std::stod(rv) : maxValue;

    return maxValue;
}


/**
 * Function used to print to a file the resources utilization obtained after running tests.
 * @param stringsGetter a strings getter.
 */
void gatherResourcesUsage(StringsGetter &stringsGetter)
{
    for (const auto &entry: getEntriesInAlphabeticalOrder(stringsGetter.getTestingResourceUsageDirPath()))
    {
        std::string entryPath { static_cast<std::string>(entry.path()) };
        std::string nameTA { getStringGivenPosAndToken(getWordAfterLastSymbol(entry.path(), '/'), '.', 0) };

        std::ofstream out;
        std::string outputFileName { "ResourceUsages.txt" };
        out.open(stringsGetter.getTestingResultsDirPath() + "/" + outputFileName, std::ofstream::out | std::ofstream::app);

        // Now getting the total number of runs in order to use it in computing averages.
        // The number of total runs is equal to the number of times tChecker has been called during the check
        // of the parameter being > 2C and the parameter being < 2C, conditioned by the fact of having enabled the
        // option to get the full range of suitable parameters and not stopping at the first one found.
        // Please note that this should work also for non-parametric TAs.

        std::string totalRuns_command { "grep -c \"CYCLE\" " + entryPath };
        int totalRuns { std::stoi(Command::exec(totalRuns_command)) };

        // Now getting the average and peak resources' utilization.
        // The meanRunningTime will be the mean time spent by executing the algorithm in tChecker (e.g. the couvscc).
        // The meanExecutionTime will be the mean time spent executing tChecker (plus some small overhead for the shell script).

        std::string runningTime_command { "grep \"RUNNING_TIME_SECONDS\" " + entryPath + " | awk '{print $NF}'" };
        double meanRunningTime { getMeanResourceUsage(totalRuns, runningTime_command) };
        double peakRunningTime { getPeakResourceUsage(runningTime_command) };

        std::string memoryMaxRSS_command { "grep \"MEMORY_MAX_RSS\" " + entryPath + " | awk '{print $NF}'" };
        double meanMemoryMaxRSS { getMeanResourceUsage(totalRuns, memoryMaxRSS_command) };
        double peakMemoryMaxRSS { getPeakResourceUsage(memoryMaxRSS_command) };

        std::string storedStates_command { "grep \"STORED_STATES\" " + entryPath + " | awk '{print $NF}'" };
        double meanStoredStates { getMeanResourceUsage(totalRuns, storedStates_command) };

        std::string visitedStates_command { "grep \"VISITED_STATES\" " + entryPath + " | awk '{print $NF}'" };
        double meanVisitedStates { getMeanResourceUsage(totalRuns, visitedStates_command) };

        std::string visitedTransitions_command { "grep \"VISITED_TRANSITIONS\" " + entryPath + " | awk '{print $NF}'" };
        double meanVisitedTransitions { getMeanResourceUsage(totalRuns, visitedTransitions_command) };

        std::string executionTime_command { "grep \"EXECUTION_TIME\" " + entryPath + " | awk '{print $NF}'" };
        double meanExecutionTime { getMeanResourceUsage(totalRuns, executionTime_command) };
        double peakExecutionTime { getPeakResourceUsage(executionTime_command) };

        // Now printing results to the file.

        out << nameTA << '\n';
        out << std::string(nameTA.length(), '-') << '\n';
        out << "Total number of runs:                        " << totalRuns << '\n';
        out << "Mean algorithm running time [milliseconds]:  " << meanRunningTime * 1000 << '\n';
        out << "Peak algorithm running time [milliseconds]:  " << peakRunningTime * 1000 << '\n';
        out << "Mean tChecker running time [milliseconds]:   " << meanExecutionTime << '\n';
        out << "Peak tChecker running time [milliseconds]:   " << peakExecutionTime << '\n';
        out << "Mean maximum memory utilization [Bytes]:     " << meanMemoryMaxRSS << '\n';
        out << "Peak maximum memory utilization [Bytes]:     " << peakMemoryMaxRSS << '\n';
        out << "Mean number of stored states:                " << meanStoredStates << '\n';
        out << "Mean number of visited states:               " << meanVisitedStates << '\n';
        out << "Mean number of visited transitions:          " << meanVisitedTransitions << '\n';
        out << "\n\n";

        out.flush();
        out.close();
    }
}


int main(int argc, char *argv[])
{
    // TODO: you should put in the documentation that MacOS users need coreutils to be installed via homebrew.

    // TODO: also add comments explaining the differences between algorithm running time and tchecker running time.

    CliHandler cliHandler(&argc, &argv);
    StringsGetter stringsGetter(cliHandler);
    TATileInputParser taTileInputParser(stringsGetter);

    // Cleaning directories before starting test generation.
    cleanDirectories(stringsGetter);

    // Integer indicating the maximum number of states randomly-generated tiles will have.
    int maxNumOfRandomStates { cliHandler.isCmd(sup) ? std::stoi(cliHandler.getCmdArgument(sup)) : 5 };

    // Now generating the right regex generator based on the given cli command.
    TATileRegExGenerator *taTileRegExGenerator;

    if (cliHandler.isCmd(tns))
        taTileRegExGenerator = new TATileRegExGenerator(std::stoi(cliHandler.getCmdArgument(tns)));
    else if (cliHandler.isCmd(tst))
    {
        taTileRegExGenerator = new TATileRegExGeneratorStrict(
                std::stoi(cliHandler.getCmdArgument(tst)), maxNumOfRandomStates,
                taTileInputParser.getRngTileTokens(),
                taTileInputParser.getAccTileTokens(),
                taTileInputParser.getBinTileTokens(),
                taTileInputParser.getTriTileTokens());
    } else
    {
        std::cerr << BHRED << "Invalid argument specified for running the tester." << rstColor << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Starting generating random tests.\n";

    int numTests = cliHandler.isCmd(nbt) ? std::stoi(cliHandler.getCmdArgument(nbt)) : 10;
    for (int i = 0; i < numTests; i++)
    {
        // Resetting the nonce at each test generation in order to avoid the index becoming too big.
        TATileRenamer::resetTANonce();

        std::string regEx { taTileRegExGenerator->generateRegEx() };
        std::cout << "Obtained string:\n" << regEx << "\n";

        json tiledTA = taTileInputParser.getTiledTA(regEx);

        std::string TAName = "RegExTA_" + std::to_string(i + 1);

        writeLogs(stringsGetter, taTileInputParser, TAName, regEx);

        printTiledTA(tiledTA);
        convertTiledTAtoDOT(stringsGetter.getOutputDOTsDirPath(), TAName, tiledTA);
        convertTiledTAtoTCK(stringsGetter.getOutputDirPath(), TAName, tiledTA);
    }

    gatherResults(stringsGetter, cliHandler);
    gatherResourcesUsage(stringsGetter);

    return EXIT_SUCCESS;
}

