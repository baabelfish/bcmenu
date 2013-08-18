#include "aux.hpp"
#include <curses.h>
#include <cmath>

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
        size_t nindex = closestIndex(input, index_start,  input[i]);
        score += (nindex != -1)
            ? std::abs(static_cast<int>(nindex) - static_cast<int>(index_start))
            : tcmp.size() * 2;
        index_start = nindex;
    }
    return score;
}

size_t closestIndex(const std::wstring& str, size_t index, wchar_t ch) {
    int litr = index - 1;
    int ritr = index + 1;
    int tindex = -1;

    while (litr >= 0 || ritr < str.size() - 1) {
        if (ritr < str.size() - 1 && str[ritr] == ch) {
            tindex = index + ritr;
            break;
        }
        else if (litr >= 0 && str[litr] == ch) {
            tindex = index - litr;
            break;
        }
        ++ritr;
        --litr;
    }
    return tindex;
}
} // namespace aux
