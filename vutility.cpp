#include "vutility.hpp"

std::vector<std::string> vutility::split(std::string str, char delimit)
{
    std::vector<std::string> tokens;
    std::stringstream tokenizer(str);
    std::string intermediate;
    while (std::getline(tokenizer, intermediate, delimit))
    {
        tokens.push_back(intermediate);
    }
    return tokens;
}

std::string vutility::trim(std::string str)
{
    if (str.substr(0, 2) == "//")
    {
        str.erase();
    }
    else
    {
        str.erase(0, str.find_first_not_of(" \r\t\v\n")); //prefixing
        str.erase(str.find_last_not_of(" \r\t\v\n") + 1); //surfixing}
    }
    return str;
}