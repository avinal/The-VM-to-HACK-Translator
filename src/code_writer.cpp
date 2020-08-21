#include "../include/code_writer.hpp"

code_writer::code_writer(lots_of_files names)
{
    this->files = names;
    std::string outfile_name;
    outfile_name = files.front();
    if (files.size() > 1)
    {
        files.erase(files.begin());
        std::string check = files.front();
        if (check.substr(check.length() - 6) == "Sys.vm")
        {
            this->is_dir = true;
        }
        outfile_name = outfile_name + "/" + outfile_name;
    }
    else
    {
        outfile_name.erase(outfile_name.find('.'));
    }

    outfile_name.append(".asm");
    outfile.open(outfile_name);
}

void code_writer::write_assembly()
{
    std::unordered_map<c_type,
                       std::function<void(std::string, int, bool)>>
        write_map({{C_ARITHMETIC,
                    [this](std::string arg1, int arg2, bool arg3) {
                        this->write_arithmetic(arg1, arg2, arg3);
                    }},
                   {C_PUSH,
                    [this](std::string arg1, int arg2, bool arg3) {
                        this->write_push(arg1, arg2, arg3);
                    }},
                   {C_POP,
                    [this](std::string arg1, int arg2, bool arg3) {
                        this->write_pop(arg1, arg2, arg3);
                    }},
                   {C_LABEL,
                    [this](std::string arg1, int arg2, bool arg3) {
                        this->write_label(arg1, arg2, arg3);
                    }},
                   {C_FUNCTION,
                    [this](std::string arg1, int arg2, bool arg3) {
                        this->write_function(arg1, arg2, arg3);
                    }},
                   {C_RETURN,
                    [this](std::string arg1, int arg2, bool arg3) {
                        this->write_return(arg3);
                    }},
                   {C_GOTO,
                    [this](std::string arg1, int arg2, bool arg3) {
                        this->write_goto(arg1, arg2, arg3);
                    }},
                   {C_IF,
                    [this](std::string arg1, int arg2, bool arg3) {
                        this->write_if(arg1, arg2, arg3);
                    }},
                   {C_CALL,
                    [this](std::string arg1, int arg2, bool arg3) {
                        this->write_call(arg1, arg2, arg3);
                    }}});

    parser lets_parse;
    this->write_init(this->dbg);
    for (auto curr_file : this->files)
    {
        lets_parse.change_file(curr_file);
        lets_parse.parse();
        this->commands = lets_parse.get_commands();
        this->current_file = curr_file;
        for (auto curr : commands)
        {
            c_type write_type;
            std::string instruction;
            int index;
            std::tie(write_type, instruction, index) = curr;
            if (outfile.is_open())
            {
                write_map[write_type](instruction, index, this->dbg);
            }
        }
        this->commands.clear();
    }
    outfile.close();
}

void code_writer::write_push(std::string segment, int index, bool debug)
{
    std::string toname = this->current_file;
    toname = toname.substr(toname.find_last_of('/') + 1);
    toname = toname.erase(toname.find('.'));

    if (debug)
    {
        outfile << "// push " << segment << " " << index << " " << this->ins_no << std::endl;
    }

    if (segment == "constant")
    {
        outfile << "@" << index << std::endl
                << "D=A" << std::endl
                << "@SP" << std::endl
                << "AM=M+1" << std::endl
                << "A=A-1" << std::endl
                << "M=D" << std::endl;
        this->ins_no += 6;
    }
    else if (segment == "static")
    {
        outfile << "@" << toname << "." << index << std::endl
                << "D=M" << std::endl
                << "@SP" << std::endl
                << "AM=M+1" << std::endl
                << "A=A-1" << std::endl
                << "M=D" << std::endl;
        this->ins_no += 6;
    }
    else if (segment == "pointer")
    {
        std::string which = (index == 0) ? "THIS" : "THAT";
        outfile << "@" << which << std::endl
                << "D=M" << std::endl
                << "@SP" << std::endl
                << "AM=M+1" << std::endl
                << "A=A-1" << std::endl
                << "M=D" << std::endl;
        this->ins_no += 6;
    }
    else if (segment == "temp")
    {
        outfile << "@" << index + 5 << std::endl
                << "D=M" << std::endl
                << "@SP" << std::endl
                << "AM=M+1" << std::endl
                << "A=A-1" << std::endl
                << "M=D" << std::endl;
        this->ins_no += 6;
    }
    else
    {
        std::string which;
        if (segment == "local")
        {
            which = "LCL";
        }
        else if (segment == "argument")
        {
            which = "ARG";
        }
        else if (segment == "this")
        {
            which = "THIS";
        }
        else if (segment == "that")
        {
            which = "THAT";
        }
        outfile << "@" << which << std::endl
                << "D=M" << std::endl
                << "@" << index << std::endl
                << "D=D+A" << std::endl
                << "A=D" << std::endl
                << "D=M" << std::endl
                << "@SP" << std::endl
                << "AM=M+1" << std::endl
                << "A=A-1" << std::endl
                << "M=D" << std::endl;
        this->ins_no += 10;
    }
}

