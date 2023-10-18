#ifndef UTOTPARSER_TADOTCONVERTER_HPP
#define UTOTPARSER_TADOTCONVERTER_HPP

#include "nlohmann/json.hpp"

#include "TAHeaders/TAContentExtractor.hpp"
#include "utilities/JsonHelper.hpp"

using json = nlohmann::json;


class TADotConverter {

private:
    const std::string outFilePath;

    /**
     * Method used to write the starting part of the graphviz .dot file.
     * @param systemName the name of the system.
     * @param out the stream in which to write the file.
     */
    static void writeHeading(const std::string &systemName, std::ofstream &out) {
        out << "digraph " << systemName << " {" << std::endl;
        out << "fontname=\"Helvetica,Arial,sans-serif\"" << std::endl;
        out << "node [fontname=\"Helvetica,Arial,sans-serif\"]" << std::endl;
        out << "edge [fontname=\"Helvetica,Arial,sans-serif\"]" << std::endl;
        out << "node [shape=circle, width=1, height=1];" << std::endl;
        out.flush();
    }

    /**
     * Method used to write the locations part of the graphviz .dot file.
     * @param locations a vector containing a json representation of locations.
     * @param initialLocation the name of the initial location.
     * @param out out the stream in which to write the file.
     */
    static void writeLocations(std::vector<json> locations, const std::string &initialLocation, std::ofstream &out) {
        for (auto &location: locations) {
            bool isInitial = false, isFinal = false;

            std::string locationID = static_cast<std::string>(location.at(ID));
            std::string outString = "P_" + locationID + "[";
            // This is the 'label' attribute used in a typical .dot file.
            std::string dotLabel = "label=\"" + locationID;

            // If the location is initial we mark it with a double circle.
            if (locationID == initialLocation) {
                isInitial = true;
                outString.append("shape=doublecircle");
            }

            // If the location contains an invariant, we have to add it to the node as a label info.
            if (location.contains(LABEL) && location.at(LABEL).contains(KIND))
                if (static_cast<std::string>(location.at(LABEL).at(KIND)) == INVARIANT)
                    dotLabel.append("\n" + static_cast<std::string>(location.at(LABEL).at(TEXT)));

            // As we assume that in UPPAAL a colored location is also final, also here we mark a final location with a color.
            if (location.contains(COLOR)) {
                (isInitial) ? outString.append(", ") : outString;
                outString.append("style=filled, fillcolor=\"#FF00004C\"");
                isFinal = true;
            }

            dotLabel.append("\"");
            (isInitial || isFinal) ? outString.append(", " + dotLabel) : outString.append(dotLabel);

            outString.append("]\n");
            out << outString;
        }
        out.flush();
    }


public:
    explicit TADotConverter(std::string outFilePath) : outFilePath(std::move(outFilePath)) {}

    /**
     * Method used to translate a TA from .xml format to .dot format.
     * @param systemName the name of the system to translate.
     * @param inFile a json representation of the TA.
     */
    void translateTAtoDot(const std::string &systemName, const json &inFile) {
        std::ofstream out;
        out.open(outFilePath, std::ofstream::out | std::ofstream::trunc);

        std::cout << "Writing heading." << std::endl;
        writeHeading(systemName, out);

        std::cout << "Writing locations." << std::endl;
        json locations = TAContentExtractor::getLocations(inFile);
        writeLocations(getJsonAsArray(locations), TAContentExtractor::getInitialLocationName(inFile), out);

        // TODO Write transitions

        out << "}" << std::endl;
        out.flush();

        out.close();
    }

};

#endif //UTOTPARSER_TADOTCONVERTER_HPP
