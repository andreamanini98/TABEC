#include <iostream>
#include <sstream>
#include "xml2json.hpp"
#include <nlohmann/json.hpp>

#define STRING(x) #x
#define XSTRING(x) STRING(x)

int main() {
    std::string currentDirPath = XSTRING(SOURCE_ROOT);
    std::ifstream t(currentDirPath.append("/multiplesoftwo.xml"));
    std::stringstream buffer;
    buffer << t.rdbuf();

    const std::string tmp = buffer.str();
    const char *xml_str = tmp.c_str();

    auto j = nlohmann::json::parse(xml2json(xml_str));

    std::cout << std::setw(4) << j.at("nta").at("template").at("location") << std::endl;

    return 0;
}