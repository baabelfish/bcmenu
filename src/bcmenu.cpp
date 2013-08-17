#include <curses.h>
#include <deque>
#include <set>
#include <locale>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include "aux.hpp"

enum class MatchAlgorithm {
    Exact
    , SimpleFuzzy
    , Fuzzy
};
enum class TypeCase {
    Exact = 0
    , Ignore
    , Smart
};

static bool g_has_upper = false;
static std::string g_tempfile;
static std::wstring g_prompt;
static TypeCase g_case = TypeCase::Smart;
static MatchAlgorithm g_algorithm;

static const std::string g_helptext =
"Tämä on jelppiteksti\
";

int parseArguments(int argc, char* argv[]);
bool matchStraight(const std::wstring& input, const std::wstring& result);
int fetchKey();
int takeInput(const std::deque<std::wstring>& lines);
void initCurses();
void initPairs();
void printBlank();
void printLine(std::wstring str);
void readIn(std::deque<std::wstring>& lines);
bool matchCharacter(wchar_t a, wchar_t b);
void matchInputToLines(const std::wstring& input, std::deque<size_t>& choises
        , const std::deque<std::wstring>& lines);
void printInput(int line, const std::wstring& input);
void printOptions(const std::deque<std::wstring>& lines, const std::deque<size_t>& options
        , int line_top, int line_bottom, int selected, const std::set<int>& multiple);

int main(int argc, char* argv[]) {
    int re = parseArguments(argc, argv);
    if (re == 2) std::cout << g_helptext << std::endl;
    if (re != 0) {
        return 1;
    }
    g_tempfile = argv[1];
    std::deque<std::wstring> lines;
    readIn(lines);
    bool rval = takeInput(lines);
    return rval;
}

int parseArguments(int argc, char* argv[]) {
    if (argc < 2 || std::strcmp("1234", argv[1]) != 0) {
        std::cerr << "You should use bcmenu instead of this." << std::endl;
        return 1;
    }

    std::deque<std::string> options;

    for (auto i = 2; i < argc; ++i) {
        if (std::strcmp(argv[i], "--ignore-case") == 0) {
            g_case = TypeCase::Ignore;
        }
        else if (std::strcmp(argv[i], "--smart-case") == 0) {
            g_case = TypeCase::Smart;
        }
        else if (std::strcmp(argv[i], "--exact-case") == 0) {
            g_case = TypeCase::Exact;
        }
        else if (std::strcmp(argv[i], "--exact") == 0) {
            g_algorithm = MatchAlgorithm::Exact;
        }
        else if (std::strcmp(argv[i], "--fuzzy") == 0) {
            g_algorithm = MatchAlgorithm::Fuzzy;
        }
        else if (std::strcmp(argv[i], "--simplyfuzzy") == 0) {
            g_algorithm = MatchAlgorithm::SimpleFuzzy;
        }
        else if (std::strcmp(argv[i], "-h") == 0) {
            return 2;
        }
        else if (std::strcmp(argv[i], "--prompt") == 0) {
            if (!aux::parseNext(argc, argv, i, 1, options) || options.size() != 1) return 2;
            else g_prompt = aux::stringToWideString(options[0]);
        }
        else if (std::strcmp(argv[i], "--focus-prefix") == 0) {
        }
        else if (std::strcmp(argv[i], "--color-active-fg") == 0) {
        }
        else if (std::strcmp(argv[i], "--color-focused-fg") == 0) {
        }
        else if (std::strcmp(argv[i], "--color-normal-fg") == 0) {
        }
        else if (std::strcmp(argv[i], "--color-prefix-fg") == 0) {
        }
        else if (std::strcmp(argv[i], "--color-input-fg") == 0) {
        }
        else if (std::strcmp(argv[i], "--color-active-bg") == 0) {
        }
        else if (std::strcmp(argv[i], "--color-focused-bg") == 0) {
        }
        else if (std::strcmp(argv[i], "--color-normal-bg") == 0) {
        }
        else if (std::strcmp(argv[i], "--color-prefix-bg") == 0) {
        }
        else if (std::strcmp(argv[i], "--color-input-bg") == 0) {
        }
        else {
            return 2;
        }
    }
    return 0;
}

void readIn(std::deque<std::wstring>& lines) {
    std::string temp;
    while (getline(std::cin, temp)) {
        if (temp == ".") continue;
        lines.push_front(aux::stringToWideString(temp));
    }
}

