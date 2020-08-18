
#include "parser.hpp"
#include <functional>
class code_writer
{
private:
    std::string file_name;
    std::vector<vmcommand> commands;
    std::ofstream outfile;
    int ins_no = -1;
    bool dbg = false;

public:
    code_writer(std::string name);
    void write_assembly();
    void write_push(std::string segment, int index, bool debug);
    void write_pop(std::string segment, int index, bool debug);
    void write_arithmetic(std::string comm, int index, bool debug);
    void write_add(bool debug);
    void write_sub(bool debug);
    void write_neg(bool debug);
    void write_and(bool debug);
    void write_or(bool debug);
    void write_not(bool debug);
    void write_eq(bool debug);
    void write_lt(bool debug);
    void write_gt(bool debug);
};