void code_writer::write_push(std::string segment, bool debug)
{
    if (debug)
    {
        outfile << "// push " << segment << " " << this->ins_no << std::endl;
    }
    outfile << "@" << segment << std::endl
            << "D=M" << std::endl
            << "@SP" << std::endl
            << "AM=M+1" << std::endl
            << "A=A-1" << std::endl
            << "M=D" << std::endl;
    this->ins_no += 6;
}

void code_writer::write_pop(std::string segment, int index, bool debug)
{
    std::string toname = this->current_file;
    toname = toname.substr(toname.find_last_of('/') + 1);
    toname = toname.erase(toname.find('.'));

    if (debug && segment != "constant")
    {
        outfile << "// pop " << segment << " " << index << " " << this->ins_no << std::endl;
    }

    if (segment == "static")
    {
        outfile << "@SP" << std::endl
                << "AM=M-1" << std::endl
                << "D=M" << std::endl
                << "@" << toname << "." << index << std::endl
                << "M=D" << std::endl;
        this->ins_no += 5;
    }
    else if (segment == "pointer")
    {
        std::string which = (index == 0) ? "THIS" : "THAT";
        outfile << "@SP" << std::endl
                << "AM=M-1" << std::endl
                << "D=M" << std::endl
                << "@" << which << std::endl
                << "M=D" << std::endl;
        this->ins_no += 5;
    }
    else if (segment == "temp")
    {
        outfile << "@SP" << std::endl
                << "AM=M-1" << std::endl
                << "D=M" << std::endl
                << "@" << index + 5 << std::endl
                << "M=D" << std::endl;
        this->ins_no += 5;
    }
    else
    {
        std::string which;
        if (segment == "local")
        {
            which = "LCL";
        }
        else if (segment == "argument")
        {
            which = "ARG";
        }
        else if (segment == "this")
        {
            which = "THIS";
        }
        else if (segment == "that")
        {
            which = "THAT";
        }
        outfile << "@" << which << std::endl
                << "D=M" << std::endl
                << "@" << index << std::endl
                << "D=D+A" << std::endl
                << "@R13" << std::endl
                << "M=D" << std::endl
                << "@SP" << std::endl
                << "AM=M-1" << std::endl
                << "D=M" << std::endl
                << "@R13" << std::endl
                << "A=M" << std::endl
                << "M=D" << std::endl;
        this->ins_no += 12;
    }
}

void code_writer::write_pop(std::string segment, bool debug)
{
    if (debug)
    {
        outfile << "// pop " << segment << " " << this->ins_no << std::endl;
    }

    outfile << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "D=M" << std::endl
            << "@" << segment << std::endl
            << "A=M" << std::endl
            << "M=D" << std::endl;
    this->ins_no += 6;
}

void code_writer::write_arithmetic(std::string comm, int index, bool debug)
{
    std::unordered_map<std::string,
                       std::function<void(std::string, bool)>>
        arith_mapic({
            {"add",
             [this](std::string arg0, bool arg1) { this->write_add_sub_and_or(arg0, arg1); }},
            {"sub",
             [this](std::string arg0, bool arg1) { this->write_add_sub_and_or(arg0, arg1); }},
            {"and",
             [this](std::string arg0, bool arg1) { this->write_add_sub_and_or(arg0, arg1); }},
            {"or",
             [this](std::string arg0, bool arg1) { this->write_add_sub_and_or(arg0, arg1); }},
            {"neg",
             [this](std::string arg0, bool arg1) { this->write_neg_not(arg0, arg1); }},
            {"not",
             [this](std::string arg0, bool arg1) { this->write_neg_not(arg0, arg1); }},
            {"gt",
             [this](std::string arg0, bool arg1) { this->write_eq_lt_gt(arg0, arg1); }},
            {"lt",
             [this](std::string arg0, bool arg1) { this->write_eq_lt_gt(arg0, arg1); }},
            {"eq",
             [this](std::string arg0, bool arg1) { this->write_eq_lt_gt(arg0, arg1); }},
        });

    arith_mapic[comm](comm, debug);
}

void code_writer::write_add_sub_and_or(std::string op, bool debug)
{
    if (debug)
    {
        outfile << "// " << op << " " << this->ins_no << std::endl;
    }
    outfile << "@SP" << std::endl
            << "AM=M-1" << std::endl
            << "D=M" << std::endl
            << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=" << operation_map.at(op) << std::endl;
    this->ins_no += 6;
}

