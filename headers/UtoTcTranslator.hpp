#ifndef TA_TRANSLATOR
#define TA_TRANSLATOR

#include <iostream>
#include <sstream>
#include <fstream>
#include <utility>
#include <string>
#include <algorithm>
#include <nlohmann/json.hpp>

#include "XMLtoJSONInclude/xml2json.hpp"
#include "jsonHelper.hpp"
#include "UPPAALxmlAttributes.h"

using json = nlohmann::json;


class Translator {

private:
    const std::string outFilePath;

    /**
     * Method used to write the clocks declarations in tChecker syntax.
     * Up to now, we consider only single clocks (not arrays of clocks).
     * @param declaration the string coming from the UPPAAL .xml file containing the list of clocks.
     * @param out the stream where we write our output file.
     */
    static void writeClocksDeclarations(const std::string &declaration, std::ofstream &out) {
        if (declaration.find(CLOCK) != std::string::npos) {
            std::vector<std::string> clocks = getClocks(declaration);

            for (auto &clock: clocks)
                out << "clock:1:" << clock << std::endl;

            out << "\n";
            out.flush();
        }
    }

    /**
     * Method used to retrieve the identifiers of the clocks used in the UPPAAL TA.
     * @param declaration the string containing the clocks declaration.
     * @param clockKeywordPos the position in the declaration parameter of the keyword "clock".
     * @return a vector containing all the TA's clocks.
     */
    static std::vector<std::string> getClocks(std::string declaration) {
        size_t clockPos = declaration.find(CLOCK);

        // We keep only the portion of string after the "clock" word.
        declaration = declaration.substr(clockPos + 6);
        // We remove the trailing semicolon ';'.
        declaration.pop_back();
        // Remove all whitespaces from the string.
        declaration.erase(std::remove_if(declaration.begin(), declaration.end(), ::isspace), declaration.end());

        std::stringstream ss(declaration);
        std::vector<std::string> clocks;
        std::string s;

        // We now split the string for each encountered ',' storing the resulting token in a vector.
        while (getline(ss, s, ','))
            clocks.push_back(s);

        return clocks;
    }

    /**
    * Method used to write the locations declarations in tChecker syntax.
    * @param processName the name of the process (up to now we only assume one process).
    * @param initialLocation the name of the TA's initial location.
    * @param locations a vector of locations saved in json format.
    * @param out the stream where we write our output file.
    */
    static void writeLocationsDeclarations(const std::string &processName, const std::string &initialLocation, std::vector<json> locations, std::ofstream &out) {
        for (auto &location: locations) {
            bool putColon = false;
            std::string outString = "location:" + processName + ":" + static_cast<std::string>(location.at(ID)) + "{";

            if (static_cast<std::string>(location.at(ID)) == initialLocation) {
                outString.append("initial:");
                putColon = true;
            }

            // Here we check if the location has some invariants.
            if (location.contains(LABEL) && location.at(LABEL).contains(KIND)) {
                (putColon) ? outString.append(" : ") : outString;
                std::string labelKind = static_cast<std::string>(location.at(LABEL).at(KIND));
                if (labelKind == INVARIANT) {
                    outString.append(labelKind + ": ");
                    outString.append(static_cast<std::string>(location.at(LABEL).at(TEXT)));
                    putColon = true;
                }
            } else
                putColon = false;

            // We use the convention where a color in a state means that the state is final.
            // For this reason, only final states must have a color when designed in UPPAAL.
            if (location.contains(COLOR)) {
                (putColon) ? outString.append(" : ") : outString;
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
    static void writeTransitionsDeclarations(const std::string &processName, std::vector<json> transitions, std::ofstream &out) {
        for (auto &transition: transitions) {
            bool putColon = false;
            std::string outString =
                    "edge:" + processName + ":" + static_cast<std::string>(transition.at(SOURCE).at(REF)) +
                    ":" + static_cast<std::string>(transition.at(TARGET).at(REF)) + ":" + "a" + "{";

            if (transition.contains(LABEL)) {
                json labels = transition.at(LABEL);
                writeTransitionsDeclarations_helper(outString, getJsonAsArray(labels), putColon);
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
    static void writeTransitionsDeclarations_helper(std::string &outString, std::vector<json> labels, bool &putColon) {
        for (auto &label: labels) {
            if (static_cast<std::string>(label.at(KIND)) == GUARD) {
                (putColon) ? outString.append(" : ") : outString;
                outString.append("provided: ");
                outString.append(static_cast<std::string>(label.at(TEXT)));
                putColon = true;
            }
            if (static_cast<std::string>(label.at(KIND)) == ASSIGNMENT) {
                (putColon) ? outString.append(" : ") : outString;
                outString.append("do: ");
                std::string resetString = static_cast<std::string>(label.at(TEXT));
                std::replace(resetString.begin(), resetString.end(), ',', ';');
                outString.append(resetString);
                putColon = true;
            }
        }
    }


public:
    explicit Translator(std::string outFilePath) : outFilePath(std::move(outFilePath)) {}

    /**
    * This method performs the translation from UPPAAL syntax to tChecker syntax.
    * @param systemName the name of the system to translate.
    * @param inFile the json file containing the UPPAAL representation to convert.
    */
    void translateTA(const std::string &systemName, json inFile) {
        std::ofstream out;
        out.open(outFilePath, std::ofstream::out | std::ofstream::trunc);

        // In our case study we don't care about having multiple processes, so we can simply put the name we want.
        std::string processName = "P";

        std::cout << "Starting system declaration\n";
        out << "system:" + systemName + "\n\n";

        std::cout << "Starting clocks declaration\n";
        writeClocksDeclarations(static_cast<std::string>(inFile.at(NTA).at(TEMPLATE).at(DECLARATION)), out);

        std::cout << "Starting event declaration\n";
        out << "event:a\n\n"; // Up to now we only use one event named a (also check in writeTransitionsDeclarations).

        std::cout << "Starting process declaration";
        out << "process:" + processName + "\n";

        // Locations declaration.
        std::cout << "Starting locations declaration\n";
        std::string initialLocation = inFile.at(NTA).at(TEMPLATE).at(INIT).at(REF);
        json locations = inFile.at(NTA).at(TEMPLATE).at(LOCATION);
        writeLocationsDeclarations(processName, initialLocation, getJsonAsArray(locations), out);

        //Transitions declarations.
        std::cout << "Starting transitions declaration\n";
        json transitions = inFile.at(NTA).at(TEMPLATE).at(TRANSITION);
        writeTransitionsDeclarations(processName, getJsonAsArray(transitions), out);

        out.close();
    }

    /**
     * Method used to tell if a given TA in json format is nrt or not.
     * @param inFile the json representation of the TA to check.
     * @return true if the given TA is nrt, false otherwise.
     */
    static bool isNRT(json inFile) {
        bool isNRT = true;
        std::vector<json> transitions = getJsonAsArray(inFile.at(NTA).at(TEMPLATE).at(TRANSITION));
        std::vector<std::string> clocks = getClocks(static_cast<std::string>(inFile.at(NTA).at(TEMPLATE).at(DECLARATION)));

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

#endif