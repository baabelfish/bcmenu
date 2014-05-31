#pragma once

#include "../config.h"

#if defined(USING_NCURSESW_CURSES_H)
#include <ncursesw/curses.h>
#elif defined(USING_NCURSESW_H)
#include <ncursesw.h>
#elif defined(USING_NCURSES_CURSES_H)
#include <ncurses/curses.h>
#elif defined(USING_NCURSES_H)
#include <ncurses.h>
#else
#include <curses.h>
#endif

#include <string>
#include <deque>
#include <sstream>
#include <iostream>
#include <vector>

namespace Misc {
static const std::wstring Prompt = L":";
static const std::wstring PromptFocused = L">";
static const std::wstring PromptSelected = L"*";
} // namespace Misc

namespace Color {
const int DEFAULT = 225;
const int TRANSPARENT = -1; // only for background
const int BLACK = 0;
const int RED = 1;
const int GREEN = 2;
const int YELLOW = 3;
const int BLUE = 4;
const int MAGENTA = 5;
const int CYAN = 6;
const int WHITE = 7;
const int BRIGHT_BLACK = 0 + 8;
const int BRIGHT_RED = 1 + 8;
const int BRIGHT_GREEN = 2 + 8;
const int BRIGHT_YELLOW = 3 + 8;
const int BRIGHT_BLUE = 4 + 8;
const int BRIGHT_MAGENTA = 5 + 8;
const int BRIGHT_CYAN = 6 + 8;
const int BRIGHT_WHITE = 7 + 8;
const int DEFAULT_OFFSET = 226;
const int COLOR_MODIFIER = 15;
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

void setColor(int fg, int bg = Color::TRANSPARENT);

void printLine(std::wstring str);

void printBlank();

// This is just a test
unsigned distance(const std::wstring& input, const std::wstring& tcmp);

size_t closestIndex(const std::wstring& str, size_t index, wchar_t ch);
// void clearSurroundingWhitespace(std::string& str);
} // namespace aux
