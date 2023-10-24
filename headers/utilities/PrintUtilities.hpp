#include "Structs.h"

// Used to create columns for 'Conversion' and 'Emptiness'.
#define FIXED_COL_WIDTH 12
// Used to add a padding to the maximum width found scanning the names of the TAs to show.
#define TA_NAME_COL_WIDTH_PADDING 2


/**
 * Function used to print the title and summary information on the started conversion.
 * @param nameTA the name of the TA.
 * @param path the path where the source of the given TA is located.
 * @param outputFileName the name of the output file.
 * @param outputDirPath the directory in which the output file will be located.
 */
void printTitle(const std::string &nameTA, const std::string &path, const std::string &outputFileName, const std::string &outputDirPath) {
    std::cout << "\n-------- " << nameTA << " --------\n";
    std::cout << "Starting conversion of file: " <<
              std::endl << path <<
              std::endl << "as " << outputFileName <<
              std::endl << "in dest directory: " << outputDirPath << std::endl;
}

/**
 * This function is used to print in a tabular manner the results of the process:
 * - Green ball: success.
 * - Red ball: failure.
 * @param dashboardResults a vector of structs containing useful information for the table printing.
 * @param onlyConversion used to print only conversion results.
 * @param onlyEmptiness used to print only emptiness results.
 */
void printDashBoard(const std::vector<DashBoardEntry> &dashboardResults, bool onlyConversion = false, bool onlyEmptiness = false) {
    // Getting the column width for 'TA name' from the longest name we have to display in the dashboard.
    // By default, columnWidth has a minimum value of FIXED_COL_WIDTH.
    int nameColumnWidth = FIXED_COL_WIDTH;
    for (auto &dEntry: dashboardResults)
        nameColumnWidth = (dEntry.nameTA.length() > nameColumnWidth) ? static_cast<int>(dEntry.nameTA.length()) : nameColumnWidth;
    // We add a padding only if we have a width greater than the minimum required.
    nameColumnWidth += (nameColumnWidth == FIXED_COL_WIDTH) ? 0 : TA_NAME_COL_WIDTH_PADDING;

    // Printing the legend and name columns' names.
    std::cout <<
              "\n--- Dashboard ---" <<
              std::endl <<
              "Legend: " <<
              std::endl <<
              BHGRN << "\u25CF" << reset << " = success" <<
              std::endl <<
              BHRED << "\u25CF" << reset << " = failure" <<
              std::endl;
    std::cout << std::string(nameColumnWidth + 2 * FIXED_COL_WIDTH + 4, '_') << std::endl;
    std::cout << "| TA name    " + std::string(nameColumnWidth - FIXED_COL_WIDTH, ' ') + "| Conversion | Emptiness  |" << std::endl;

    // For each entry we print the name and, based on the boolean parameters, the wanted results.
    for (auto &d_entry: dashboardResults) {
        std::cout << "| " << d_entry.nameTA <<
                  std::string(nameColumnWidth - d_entry.nameTA.length() - 1, ' ') <<
                  "| " << std::string(FIXED_COL_WIDTH - 3, ' ') <<
                  (onlyEmptiness ? " " : (d_entry.translationResult ? BHGRN "\u25CF" reset : BHRED "\u25CF" reset)) <<
                  " |" << std::string(FIXED_COL_WIDTH - 2, ' ') <<
                  (onlyConversion ? " " : (d_entry.emptinessResult ? BHGRN "\u25CF" reset : BHRED "\u25CF" reset)) <<
                  " |" << std::endl;
    }
    for (int i = 0; i < nameColumnWidth + 2 * FIXED_COL_WIDTH + 4; i++) std::cout << "\u203E";
    std::cout << std::endl;
}