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
} // namespace aux
