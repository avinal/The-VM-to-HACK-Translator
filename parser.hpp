#pragma once
#include <tuple>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "vutility.hpp"

enum c_type
{
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};

typedef std::tuple<c_type, std::string, int> vmcommand;

class parser
{
private:
    std::string input_file;
    std::vector<vmcommand> parsed;

public:
    bool change_file(std::string name);
    std::vector<vmcommand> get_commands();
    c_type command_type(std::string com);
    void parse();
};
