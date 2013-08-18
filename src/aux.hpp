#pragma once

#include <string>
#include <deque>
#include <sstream>

namespace Color {
static const int DEFAULT = 225;
static const int TRANSPARENT = -1; // only for background
static const int BLACK = 0;
static const int RED = 1;
static const int GREEN = 2;
static const int YELLOW = 3;
static const int BLUE = 4;
static const int MAGENTA = 5;
static const int CYAN = 6;
static const int WHITE = 7;
static const int BRIGHT_BLACK = 0 + 8;
static const int BRIGHT_RED = 1 + 8;
static const int BRIGHT_GREEN = 2 + 8;
static const int BRIGHT_YELLOW = 3 + 8;
static const int BRIGHT_BLUE = 4 + 8;
static const int BRIGHT_MAGENTA = 5 + 8;
static const int BRIGHT_CYAN = 6 + 8;
static const int BRIGHT_WHITE = 7 + 8;
static const int DEFAULT_OFFSET = 226;
static const int COLOR_MODIFIER = 15;
}

namespace aux {
template <class T>
std::wstring toWideString(T value) {
    std::wstringstream ws;
    ws << value;
    return ws.str();
}

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

bool parseNext(int argc, char* argv[], int& current, int option_amount, std::deque<std::string>& options);

void attrReset();

void attrBold();

void attrUnderline();

void attrBlink();

int getCols();

int getRows();

void setColor(int fg, int bg);

// This is just a test
unsigned distance(const std::wstring& input, const std::wstring& tcmp);
// void clearSurroundingWhitespace(std::string& str);
} // namespace aux
