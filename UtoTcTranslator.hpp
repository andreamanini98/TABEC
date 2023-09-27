#ifndef TA_TRANSLATOR
#define TA_TRANSLATOR

#include <iostream>
#include <sstream>
#include <fstream>
#include "xml2json.hpp"
#include <nlohmann/json.hpp>
#include <utility>
#include <string>
#include <algorithm>

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
    static void writeClocksDeclarations(std::string declaration, std::ofstream &out) {
        size_t clockPos = declaration.find("clock");
        if (clockPos != std::string::npos) {
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

            for (auto &clock: clocks)
                out << "clock:1:" << clock << std::endl;

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
    static void writeLocationsDeclarations(const std::string &processName, const std::string &initialLocation, std::vector<json> locations, std::ofstream &out) {
        for (auto &location: locations) {
            bool putColon = false;
            std::string outString = "location:" + processName + ":" + static_cast<std::string>(location.at("@id")) + "{";

            if (static_cast<std::string>(location.at("@id")) == initialLocation) {
                outString.append("initial:");
                putColon = true;
            }

            // Here we check if the location has some invariants.
            if (location.contains("label") && location.at("label").contains("@kind")) {
                (putColon) ? outString.append(" : ") : outString;
                std::string labelKind = static_cast<std::string>(location.at("label").at("@kind"));
                if (labelKind == "invariant") {
                    outString.append(labelKind + ": ");
                    outString.append(static_cast<std::string>(location.at("label").at("#text")));
                    putColon = true;
                }
            } else
                putColon = false;

            // We use the convention where a color in a state means that the state is final.
            // For this reason, only final states must have a color when designed in UPPAAL.
            if (location.contains("@color")) {
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
                    "edge:" + processName + ":" + static_cast<std::string>(transition.at("source").at("@ref")) +
                    ":" + static_cast<std::string>(transition.at("target").at("@ref")) + ":" + "a" + "{";

            if (transition.contains("label")) {
                json labels = transition.at("label");
                // We have to put this since, if a transition only have either
                // a guard or a reset (not both), then labels is not a vector.
                if (labels.is_array())
                    writeTransitionsDeclarations_helper(outString, labels, putColon);
                else {
                    json labelsToArray = json::array();
                    labelsToArray.push_back(labels);
                    writeTransitionsDeclarations_helper(outString, labelsToArray, putColon);
                }
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
            if (static_cast<std::string>(label.at("@kind")) == "guard") {
                (putColon) ? outString.append(" : ") : outString;
                outString.append("provided: ");
                outString.append(static_cast<std::string>(label.at("#text")));
                putColon = true;
            }
            if (static_cast<std::string>(label.at("@kind")) == "assignment") {
                (putColon) ? outString.append(" : ") : outString;
                outString.append("do: ");
                std::string resetString = static_cast<std::string>(label.at("#text"));
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
        writeClocksDeclarations(static_cast<std::string>(inFile.at("nta").at("template").at("declaration")), out);

        std::cout << "Starting event declaration\n";
        out << "event:a\n\n"; // Up to now we only use one event named a (also check in writeTransitionsDeclarations).

        std::cout << "Starting process declaration";
        out << "process:" + processName + "\n";

        // Locations declaration.
        std::cout << "Starting locations declaration\n";
        std::string initialLocation = inFile.at("nta").at("template").at("init").at("@ref");
        json locations = inFile.at("nta").at("template").at("location");
        // We have to put this since, if we have only one location, then locations is not a vector.
        if (locations.is_array())
            writeLocationsDeclarations(processName, initialLocation, locations, out);
        else {
            json locationsToArray = json::array();
            locationsToArray.push_back(locations);
            writeLocationsDeclarations(processName, initialLocation, locationsToArray, out);
        }

        //Transitions declarations.
        std::cout << "Starting transitions declaration\n";
        json transitions = inFile.at("nta").at("template").at("transition");
        // We have to put this since, if we have only one transition, then transitions is not a vector.
        if (transitions.is_array())
            writeTransitionsDeclarations(processName, transitions, out);
        else {
            json transitionsToArray = json::array();
            transitionsToArray.push_back(transitions);
            writeTransitionsDeclarations(processName, transitionsToArray, out);
        }

        out.close();
    }

};

#endif