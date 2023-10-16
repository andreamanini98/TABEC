#define COLUMN_WIDTH 12

#include "Structs.h"


/**
 * Function used to print the title and summary information on the started conversion.
 * @param nameTA the name of the TA.
 * @param path the path where the source of the given TA is located.
 * @param outputFileName the name of the output file.
 * @param outputDirPath the directory in which the output file will be located.
 */
void printTitle(const std::string &nameTA, const std::string &path, const std::string &outputFileName, const std::string &outputDirPath) {
    std::cout << std::endl;
    std::cout << "-------- " << nameTA << " --------\n";
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
 */
 // TODO adjust the way the printing is done
 //      also adjust the method documentation
void printDashBoard(const std::vector<DashBoardEntry> &dashboardResults, bool onlyConversion = false, bool onlyEmptiness = false) {
    std::cout <<
              "\n--- Results of conversion ---" <<
              std::endl <<
              "Legend: " <<
              std::endl <<
              BHGRN << "\u25CF" << reset << " = success" <<
              std::endl <<
              BHRED << "\u25CF" << reset << " = failure" <<
              std::endl;
    std::cout << std::string(40, '_') << std::endl;
    std::cout << "| TA name    | Conversion | Emptiness  |" << std::endl;
    for (auto &d_entry: dashboardResults) {
        std::cout << "| " << d_entry.nameTA <<
                  std::string(COLUMN_WIDTH - d_entry.nameTA.length() - 1, ' ') <<
                  "| " << std::string(COLUMN_WIDTH - 3, ' ') <<
                  (onlyEmptiness ? " " : (d_entry.translationResult ? BHGRN "\u25CF" reset : BHRED "\u25CF" reset)) <<
                  " |" << std::string(COLUMN_WIDTH - 2, ' ') <<
                  (onlyConversion ? " " : (d_entry.emptinessResult ? BHGRN "\u25CF" reset : BHRED "\u25CF" reset)) <<
                  " |" << std::endl;
    }
    for (int i = 0; i < 40; i++) std::cout << "\u203E";
    std::cout << std::endl;
}