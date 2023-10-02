#define RESULTS_PADDING 10


/**
 * This function is used to print in a tabular manner the results of the conversion:
 * - Green ball: the conversion was successful.
 * - Red ball: the conversion encountered some errors and could not be finished.
 * @param translationsResults a vector of pairs, where each pair contains as the first element the name
 * of a TA, while in the second element it contains true if the conversion was successful, false otherwise.
 */
void printResultsDashboard(const std::vector<std::pair<std::string, bool>> &translationsResults) {
    std::cout <<
              "\n--- Results of conversion ---" <<
              std::endl <<
              "Legend: " <<
              std::endl <<
              BHGRN << "\u25CF" << reset << " = success" <<
              std::endl <<
              BHRED << "\u25CF" << reset << " = failure" <<
              std::endl;
    std::cout << std::string(RESULTS_PADDING + 1, '-') << std::endl;
    for (auto &result: translationsResults) {
        std::cout << result.first <<
                  std::string(RESULTS_PADDING - result.first.length(), ' ') <<
                  ((result.second) ? BHGRN "\u25CF" reset : BHRED "\u25CF" reset) << std::endl;
        std::cout << std::string(RESULTS_PADDING + 1, '-') << std::endl;
    }
}