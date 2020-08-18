#include "code_writer.hpp"

code_writer::code_writer(std::string name)
{
    file_name = name;
    name.erase(name.find('.'));
    name.append(".asm");
    outfile.open(name);
}

void code_writer::write_assembly()
{
    parser toparse(file_name);
    toparse.parse();
    commands = toparse.get_commands();

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
                    }}});

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
    outfile.close();
}

void code_writer::write_push(std::string segment, int index, bool debug)
{
    std::string toname = file_name;
    toname = toname.substr(toname.find_last_of('/') + 1);
    toname = toname.erase(toname.find('.'));

    if (debug)
    {
        outfile << "// push " << segment << " " << index << std::endl;
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

void code_writer::write_pop(std::string segment, int index, bool debug)
{
    std::string toname = file_name;
    toname = toname.substr(toname.find_last_of('/')+1);
    toname = toname.erase(toname.find('.'));

    if (debug && segment != "constant")
    {
        outfile << "// pop " << segment << " " << index << std::endl;
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
        this->ins_no += 4;
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

void code_writer::write_arithmetic(std::string comm, int index, bool debug)
{
    std::unordered_map<std::string,
                       std::function<void(bool)>>
        arith_mapic({
            {"add",
             [this](bool arg1) { this->write_add(arg1); }},
            {"sub",
             [this](bool arg1) { this->write_sub(arg1); }},
            {"neg",
             [this](bool arg1) { this->write_neg(arg1); }},
            {"and",
             [this](bool arg1) { this->write_and(arg1); }},
            {"or",
             [this](bool arg1) { this->write_or(arg1); }},
            {"not",
             [this](bool arg1) { this->write_not(arg1); }},
            {"gt",
             [this](bool arg1) { this->write_gt(arg1); }},
            {"lt",
             [this](bool arg1) { this->write_lt(arg1); }},
            {"eq",
             [this](bool arg1) { this->write_eq(arg1); }},
        });

    arith_mapic[comm](debug);
}

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