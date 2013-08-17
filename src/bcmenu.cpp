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

static bool g_draw_inverted = false;
static bool g_has_upper = false;
static std::string g_tempfile;
static std::wstring g_prompt = L":";
static TypeCase g_case = TypeCase::Smart;
static MatchAlgorithm g_algorithm;

static const std::string g_helptext =
"Usage: bcmenu [OPTIONS] <<< PIPE\n\
\n\
Bcmenu is a fuzzy menu for the terminal, made with ncurses.\n\
\n\
\n\
Options:\n\
--------\n\
\n\
--ignore-case or -ic\n\
    Ignores case in user input.\n\
\n\
--smart-case or -sc\n\
    Ignores case until user types an upper case letter. [Default]\n\
\n\
--exact-case or -ec\n\
    Input string's case must match exactly.\n\
\n\
--exact or -e\n\
    User input must match exactly to a part of the option.\n\
\n\
--fuzzy or -f\n\
    Real fuzzy matching [TODO]\n\
\n\
--simplyfuzzy or -sf\n\
    Uses naive fuzzy matching. [Default]\n\
\n\
--bottom or -b\n\
    Draw the prompt to the bottom of the window.\n\
\n\
-h or --help\n\
    Get this help text.\n\
\n\
--prompt <prompt_string> or -p <prompt_string>\n\
    Sets input prompt.\n\
\n\
--focus-prefix\n\
    String to use in front of the focused string. [TODO]\n\
\n\
--color-active-fg\n\
--color-focused-fg\n\
--color-normal-fg\n\
--color-prefix-fg\n\
--color-input-fg\n\
--color-active-bg\n\
--color-focused-bg\n\
--color-normal-bg\n\
--color-prefix-bg\n\
--color-input-bg\n\
    These are used to control colors. [TODO]\n\
\n\
\n\
Keybindings:\n\
------------\n\
\n\
Enter   Saves the result and exits the program.\n\
C-n     Next match.\n\
C-p     Previous match.\n\
C-w     Clears the search.\n\
C-c     Normal interrupt (result is not saved).\n\
C-o     Add focused item to selection.\n\
C-i     Unselect focused item.\
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
bool compareArgument(const char* arg, const std::string& choice_first, const std::string& choice_second);
bool matchCharacter(wchar_t a, wchar_t b);
void matchInputToLines(const std::wstring& input, std::deque<size_t>& choices
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
        if (compareArgument(argv[i], "--ignore-case", "-ic")) {
            g_case = TypeCase::Ignore;
        }
        else if (compareArgument(argv[i], "--smart-case", "-sc")) {
            g_case = TypeCase::Smart;
        }
        else if (compareArgument(argv[i], "--exact-case", "-ec")) {
            g_case = TypeCase::Exact;
        }
        else if (compareArgument(argv[i], "--exact", "-e")) {
            g_algorithm = MatchAlgorithm::Exact;
        }
        else if (compareArgument(argv[i], "--fuzzy", "-f")) {
            g_algorithm = MatchAlgorithm::Fuzzy;
        }
        else if (compareArgument(argv[i], "--simplyfuzzy", "-sf")) {
            g_algorithm = MatchAlgorithm::SimpleFuzzy;
        }
        else if (compareArgument(argv[i], "--bottom", "-b")) {
            g_draw_inverted = true;
        }
        else if (compareArgument(argv[i], "--help", "-h")) {
            return 2;
        }
        else if (compareArgument(argv[i], "--prompt", "-p")) {
            if (!aux::parseNext(argc, argv, i, 1, options) || options.size() != 1) return 2;
            else g_prompt = aux::stringToWideString(options[0]);
        }
        else if (compareArgument(argv[i], "--focus-prefix", "")) {
        }
        else if (compareArgument(argv[i], "--color-active-fg", "")) {
        }
        else if (compareArgument(argv[i], "--color-focused-fg", "")) {
        }
        else if (compareArgument(argv[i], "--color-normal-fg", "")) {
        }
        else if (compareArgument(argv[i], "--color-prefix-fg", "")) {
        }
        else if (compareArgument(argv[i], "--color-input-fg", "")) {
        }
        else if (compareArgument(argv[i], "--color-active-bg", "")) {
        }
        else if (compareArgument(argv[i], "--color-focused-bg", "")) {
        }
        else if (compareArgument(argv[i], "--color-normal-bg", "")) {
        }
        else if (compareArgument(argv[i], "--color-prefix-bg", "")) {
        }
        else if (compareArgument(argv[i], "--color-input-bg", "")) {
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

bool compareArgument(const char* arg, const std::string& choice_first, const std::string& choice_second) {
    return (std::strcmp(arg, choice_first.c_str()) == 0 || std::strcmp(arg, choice_second.c_str()) == 0);
}

int takeInput(const std::deque<std::wstring>& lines) {
    if (lines.empty()) return 1;
    initCurses();
    std::deque<size_t> choices;
    std::set<int> selected;
    std::wstring input;
    std::wstring final_choice;
    int key = 0;
    bool done = false;
    bool interrupt = false;
    int choice = 0;

    while (!done) {
        if (choice > choices.size() - 1) choice = choices.size() - 1;
        matchInputToLines(input, choices, lines);
        printOptions(lines, choices, 1, aux::getRows(), choice, selected);
        printInput(0, input);
        refresh();

        key = fetchKey();
        if (g_draw_inverted) {
            if (key == 14) key = 16;
            else if (key == 16) key = 14;
        }

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
                if (!choices.empty() && !lines.empty() && choice < choices.size())
                    final_choice = lines[choices[choice]];
                done = true;
                break;
            case 14:
                if ((unsigned)choice < choices.size() - 1) ++choice;
                break;
            case 16:
                if (choice > 0) --choice;
                break;
            case 15:
                selected.insert(choices[choice]);
                break;
            case 9:
                if (selected.find(choice) != selected.end())
                    selected.erase(selected.find(choice));
                break;
            case 23:
                choice = 0;
                input.clear();
                break;
            case -1:
                break;
            default:
                if (!(1 <= key && key <= 31)) input += key;
                break;
        }
    }

    endwin();
    if (!interrupt) {
        if (!selected.empty())
            for (auto it = selected.rbegin(); it != selected.rend(); ++it)
                std::wcerr << lines[*it] << std::endl;
        else std::wcerr << final_choice;
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

void matchInputToLines(const std::wstring& input, std::deque<size_t>& choices
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

    choices.clear();
    for (size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find(input) != -1) {
            choices.push_front(i);
        }
        else if (matchStraight(input, lines[i])) {
            choices.push_back(i);
        }
    }
}

void printInput(int line, const std::wstring& input) {
    if (g_draw_inverted) line = aux::getRows() - 1;
    move(line, 0);
    aux::setColor(Color::BRIGHT_GREEN, Color::TRANSPARENT);
    printLine(g_prompt + input);
}

void printOptions(const std::deque<std::wstring>& lines, const std::deque<size_t>& options
        , int line_top, int line_bottom, int selected, const std::set<int>& multiple) {
    int printline = 1;
    for (auto i = 0; i < options.size() && printline < line_bottom - line_top; ++i) {
        if (g_draw_inverted) move(aux::getRows() - 1 - printline++, 0);
        else move(printline++, 0);
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
        if (g_draw_inverted) move(aux::getRows() - 1 - printline++, 0);
        else move(printline++, 0);
        printBlank();
    }
}
