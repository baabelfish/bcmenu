#include <curses.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>

enum class TypeCase {
    Exact = 0
    , Ignore
    , Smart
};

static std::string g_tempfile;
static TypeCase g_case = TypeCase::Exact;

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
static const std::string g_helptext =
"Tämä on jelppiteksti\
";

bool parseArguments(int argc, char* argv[]);
void setColor(int fg, int bg);
void attrReset();
void attrBold();
void attrUnderline();
void attrBlink();
bool matchStraight(const std::wstring& input, const std::wstring& result);
int fetchKey();
int getCols();
int getRows();
int takeInput(const std::vector<std::wstring>& lines);
void initCurses();
void initPairs();
void printBlank();
void printLine(std::wstring str);
void readIn(std::vector<std::wstring>& lines);

int main(int argc, char* argv[]) {
    if (!parseArguments(argc, argv)) return 1;
    g_tempfile = argv[1];
    std::vector<std::wstring> lines;
    readIn(lines);
    bool rval = takeInput(lines);
    return rval;
}

bool parseArguments(int argc, char* argv[]) {
    if (argc < 2) return false;
    for (auto i = 2; i < argc; ++i) {
        if (std::strcmp(argv[i], "--ignore-case")) {
            g_case = TypeCase::Ignore;
        }
        else if (std::strcmp(argv[i], "--smart-case")) {
            g_case = TypeCase::Smart;
        }
        else if (std::strcmp(argv[i], "--exact-case")) {
            g_case = TypeCase::Exact;
        }
        else if (std::strcmp(argv[i], "-h")) {
            std::cout << g_helptext << std::endl;
            return false;
        }
        else if (std::strcmp(argv[i], "--prompt")) {
        }
        else if (std::strcmp(argv[i], "--focus-prefix")) {
        }
        else if (std::strcmp(argv[i], "--color-active-fg")) {
        }
        else if (std::strcmp(argv[i], "--color-focused-fg")) {
        }
        else if (std::strcmp(argv[i], "--color-normal-fg")) {
        }
        else if (std::strcmp(argv[i], "--color-prefix-fg")) {
        }
        else if (std::strcmp(argv[i], "--color-input-fg")) {
        }
        else if (std::strcmp(argv[i], "--color-active-bg")) {
        }
        else if (std::strcmp(argv[i], "--color-focused-bg")) {
        }
        else if (std::strcmp(argv[i], "--color-normal-bg")) {
        }
        else if (std::strcmp(argv[i], "--color-prefix-bg")) {
        }
        else if (std::strcmp(argv[i], "--color-input-bg")) {
        }
        else {
            std::cout << g_helptext << std::endl;
            return false;
        }
    }
    return true;
}

void readIn(std::vector<std::wstring>& lines) {
    std::string temp;
    while (getline(std::cin, temp)) {
        if (temp == ".") continue;
        std::wstringstream ws;
        ws << temp.c_str();
        lines.push_back(ws.str());
    }
}

int takeInput(const std::vector<std::wstring>& lines) {
    if (lines.empty()) return 1;
    initCurses();
    setColor(Color::BRIGHT_GREEN, Color::TRANSPARENT);
    printLine(L":<");
    std::vector<size_t> choises;
    std::wstring input;
    std::wstring final_choise;
    int key = 0;
    int cx, cy;
    bool done = false;
    int choise = 0;
    int printline = 2;

    move(1, 0);
    setColor(Color::BRIGHT_YELLOW, Color::TRANSPARENT);
    printLine(L">  " + lines[0]);
    setColor(Color::WHITE, Color::TRANSPARENT);
    for (size_t i = 1; i < lines.size(); ++i) {
        move(printline++, 0);
        printLine(L"  " + lines[i]);
    }
    // for (auto i = getRows() - done; i > 0; --i) {
    //     move(printline++, 0);
    //     printBlank();
    // }

    while (!done) {
        key = fetchKey();
        if (key == 3) {
            done = true;
        }
        switch (key) {
            case 9:
                break;
            case KEY_BACKSPACE:
            case 127:
                if (!input.empty()) input = input.substr(0, input.size() - 1);
                break;
            case '\n':
            case '\r':
                if (!choises.empty() && !lines.empty() && choise < choises.size())
                    final_choise = lines[choises[choise]];
                done = true;
                break;
            case 14:
                ++choise;
                break;
            case 16:
                if (choise > 0) --choise;
                break;
            case 23:
                input.clear();
                break;
            case -1:
                break;
            default:
                input += key;
                break;
        }

        move(0, 0);
        printline = 1;
        setColor(Color::BRIGHT_GREEN, Color::TRANSPARENT);
        printLine(L":" + input + L"<");
        int done = 1;
        choises.clear();
        for (size_t i = 0; i < lines.size(); ++i) {
            // if (lines[i].find(input) != -1) {
            //     choises.push_back(i);
            // }
            if (matchStraight(input, lines[i])) {
                choises.push_back(i);
            }
        }
        if (choise > choises.size() - 1) choise = choises.size() - 1;
        for (auto i = 0; i < choises.size(); ++i) {
            move(printline++, 0);
            if (i == choise) {
                setColor(Color::BRIGHT_YELLOW, Color::TRANSPARENT);
                printLine(L"> " + lines[choises[i]]);
            }
            else {
                setColor(Color::WHITE, Color::TRANSPARENT);
                printLine(L"  " + lines[choises[i]]);
            }
        }
        setColor(Color::WHITE, Color::TRANSPARENT);
        for (auto i = getRows() - done; i > 0; --i) {
            move(printline++, 0);
            printBlank();
        }
        refresh();
    }
    endwin();
    std::wcerr << final_choise;
    return 0;
}

int fetchKey() {
    nodelay(stdscr, false);
    wint_t ch;
    int key = get_wch(&ch);
    key = ch;
    nodelay(stdscr, true);
    return key;
}

void initCurses() {
    freopen("/dev/tty", "rw", stdin);
    setlocale(LC_CTYPE, "");
    initscr();
    raw();
    noecho();
    nodelay(stdscr, true);
    wtimeout(stdscr, 10000);
    keypad(stdscr, true);
    use_default_colors();
    start_color();
    initPairs();
    curs_set(0);
}

void printLine(std::wstring str) {
    int cols = getCols();
    if (str.size() > cols) {
        str = str.substr(0, getCols());
    }
    else {
        int amount = cols - str.size();
        for (auto i = 0; i < amount; ++i) {
            str += L' ';
        }
    }
    printw("%ls", str.c_str());
}

void printBlank() {
    int cols = getCols();
    std::wstring str;
    for (auto i = 0; i < cols; ++i) {
        str += L' ';
    }
    str += L'\n';
    printLine(str);
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

bool matchStraight(const std::wstring& input, const std::wstring& result) {
    size_t iinput = 0;
    for (auto i = 0; i < result.size(); ++i) {
        if (input[iinput] == result[i]) ++iinput;
        if (iinput == input.size()) return true;
    }
    return false;
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

void initPairs() {
    for (int i = 0; i < Color::COLOR_MODIFIER; ++i) {
        init_pair(Color::DEFAULT_OFFSET + i, i, -1);
        for (int j = 0; j < Color::COLOR_MODIFIER; ++j) {
            init_pair(i * Color::COLOR_MODIFIER + j, i, j);
        }
    }
}

void attrReset() {
    // use_default_colors();
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

