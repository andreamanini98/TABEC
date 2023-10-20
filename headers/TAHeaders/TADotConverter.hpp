#ifndef UTOTPARSER_TADOTCONVERTER_HPP
#define UTOTPARSER_TADOTCONVERTER_HPP

#include "nlohmann/json.hpp"

#include "TAHeaders/TAContentExtractor.hpp"
#include "utilities/JsonHelper.hpp"
#include "utilities/Utils.hpp"

using json = nlohmann::json;

// The following defines can be used to tune graphviz parameters
// -------------------------------------------------------------
#define FONT_NAME "Helvetica,Arial,sans-serif"
#define NODESEP "0.3"
#define NODE_SHAPE "circle"
#define NODE_WIDTH "0.5"
#define NODE_HEIGHT "0.5"
#define INIT_NODE_SHAPE "shape=doublecircle"
#define FINAL_NODE_STYLE "style=filled, fillcolor=\"#FF00004C\""
#define NODE_LABEL_COLOR "BLACK"
#define GUARD_COLOR "GREEN2"
#define ASSIGNMENT_COLOR "PURPLE"
#define LOCATION_FONT_SIZE "8pt"
#define EDGE_FONT_SIZE "8pt"
#define ARROW_SIZE "0.5"


class TADotConverter {

private:
    const std::string outFilePath;

    /**
     * Method use to escape html characters since we're using a graphviz html representation.
     * @param stringToSanitize the string to sanitize.
     * @return a string where the specified html characters are escaped.
     */
    static std::string htmlSanitize(std::string stringToSanitize) {
        // Remember to first replace the '&' since otherwise the others derived
        // from subsequent replacements will be substituted as well.
        std::vector<std::pair<char, std::string>> symbolsAndStrings = {{'&', "&amp;"},
                                                                       {'<', "&lt;"},
                                                                       {'>', "&gt;"}};
        for (auto &sAs: symbolsAndStrings)
            stringToSanitize = substituteCharInString(stringToSanitize, sAs.first, sAs.second);
        return stringToSanitize;
    }

    /**
     * Method used to write the starting part of the graphviz .dot file.
     * @param systemName the name of the system.
     * @param out the stream in which to write the file.
     */
    static void writeHeading(const std::string &systemName, std::ofstream &out) {
        out << "digraph " << systemName << " {" << std::endl;
        out << "fontname=\"" << FONT_NAME << "\"" << std::endl;
        out << "node [fontname=\"" << FONT_NAME << "\"]" << std::endl;
        out << "edge [fontname=\"" << FONT_NAME << "\"]" << std::endl;
        out << "nodesep = " << NODESEP << ";" << std::endl;
        out << "node [fixedsize=true, shape=" << NODE_SHAPE << ", width=" << NODE_WIDTH << ", height=" << NODE_HEIGHT << "];" << std::endl;
        out.flush();
    }

    /**
     * Method used to write the locations part of the graphviz .dot file.
     * @param locations a vector containing a json representation of locations.
     * @param initialLocation the name of the initial location.
     * @param out the stream in which to write the file.
     */
    static void writeLocations(std::vector<json> locations, const std::string &initialLocation, std::ofstream &out) {
        for (auto &location: locations) {
            bool isInitial = false, isFinal = false;

            std::string locationID = static_cast<std::string>(location.at(ID));
            std::string outString = "P_" + locationID + "[fontsize=\"" + std::string(LOCATION_FONT_SIZE) + "\", ";
            // This is the 'label' attribute used in a typical .dot file.
            std::string dotLabel = "label=<<FONT>" + locationID;

            // If the location is initial we mark it with a double circle.
            if (locationID == initialLocation) {
                isInitial = true;
                outString.append(INIT_NODE_SHAPE);
            }

            // If the location contains an invariant, we have to add it to the node as a label info.
            if (location.contains(LABEL) && location.at(LABEL).contains(KIND))
                if (static_cast<std::string>(location.at(LABEL).at(KIND)) == INVARIANT)
                    dotLabel.append("<BR />" + htmlSanitize(static_cast<std::string>(location.at(LABEL).at(TEXT))));

            // As we assume that in UPPAAL a colored location is also final, also here we mark a final location with a color.
            if (location.contains(COLOR)) {
                (isInitial) ? outString.append(", ") : outString;
                outString.append(FINAL_NODE_STYLE);
                isFinal = true;
            }

            dotLabel.append("</FONT>>");
            (isInitial || isFinal) ? outString.append(", " + dotLabel) : outString.append(dotLabel);

            outString.append("];\n");
            out << outString;
        }
        out.flush();
    }

    /**
     * Method used to write the transitions part of the graphviz .dot file.
     * @param transitions a vector containing a json representation of transitions.
     * @param out the stream in which to write the file.
     */
    static void writeTransitions(std::vector<json> transitions, std::ofstream &out) {
        for (auto &transition: transitions) {
            std::string outString =
                    "P_" + static_cast<std::string>(transition.at(SOURCE).at(REF)) + " -> " +
                    "P_" + static_cast<std::string>(transition.at(TARGET).at(REF)) +
                    " [arrowsize=" + std::string(ARROW_SIZE) + ", fontsize=\"" + std::string(EDGE_FONT_SIZE) + "\"";

            std::string dotLabel = "label=<<FONT> <FONT COLOR=\"" + std::string(NODE_LABEL_COLOR) + "\">a</FONT>";

            if (transition.contains(LABEL)) {
                json labels = transition.at(LABEL);
                dotLabel.append(writeTransitions_helper(getJsonAsArray(labels)));
            }

            outString.append(dotLabel + "</FONT>>];\n");
            out << outString;
        }
        out.flush();
    }

    /**
     * Helper method used to write the text inside a transition.
     * @param labels the transition's labels which content will be written in the graphviz .dot file.
     * @return a string which contains a properly formatted text according to the labels content.
     */
    static std::string writeTransitions_helper(std::vector<json> labels) {
        std::string dotLabelAddition;

        for (auto &label: labels) {
            std::string labelText = static_cast<std::string>(label.at(TEXT));
            std::string labelKind = static_cast<std::string>(label.at(KIND));

            if (labelKind == GUARD)
                dotLabelAddition.append("<BR /><FONT COLOR=\"" + std::string(GUARD_COLOR) + "\">" + htmlSanitize(labelText) + "</FONT>");
            if (labelKind == ASSIGNMENT)
                dotLabelAddition.append("<BR /><FONT COLOR=\"" + std::string(ASSIGNMENT_COLOR) + "\">" + htmlSanitize(labelText) + "</FONT>");
        }
        return dotLabelAddition;
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

        std::cout << "Writing transitions." << std::endl;
        writeTransitions(getJsonAsArray(TAContentExtractor::getTransitions(inFile)), out);

        out << "overlap=false;\n}" << std::endl;
        out.flush();

        out.close();
    }

};

#endif //UTOTPARSER_TADOTCONVERTER_HPP
