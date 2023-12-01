#ifndef UTOTPARSER_PARSERNODE_H
#define UTOTPARSER_PARSERNODE_H

#include <stack>
#include "nlohmann/json.hpp"

using json = nlohmann::json;


struct ParserNode {
    friend std::ostream &operator<<(std::ostream &strm, const ParserNode &pn);

    int nestingLevel {};
    std::stack<json> tileStack {};
    std::stack<std::string> operatorStack {};
};


std::ostream &operator<<(std::ostream &strm, const ParserNode &pn)
{
    ParserNode pnCopy = pn;
    strm << "nestingLevel: " << pnCopy.nestingLevel
         << ", tileStackSize: " << pnCopy.tileStack.size()
         << ", operatorStackSize: " << pnCopy.operatorStack.size();

    strm << "\nOperator stack:\n";

    while (!pnCopy.operatorStack.empty())
    {
        strm << "\n" << pnCopy.operatorStack.top() << "\n";
        pnCopy.operatorStack.pop();
    }

    strm << "\nTile stack:\n";

    while (!pnCopy.tileStack.empty())
    {
        strm << "\n" << std::setw(4) << pnCopy.tileStack.top() << "\n";
        pnCopy.tileStack.pop();
    }

    return strm;
}


#endif //UTOTPARSER_PARSERNODE_H