void code_writer::write_neg_not(std::string op, bool debug)
{
    if (debug)
    {
        outfile << "// " << op << " " << this->ins_no << std::endl;
    }
    outfile << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=" << operation_map.at(op) << std::endl;
    this->ins_no += 3;
}

void code_writer::write_eq_lt_gt(std::string op, bool debug)
{
    if (debug)
    {
        outfile << "// " << op << " " << this->ins_no << std::endl;
    }
    outfile << "@SP" << std::endl
            << "AM=M-1" << std::endl
            << "D=M" << std::endl
            << "A=A-1" << std::endl
            << "D=M-D" << std::endl;
    this->ins_no += 5;
    outfile << "@" << this->ins_no + 6 << std::endl
            << "D;" << operation_map.at(op) << std::endl
            << "D=0" << std::endl;
    this->ins_no += 3;
    outfile << "@" << this->ins_no + 4 << std::endl
            << "0;JMP" << std::endl
            << "D=-1" << std::endl
            << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=D" << std::endl;
    this->ins_no += 6;
}

/*
void code_writer::write_add(bool debug)
{
    if (debug)
    {
        outfile << "// add" << std::endl;
    }
    outfile << "@SP" << std::endl
            << "AM=M-1" << std::endl
            << "D=M" << std::endl
            << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=D+M" << std::endl;
    this->ins_no += 6;
}

void code_writer::write_sub(bool debug)
{
    if (debug)
    {
        outfile << "// sub" << std::endl;
    }
    outfile << "@SP" << std::endl
            << "AM=M-1" << std::endl
            << "D=M" << std::endl
            << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=M-D" << std::endl;
    this->ins_no += 6;
}

void code_writer::write_neg(bool debug)
{
    if (debug)
    {
        outfile << "// neg" << std::endl;
    }
    outfile << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=-M" << std::endl;
    this->ins_no += 3;
}

void code_writer::write_and(bool debug)
{
    if (debug)
    {
        outfile << "// and" << std::endl;
    }
    outfile << "@SP" << std::endl
            << "AM=M-1" << std::endl
            << "D=M" << std::endl
            << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=D&M" << std::endl;
    this->ins_no += 6;
}

void code_writer::write_or(bool debug)
{
    if (debug)
    {
        outfile << "// or" << std::endl;
    }
    outfile << "@SP" << std::endl
            << "AM=M-1" << std::endl
            << "D=M" << std::endl
            << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=D|M" << std::endl;
    this->ins_no += 6;
}

void code_writer::write_not(bool debug)
{
    if (debug)
    {
        outfile << "// not" << std::endl;
    }
    outfile << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=!M" << std::endl;
    this->ins_no += 3;
}

void code_writer::write_eq(bool debug)
{
    if (debug)
    {
        outfile << "// eq" << std::endl;
    }
    outfile << "@SP" << std::endl
            << "AM=M-1" << std::endl
            << "D=M" << std::endl
            << "A=A-1" << std::endl
            << "D=M-D" << std::endl;
    this->ins_no += 5;
    outfile << "@" << this->ins_no + 6 << std::endl
            << "D;JEQ" << std::endl
            << "D=0" << std::endl;
    this->ins_no += 3;
    outfile << "@" << this->ins_no + 4 << std::endl
            << "0;JMP" << std::endl
            << "D=-1" << std::endl
            << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=D" << std::endl;
    this->ins_no += 6;
}

void code_writer::write_lt(bool debug)
{
    if (debug)
    {
        outfile << "// lt" << std::endl;
    }
    outfile << "@SP" << std::endl
            << "AM=M-1" << std::endl
            << "D=M" << std::endl
            << "A=A-1" << std::endl
            << "D=M-D" << std::endl;
    this->ins_no += 5;
    outfile << "@" << this->ins_no + 6 << std::endl
            << "D;JLT" << std::endl
            << "D=0" << std::endl;
    this->ins_no += 3;
    outfile << "@" << this->ins_no + 4 << std::endl
            << "0;JMP" << std::endl
            << "D=-1" << std::endl
            << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=D" << std::endl;
    this->ins_no += 6;
}

void code_writer::write_gt(bool debug)
{
    if (debug)
    {
        outfile << "// gt" << std::endl;
    }
    outfile << "@SP" << std::endl
            << "AM=M-1" << std::endl
            << "D=M" << std::endl
            << "A=A-1" << std::endl
            << "D=M-D" << std::endl;
    this->ins_no += 5;
    outfile << "@" << this->ins_no + 6 << std::endl
            << "D;JGT" << std::endl
            << "D=0" << std::endl;
    this->ins_no += 3;
    outfile << "@" << this->ins_no + 4 << std::endl
            << "0;JMP" << std::endl
            << "D=-1" << std::endl
            << "@SP" << std::endl
            << "A=M-1" << std::endl
            << "M=D" << std::endl;
    this->ins_no += 6;
}
*/

