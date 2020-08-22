#include "parser.hpp"
#include <functional>
#include <string>

typedef std::vector<std::string> lots_of_files;

const std::unordered_map<std::string, std::string> operation_map({{"add", "M+D"},
                                                                  {"sub", "M-D"},
                                                                  {"neg", "-M"},
                                                                  {"and", "D&M"},
                                                                  {"or", "D|M"},
                                                                  {"not", "!M"},
                                                                  {"eq", "JEQ"},
                                                                  {"lt", "JLT"},
                                                                  {"gt", "LGT"}});

class code_writer
{
private:
    lots_of_files files;
    std::vector<vmcommand> commands;
    std::ofstream outfile;
    std::string current_file = "";
    std::string current_function = "";
    int ins_no = -1;
    bool dbg = true;
    bool is_dir = false;

public:
    code_writer(lots_of_files name);
    void write_assembly();

    void write_push(std::string segment, int index, bool debug);
    void write_push(std::string segment, bool debug);
    void write_pop(std::string segment, int index, bool debug);
    void write_pop(std::string segment, bool debug);

    void write_arithmetic(std::string comm, int index, bool debug);
    void write_add_sub_and_or(std::string op, bool debug);
    void write_neg_not(std::string op, bool debug);
    void write_eq_lt_gt(std::string op, bool debug);

    void write_init(bool debug);

    void write_label(std::string label, int index, bool debug);
    void write_goto(std::string label, int index, bool debug);
    void write_if(std::string label, int index, bool debug);

    void write_call(std::string fun_name, int args, bool debug);
    void write_return(bool debug);
    void write_function(std::string fun_name, int locals, bool debug);
    void write_frame_restore(std::string segment, int minus, bool debug);
};
