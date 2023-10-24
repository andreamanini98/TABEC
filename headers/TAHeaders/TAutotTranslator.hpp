#ifndef UTOTPARSER_TAUTOTTRANSLATOR_HPP
#define UTOTPARSER_TAUTOTTRANSLATOR_HPP

#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>
#include <string>
#include <algorithm>
#include <cmath>
#include "nlohmann/json.hpp"

#include "XMLtoJSONInclude/xml2json.hpp"
#include "utilities/JsonHelper.hpp"
#include "defines/UPPAALxmlAttributes.h"
#include "TAContentExtractor.hpp"
#include "utilities/Utils.hpp"

using json = nlohmann::json;


class Translator {

private:
    const std::string outFilePath;
    // The number of states of the TA.
    int Q;
    // The maximum value appearing in TA's guards and invariants.
    int C;
    // A clock used to make the time increase strictly monotonically.
    // The behaviour of this clock is as follows:
    // - It is reset in input to a location.
    // - It is checked as a guard (> 0) in output to a location.
    std::string fictitiousClock;

    /**
     * Method used to write the clocks declarations in tChecker syntax.
     * Up to now, we consider only single clocks (not arrays of clocks).
     * @param declaration the string coming from the UPPAAL .xml file containing the list of clocks.
     * @param out the stream where we write our output file.
     */
    void writeClocksDeclarations(const std::string &declaration, std::ofstream &out) {
        if (declaration.find(CLOCK) != std::string::npos) {
            std::vector<std::string> clocks = TAContentExtractor::getClocks(declaration);

            for (auto &clock: clocks) {
                out << "clock:1:" << clock << std::endl;
                // The fictitious clock's name is obtained by concatenation of the other clocks in order to avoid name clashes.
                fictitiousClock.append(clock);
            }
            out << "clock:1:" << fictitiousClock << std::endl;

            out << "\n";
            out.flush();
        }
    }

    /**
    * Method used to write the locations declarations in tChecker syntax.
    * @param processName the name of the process (up to now we only assume one process).
    * @param initialLocation the name of the TA's initial location.
    * @param locations a vector of locations saved in json format.
    * @param out the stream where we write our output file.
    */
    void writeLocationsDeclarations(const std::string &processName, const std::string &initialLocation, std::vector<json> locations, std::ofstream &out) {
        // We get the number of states.
        Q = static_cast<int>(locations.size());

        for (auto &location: locations) {
            bool isInitial = false, hasInvariant = false;
            std::string outString = "location:" + processName + ":" + static_cast<std::string>(location.at(ID)) + "{";

            if (static_cast<std::string>(location.at(ID)) == initialLocation) {
                outString.append("initial:");
                isInitial = true;
            }

            // Here we check if the location has some invariants.
            if (location.contains(LABEL) && location.at(LABEL).contains(KIND)) {
                (isInitial) ? outString.append(" : ") : outString;
                std::string labelKind = static_cast<std::string>(location.at(LABEL).at(KIND));
                if (labelKind == INVARIANT) {
                    std::string labelText = static_cast<std::string>(location.at(LABEL).at(TEXT));
                    // We have to check if C will eventually be updated due to the invariant's constant.
                    int newMax = getMaxIntFromStr(labelText);
                    C = (C < newMax) ? newMax : C;
                    outString.append(labelKind + ": ");
                    outString.append(labelText);
                    hasInvariant = true;
                }
            }

            // We use the convention where a color in a state means that the state is final.
            // For this reason, only final states must have a color when designed in UPPAAL.
            if (location.contains(COLOR)) {
                (isInitial || hasInvariant) ? outString.append(" : ") : outString;
                outString.append("labels: final");
            }

            outString.append("}\n");
            out << outString;
        }
        out.flush();
    }

    /**
     * Method used to write the transitions declarations in tChecker syntax.
     * @param processName the name of the process (up to now we only assume one process).
     * @param transitions a vector of transitions saved in json format.
     * @param out the stream where we write our output file.
     */
    void writeTransitionsDeclarations(const std::string &processName, std::vector<json> transitions, std::ofstream &out) {
        for (auto &transition: transitions) {
            bool putColon = false;
            std::string outString =
                    "edge:" + processName + ":" + static_cast<std::string>(transition.at(SOURCE).at(REF)) +
                    ":" + static_cast<std::string>(transition.at(TARGET).at(REF)) + ":" + "a" + "{";

            if (transition.contains(LABEL)) {
                json labels = transition.at(LABEL);
                writeTransitionsDeclarations_helper(outString, getJsonAsArray(labels), putColon);
            } else {
                // If no guards or assignments are present, we still have to preserve the strictly monotonic time.
                outString.append("provided: " + fictitiousClock + " > 0 : do: " + fictitiousClock + " = 0");
            }

            outString.append("}\n");
            out << outString;
        }
        out.flush();
    }