void code_writer::write_init(bool debug)
{
    if (debug)
    {
        outfile << "// bootstrap code"
                << " " << this->ins_no << std::endl;
    }
    outfile << "@256" << std::endl
            << "D=A" << std::endl
            << "@SP" << std::endl
            << "M=D" << std::endl;
    this->ins_no += 4;

    if (this->is_dir)
    {
        outfile << "// Sys.init defined adding function call..."
                << " " << this->ins_no << std::endl;
        this->write_call("Sys.init", 0, debug);
    }
}

void code_writer::write_call(std::string fun_name, int args, bool debug)
{
    if (debug)
    {
        outfile << "// call" << fun_name << " " << args << " " << this->ins_no << std::endl;
    }
    std::string curr_ins = std::to_string(this->ins_no);
    std::string ret_label("ret." + fun_name + "$" + curr_ins);

    outfile << "@" << ret_label << std::endl
            << "D=A" << std::endl
            << "@SP" << std::endl
            << "AM=M+1" << std::endl
            << "A=A-1" << std::endl
            << "M=D" << std::endl;
    this->ins_no += 6;

    this->write_push("LCL", debug);
    this->write_push("ARG", debug);
    this->write_push("THIS", debug);
    this->write_push("THAT", debug);

    outfile << "@SP" << std::endl
            << "D=M" << std::endl
            << "@5" << std::endl
            << "D=D-A" << std::endl
            << "@" << args << std::endl
            << "D=D-A" << std::endl
            << "@ARG" << std::endl
            << "M=D" << std::endl
            << "@SP" << std::endl
            << "D=M" << std::endl
            << "@LCL" << std::endl
            << "M=D" << std::endl
            << "@" << fun_name << std::endl
            << "0;JMP" << std::endl
            << "(" << ret_label << ")" << std::endl;
    this->ins_no += 14;
}

void code_writer::write_label(std::string label, int index, bool debug)
{
    if (debug)
    {
        outfile << "// label " << label << " " << this->ins_no << std::endl;
    }
    outfile << "(" << current_function << "$" << label << ")" << std::endl;
}

void code_writer::write_goto(std::string label, int index, bool debug)
{
    if (debug)
    {
        outfile << "// goto " << label << " " << this->ins_no << std::endl;
    }
    outfile << "@" << current_function << "$" << label << std::endl
            << "0;JMP" << std::endl;
    this->ins_no += 2;
}

void code_writer::write_function(std::string fun_name, int locals, bool debug)
{
    if (debug)
    {
        outfile << "// function " << fun_name << " " << locals << " " << this->ins_no << std::endl;
    }
    current_function = fun_name;
    outfile << "(" << fun_name << ")" << std::endl;
    for (int l = 0; l < locals; l++)
    {
        this->write_push("constant", 0, debug);
    }
}

void code_writer::write_return(bool debug)
{
    if (debug)
    {
        outfile << "// return " << current_function << " " << this->ins_no << std::endl;
    }
    outfile << "@LCL" << std::endl
            << "D=M" << std::endl
            << "@R7" << std::endl
            << "M=D" << std::endl
            << "@5" << std::endl
            << "A=D-A" << std::endl
            << "D=M" << std::endl
            << "@R14" << std::endl
            << "M=D" << std::endl;
    this->ins_no += 9;
    this->write_pop("ARG", debug);
    outfile << "@ARG" << std::endl
            << "D=M" << std::endl
            << "@SP" << std::endl
            << "M=D+1" << std::endl;
    this->ins_no += 4;
    this->write_frame_restore("THAT", 1, debug);
    this->write_frame_restore("THIS", 2, debug);
    this->write_frame_restore("ARG", 3, debug);
    this->write_frame_restore("LCL", 4, debug);
    outfile << "@R14" << std::endl
            << "A=M" << std ::endl
            << "0;JMP" << std::endl;
    this->ins_no += 3;
}

void code_writer::write_frame_restore(std::string segment, int minus, bool debug)
{
    if (debug)
    {
        outfile << "// restoring " << segment << " " << this->ins_no << std::endl;
    }

    outfile << "@R7" << std::endl
            << "D=M" << std::endl
            << "@" << minus << std::endl
            << "A=D-A" << std::endl
            << "D=M" << std::endl
            << "@" << segment << std::endl
            << "M=D" << std::endl;
    this->ins_no += 7;
}

void code_writer::write_if(std::string label, int index, bool debug)
{
    if (debug)
    {
        outfile << "// if " << label << " " << this->ins_no << std::endl;
    }
    outfile << "@SP" << std::endl
            << "AM=M-1" << std::endl
            << "D=M" << std::endl
            << "@" << current_function << "$" << label << std::endl
            << "D;JNE" << std::endl;
    this->ins_no += 5;
}