int takeInput(const std::deque<std::wstring>& lines) {
    if (lines.empty()) return 1;
    initCurses();
    std::deque<size_t> choises;
    std::set<int> selected;
    std::wstring input;
    std::wstring final_choise;
    int key = 0;
    bool done = false;
    bool interrupt = false;
    int choise = 0;

    while (!done) {
        if (choise > choises.size() - 1) choise = choises.size() - 1;
        matchInputToLines(input, choises, lines);
        printInput(0, input);
        printOptions(lines, choises, 1, aux::getRows(), choise, selected);
        refresh();

        key = fetchKey();
        switch (key) {
            // case KEY_ESC:
            case 3:
                done = true;
                interrupt = true;
                break;
            case KEY_BACKSPACE:
            case 127:
                if (!input.empty()) input = input.substr(0, input.size() - 1);
                break;
            case '\r':
            case '\n':
                if (!choises.empty() && !lines.empty() && choise < choises.size())
                    final_choise = lines[choises[choise]];
                done = true;
                break;
            case 14:
                if ((unsigned)choise < choises.size() - 1) ++choise;
                break;
            case 16:
                if (choise > 0) --choise;
                break;
            case 15:
                selected.insert(choises[choise]);
                break;
            case 9:
                if (selected.find(choise) != selected.end())
                    selected.erase(selected.find(choise));
                break;
            case 23:
                choise = 0;
                input.clear();
                break;
            case -1:
                break;
            default:
                input += key;
                break;
        }
    }

    endwin();
    if (!interrupt) {
        if (!selected.empty())
            for (auto it = selected.rbegin(); it != selected.rend(); ++it)
                std::wcerr << lines[*it] << std::endl;
        else std::wcerr << final_choise;
    }
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
    int cols = aux::getCols();
    if (str.size() > (unsigned)cols) {
        str = str.substr(0, aux::getCols());
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
    int cols = aux::getCols();
    std::wstring str;
    for (auto i = 0; i < cols; ++i) {
        str += L' ';
    }
    str += L'\n';
    printLine(str);
}

bool matchStraight(const std::wstring& input, const std::wstring& result) {
    size_t iinput = 0;
    for (auto i = 0; i < result.size(); ++i) {
        if (matchCharacter(input[iinput], result[i])) ++iinput;
        if (iinput == input.size()) return true;
    }
    return false;
}

void initPairs() {
    for (int i = 0; i < Color::COLOR_MODIFIER; ++i) {
        init_pair(Color::DEFAULT_OFFSET + i, i, -1);
        for (int j = 0; j < Color::COLOR_MODIFIER; ++j) {
            init_pair(i * Color::COLOR_MODIFIER + j, i, j);
        }
    }
}

bool matchCharacter(wchar_t a, wchar_t b) {
    if (g_case == TypeCase::Ignore || (g_case == TypeCase::Smart && !g_has_upper))
        return std::towlower(a) == std::tolower(b);
    else return a == b;
}

void matchInputToLines(const std::wstring& input, std::deque<size_t>& choises
        , const std::deque<std::wstring>& lines) {
    if (g_case == TypeCase::Smart) {
        g_has_upper = false;
        for (auto& x : input) {
            if (isupper(x)) {
                g_has_upper = true;
                break;
            }
        }
    }

    choises.clear();
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find(input) != -1) {
            choises.push_front(i);
        }
        else if (matchStraight(input, lines[i])) {
            choises.push_back(i);
        }
    }
}

void printInput(int line, const std::wstring& input) {
    move(line, 0);
    aux::setColor(Color::BRIGHT_GREEN, Color::TRANSPARENT);
    printLine(g_prompt + input + L"<");
}

void printOptions(const std::deque<std::wstring>& lines, const std::deque<size_t>& options
        , int line_top, int line_bottom, int selected, const std::set<int>& multiple) {
    int printline = 1;
    for (auto i = 0; i < options.size() && printline < line_bottom - line_top; ++i) {
        move(printline++, 0);
        if (i == selected) {
            aux::setColor(Color::BRIGHT_GREEN, Color::TRANSPARENT);
            if (multiple.find(options[i]) != multiple.end()) printLine(L"*>" + lines[options[i]]);
            else printLine(L"> " + lines[options[i]]);
        }
        else if (multiple.find(options[i]) != multiple.end()) {
            aux::setColor(Color::GREEN, Color::TRANSPARENT);
            printLine(L"* " + lines[options[i]]);
        }
        else {
            aux::setColor(Color::WHITE, Color::TRANSPARENT);
            printLine(L"  " + lines[options[i]]);
        }
    }
    aux::setColor(Color::WHITE, Color::TRANSPARENT);
    for (auto i = printline; i < line_bottom - line_top; ++i) {
        move(printline++, 0);
        printBlank();
    }
}
