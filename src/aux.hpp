#pragma once

#include <string>
#include <vector>
#include <sstream>

namespace aux {
/**
 * @brief Parses argument parameters to strings. It automatically increases the argument index.
 * It also parses quotes.
 *
 * @param argc Size of argv.
 * @param argv[] Parameters, you know.
 * @param current The argument you want to parse
 * @param options Amount of predicted
 *
 * @return <b>false</b> if fails to parse. This happens usually when there are not
 * enought arguments left in argv and you try to overindex.
 */
std::wstring stringToWideString(const std::string& str);
bool parseNext(int argc, char* argv[], int& current, int option_amount, std::vector<std::string>& options);
// void clearSurroundingWhitespace(std::string& str);
} // namespace aux
