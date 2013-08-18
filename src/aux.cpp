#include "aux.hpp"
#include <curses.h>

namespace aux {

std::wstring stringToWideString(const std::string& str) {
    std::wstringstream ws;
    ws << str.c_str();
    return ws.str();
}

bool parseNext(int argc, char* argv[], int& current, int option_amount, std::deque<std::string>& options) {
    ++current;
    if (current < argc) {
        std::string str = argv[current];
        options.push_back(str);
        return true;
    }
    return false;
}

void attrReset() {
    attron(COLOR_PAIR(Color::DEFAULT));
    attroff(A_BLINK);
    attroff(A_UNDERLINE);
    attroff(A_BOLD);
}

void attrBold() {
    attron(A_BOLD);
}

void attrUnderline() {
    attron(A_UNDERLINE);
}

void attrBlink() {
    attron(A_BLINK);
}

int getCols() {
    int value;
    int asda;
    getmaxyx(stdscr, asda, value);
    return value;
}

int getRows() {
    int value;
    int asda;
    getmaxyx(stdscr, value, asda);
    return value;
}

void setColor(int foreground, int background) {
    if (background == -1) {
        if (foreground == -1) {
            attron(COLOR_PAIR(Color::DEFAULT));
        }
        else {
            attron(COLOR_PAIR(foreground + Color::DEFAULT_OFFSET));
        }
    }
    else {
        attron(COLOR_PAIR(foreground * Color::COLOR_MODIFIER + background));
    }
}

unsigned distance(const std::wstring& input, const std::wstring& tcmp) {
    if (input.empty()) return 0;

    int index_start = 0;
    while (index_start < tcmp.size() && tcmp[index_start] != input[0]) ++index_start;
    if (index_start == tcmp.size()) return -1;
    unsigned score = index_start;

    for (auto i = 1; i < input.size(); ++i) {
        unsigned left_score = 0;
        unsigned right_score = 0;

        // Left side
        int lmove = 0;
        for (auto j = index_start - 1; j >= 0; --j) {
            ++lmove;
            if (tcmp[j] == input[i]) break;
            else if (tcmp[j] == L' ') left_score += 2;
            else if (i == 0) lmove = 0;
            else left_score += 1;
        }
        // Right side
        int rmove = 0;
        for (auto j = index_start + 1; j < tcmp.size(); ++j) {
            ++rmove;
            if (tcmp[j] == input[i]) break;
            else if (tcmp[j] == L' ') right_score += 2;
            else if (i == tcmp.size() - 1) rmove = 0;
            else right_score += 1;
        }

        if (rmove == 0 && lmove == 0) score += tcmp.size() * 2;
        if (left_score < right_score) {
            score += left_score;
            index_start -= lmove;
        }
        else {
            score += right_score;
            index_start += rmove;
        }
    }
    return score;
}
} // namespace aux
