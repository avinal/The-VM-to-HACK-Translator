#include "code_writer.hpp"
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

int main(int argc, char const *argv[])
{
    lots_of_files files;
    std::string input(argv[1]);
    if (fs::is_directory(input))
    {
        files.push_back(input);
        for (auto &entry : fs::directory_iterator(input))
        {
            if (entry.path().extension() == ".vm")
            {
                std::string vmfile = entry.path().string();
                if (entry.path().filename() == "Sys.vm")
                {
                    files.insert(files.begin() + 1, vmfile);
                }
                else
                {
                    files.push_back(vmfile);
                }
            }
        }
    }
    else
    {
        files.push_back(input);
    }

    code_writer toassembly(files);
    toassembly.write_assembly();
    return 0;
}
