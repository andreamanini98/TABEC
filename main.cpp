#include <iostream>
#include <sstream>
#include <fstream>
#include "xml2json.hpp"
#include <nlohmann/json.hpp>

#include "UtoTcTranslator.hpp"

using json = nlohmann::json;

#define STRING(x) #x
#define XSTRING(x) STRING(x)


int main() {
    // TODO we have to take all files from a directory and translate them in another directory
    std::string currentDirPath = XSTRING(SOURCE_ROOT);
    std::ifstream t(currentDirPath + "/multiplesoftwo.xml");
    std::stringstream buffer;
    buffer << t.rdbuf();
    t.close();

    // Maybe also here const will have to be removed if we take all files from a directory.
    const std::string tmp = buffer.str();
    const char *xml_str = tmp.c_str();

    json j = nlohmann::json::parse(xml2json(xml_str));

    Translator translator(currentDirPath + "/example.txt");
    translator.translateTA("ta1", j);

    return 0;
}
