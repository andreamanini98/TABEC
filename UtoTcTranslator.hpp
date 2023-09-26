#ifndef TA_TRANSLATOR
#define TA_TRANSLATOR

#include <iostream>
#include <sstream>
#include <fstream>
#include "xml2json.hpp"
#include <nlohmann/json.hpp>
#include <utility>

using json = nlohmann::json;

class Translator {

private:
    const std::string outFilePath;

    /**
    * Method used to write the locations declaration in tChecker syntax.
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
                    outString.append(labelKind + ":");
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

        out << "system:" + systemName + "\n\n";
        out << "QUI CI VANNO I CLOCK\n\n";
        out << "event:a\n\n";
        out << "process:" + processName + "\n";

        // Locations declaration
        std::string initialLocation = inFile.at("nta").at("template").at("init").at("@ref");
        std::vector<json> locations = inFile.at("nta").at("template").at("location");
        writeLocationsDeclarations(processName, initialLocation, locations, out);
        
        out.close();
    }

};

#endif