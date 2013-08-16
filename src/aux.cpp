#include "aux.hpp"

namespace aux {

std::wstring stringToWideString(const std::string& str) {
    std::wstringstream ws;
    ws << str.c_str();
    return ws.str();
}

bool parseNext(int argc, char* argv[], int& current, int option_amount, std::vector<std::string>& options) {
    ++current;
    if (current < argc) {
        std::string str = argv[current];
        options.push_back(str);
        return true;
    }
}
} // namespace aux