    /**
     * Helper method used inside writeTransitionsDeclarations().
     * @param outString the string that will be written in the output file.
     * @param labels a vector containing all the labels of a transition (if any).
     * @param putColon a boolean used to determine if in the tChecker translation we have to put a colon.
     */
    void writeTransitionsDeclarations_helper(std::string &outString, std::vector<json> labels, bool &putColon) {
        for (auto &label: labels) {
            std::string labelText = static_cast<std::string>(label.at(TEXT));
            std::string labelKind = static_cast<std::string>(label.at(KIND));

            if (labelKind == GUARD) {
                // We have to check if C will eventually be updated due to the transition's guard.
                int newMax = getMaxIntFromStr(labelText);
                C = (C < newMax) ? newMax : C;
                (putColon) ? outString.append(" : ") : outString;
                outString.append("provided: ");
                // We add the condition 'fictitiousClock > 0' to ensure the transition takes some time to fire.
                outString.append(labelText + " && " + fictitiousClock + " > 0");
                putColon = true;
            }
            if (labelKind == ASSIGNMENT) {
                (putColon) ? outString.append(" : ") : outString;
                outString.append("do: ");
                std::replace(labelText.begin(), labelText.end(), ',', ';');
                // We add the condition 'fictitiousClock = 0' to ensure the next transition will take some time to fire.
                outString.append(labelText + "; " + fictitiousClock + " = 0");
                putColon = true;
            }
        }
    }


public:
    explicit Translator(std::string outFilePath, int C = 0, int Q = 0) : outFilePath(std::move(outFilePath)), C(C), Q(Q) {}

    /**
    * This method performs the translation from UPPAAL syntax to tChecker syntax.
    * @param systemName the name of the system to translate.
    * @param inFile the json file containing the UPPAAL representation to convert.
    */
    void translateTA(const std::string &systemName, const json &inFile) {
        std::ofstream out;
        out.open(outFilePath, std::ofstream::out | std::ofstream::trunc);

        // In our case study we don't care about having multiple processes, so we can simply put the name we want.
        std::string processName = "P";

        std::cout << "Starting system declaration\n";
        out << "system:" + systemName + "\n\n";

        std::cout << "Starting clocks declaration\n";
        writeClocksDeclarations(TAContentExtractor::getClocksDeclaration(inFile), out);

        // Up to now we only use one event named a (also check in writeTransitionsDeclarations).
        std::cout << "Starting event declaration\n";
        out << "event:a\n\n";

        std::cout << "Starting process declaration\n";
        out << "process:" + processName + "\n";

        // Locations declaration.
        std::cout << "Starting locations declaration\n";
        std::string initialLocation = TAContentExtractor::getInitialLocationName(inFile);
        json locations = TAContentExtractor::getLocations(inFile);
        writeLocationsDeclarations(processName, initialLocation, getJsonAsArray(locations), out);

        // Transitions declarations.
        std::cout << "Starting transitions declaration\n";
        json transitions = TAContentExtractor::getTransitions(inFile);
        writeTransitionsDeclarations(processName, getJsonAsArray(transitions), out);

        // Writing additional information that will be used in the checking procedure.
        // Please note that, although tChecker accepts comments starting with #, the :: syntax has been chosen by ourselves.
        out << "\n# Q :: " << Q << std::endl;
        out << "\n# C :: " << C << std::endl;
        // Computing alpha as seen in Theorem 5.
        out << "\n# Alpha :: " << std::round(std::pow(4 * (1 + C * std::max(Q, 4 * C)), -1) * ALPHA_MAG) << std::endl;

        out.close();
    }

    /**
     * Method used to tell if a given TA in json format is nrt or not.
     * @param inFile the json representation of the TA to check.
     * @return true if the given TA is nrt, false otherwise.
     */
    static bool isNRT(json inFile) {
        bool isNRT = true;
        std::vector<json> transitions = getJsonAsArray(TAContentExtractor::getTransitions(inFile));
        std::vector<std::string> clocks = TAContentExtractor::getClocks(static_cast<std::string>(inFile.at(NTA).at(TEMPLATE).at(DECLARATION)));

        // For each transition we check if the nrt condition holds.
        for (auto &transition: transitions) {
            if (transition.contains(LABEL)) {
                std::vector<json> labels = getJsonAsArray(transition.at(LABEL));

                // We check if the transition has exactly two labels and if they correspond to an assignment and a reset.
                if (labels.size() == 2) {
                    if ((static_cast<std::string>(labels[0].at(KIND)) == GUARD &&
                         static_cast<std::string>(labels[1].at(KIND)) == ASSIGNMENT) ||
                        (static_cast<std::string>(labels[1].at(KIND)) == GUARD &&
                         static_cast<std::string>(labels[0].at(KIND)) == ASSIGNMENT)) {
                        // If it is the case, for each clock we check if it is used both in an assignment and in a reset.
                        for (auto &clock: clocks) {
                            if ((static_cast<std::string>(labels[0].at(TEXT)).find(clock) != std::string::npos) &&
                                (static_cast<std::string>(labels[1].at(TEXT)).find(clock) != std::string::npos))
                                return false;
                        }
                    }
                }
            }
        }
        return isNRT;
    }

};

#endif // UTOTPARSER_TAUTOTTRANSLATOR_HPP