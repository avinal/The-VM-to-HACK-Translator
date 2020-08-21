# The-VM-to-HACK-Translator
Translates Virtual Machine Language into HACK Assembly language.

**Important: If you are taking this course then I suggest you to try to write the assembler yourself first, it is not very easy and not very tough too. At least not impossible, because I did it 😘**

## Get Started with the project
1. Directory Structure
    ```shell
        .
        |_include      # Contains headers
        |_src          # Contains Source Code and Main
        |_Test-Files   # Example HACK VM Files
    ```
2. Clone this repo
    ```shell
        git clone https://github.com/avinal/The-VM-to-HACK-Translator.git
    ```
3. Build and run, provide only [HACK VM Language](https://www.nand2tetris.org/project08) file. Don't confuse with `.vm` extesion. 
    ```shell
        cd The-VM-to-HACK-Translator
        make
        ./VMTranslator file.vm
    ```
4. Start over after modification
    ```shell
        make clean
    ```


## Tips
In case you want to modify this project, you can use [Boost C++ Libraries](https://www.boost.org/), they can simplify many tasks needed by this project. 

## Some Words
This was a nice project and taugh me many things. Given below are some miraculous code snippets that proved to be really useful and are well researched. Hope you will find them useful.

1. Trim spaces, comments, carrige returns, newline, tab and verticle tabs:
    ```cpp
        inline std::string trim(std::string str)
        {
            if (str.find('/') != std::string::npos) // comments
            {
                str.erase(str.find('/'));
            }
            str.erase(0, str.find_first_not_of(" \r\t\v\n")); //prefixing
            str.erase(str.find_last_not_of(" \r\t\v\n") + 1); //surfixing}
            return str;
        }
    ```
2. Tokenize a string using C++ with respect to a delimiter, space by defalt (string split):
    ```cpp
        #include <sstream>
        #include <vector>
        std::vector<std::string> split(std::string str, char delimiter = ' ')
        {
            std::vector<std::string> tokens;
            std::stringstream tokenizer(str);
            std::string intermediate;
            while (std::getline(tokenizer, intermediate, delimiter))
            {
                tokens.push_back(intermediate);
            }
            return tokens;
        }
    ```
3. **Fascinating** Using Lambdas with Map to implement a function or function pointers. [See here](https://github.com/avinal/The-VM-to-HACK-Translator/blob/291b239065fc5cc0921b8592dfcd69c6f3022e52/src/code_writer.cpp#L29) 