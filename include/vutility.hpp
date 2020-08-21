
#include <vector>
#include <string>
#include <sstream>
class vutility
{
private:
    vutility() {}

public:
    static std::vector<std::string> split(std::string str, char delimit);
    static std::string trim(std::string str);
};
