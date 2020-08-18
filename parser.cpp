#include "parser.hpp"

c_type parser::command_type(std::string com)
{
    const std::unordered_map<std::string, c_type> type_map({
        {"add", C_ARITHMETIC},
        {"sub", C_ARITHMETIC},
        {"neg", C_ARITHMETIC},
        {"eq", C_ARITHMETIC},
        {"gt", C_ARITHMETIC},
        {"lt", C_ARITHMETIC},
        {"and", C_ARITHMETIC},
        {"or", C_ARITHMETIC},
        {"not", C_ARITHMETIC},
        {"push", C_PUSH},
        {"pop", C_POP},
        {"label", C_LABEL},
        {"goto", C_GOTO},
        {"function", C_FUNCTION},
        {"call", C_CALL},
        {"return", C_RETURN},
        {"if-goto", C_IF},
    });

    return type_map.at(com);
}

void parser::parse()
{
    std::ifstream infile(input_file);
    std::string vline;
    if (infile.is_open())
    {
        while (std::getline(infile, vline))
        {
            vline = vutility::trim(vline);
            if (!vline.empty())
            {
                std::vector<std::string> tokens = vutility::split(vline, ' ');
                c_type c = command_type(tokens[0]);

                switch (tokens.size())
                {
                case 1:
                {
                    if (c == C_ARITHMETIC)
                    {
                        tokens.push_back(tokens[0]);
                    }
                    else
                    {
                        tokens.push_back("null");
                    }
                    tokens.push_back("0");
                    break;
                }
                case 2:
                {
                    tokens.push_back("0");
                    break;
                }
                default:
                    break;
                }
                if (tokens.size() == 3)
                {
                    this->parsed.emplace_back(c, tokens[1], std::stoi(tokens[2]));
                }
            }
        }
    }
    infile.close();
}

std::vector<vmcommand> parser::get_commands()
{
    return this->parsed;
}