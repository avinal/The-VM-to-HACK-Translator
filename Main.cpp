#include "code_writer.hpp"
#include <iostream>

int main(int argc, char const *argv[])
{
    code_writer toassembly(argv[1]);
    toassembly.write_assembly();
    return 0;
}
