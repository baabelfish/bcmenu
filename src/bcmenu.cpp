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

#include <functional>
#include <regex>
#include <map>
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
void printInput(const std::wstring& input);
void printChoices(const std::deque<std::wstring>& lines, const std::deque<size_t>& choices
        , int selected, const std::set<int>& multiple);
void printHelp();

enum class MatchAlgorithm {
    Exact
    , SimpleFuzzy
    , Fuzzy
    , Regex
};

enum class TypeCase {
    Exact = 0
    , Ignore
    , Smart
};

static bool g_debug = false;
static bool g_draw_inverted = false;
static bool g_has_upper = false;
static std::string g_tempfile;
static std::wstring g_prompt = L":";
static std::wstring g_prefix_focus = L">";
static std::wstring g_prefix_selected = L"*";
static TypeCase g_case = TypeCase::Smart;
static MatchAlgorithm g_algorithm = MatchAlgorithm::SimpleFuzzy;
static size_t g_matchindex_start = 0;
static size_t g_matchindex_end = -1;
static int g_color_active_fg = Color::TRANSPARENT;
static int g_color_focused_fg = Color::BRIGHT_GREEN;
static int g_color_selected_fg = Color::GREEN;
static int g_color_normal_fg = Color::WHITE;
static int g_color_prefix_fg = Color::GREEN;
static int g_color_input_fg = Color::BRIGHT_GREEN;
static int g_color_active_bg = Color::TRANSPARENT;
static int g_color_focused_bg = Color::TRANSPARENT;
static int g_color_selected_bg = Color::TRANSPARENT;
static int g_color_normal_bg = Color::TRANSPARENT;
static int g_color_prefix_bg = Color::TRANSPARENT;
static int g_color_input_bg = Color::TRANSPARENT;

static const std::string g_helptext =
"Usage: bcmenu [OPTIONS] <<< PIPE\n\
\n\
Bcmenu is a fuzzy menu for the terminal, made with ncurses.\n\
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
C-i     Unselect focused item.\n\
C-u     Move page up\n\
C-d     Move page down\n\
C-b     Move to first item\n\
C-e     Move to last item\n\
\n\
\n\
Arguments:\n\
----------\
";

struct Argument {
    std::string argl;
    std::string args;
    std::string description;
    int rval;
    std::function<void (int&, int, char**)> func;
};

Argument g_args[] = {
    { "--ignore-case", "-ic", "Ignores case completely.", 0, [&](int&, int, char**) {
        g_case = TypeCase::Ignore;
    }},
    { "--smart-case", "-sc", "Ignores case until user presses an uppercase key. [Default]", 0, [&](int&, int, char**) {
        g_case = TypeCase::Smart;
    }},
    { "--exact-case", "-ec", "Matches characters exactly.", 0, [&](int&, int, char**) {
        g_case = TypeCase::Exact;
    }},
    { "--exact", "-e", "Uses exact match as algorithm.", 0, [&](int&, int, char**) {
        g_algorithm = MatchAlgorithm::Exact;
    }},
    { "--regex", "-r", "Matches with regex. [Todo]", 0, [&](int&, int, char**) {
        g_algorithm = MatchAlgorithm::Regex;
    }},
    { "--fuzzy", "-f", "Uses real fuzzy matching. [Todo]", 0, [&](int&, int, char**) {
        g_algorithm = MatchAlgorithm::Fuzzy;
    }},
    { "--simplyfuzzy", "-sf", "Uses a naive matching algorithm. [Default]", 0, [&](int&, int, char**) {
        g_algorithm = MatchAlgorithm::SimpleFuzzy;
    }},
    { "--prompt", "-p", "Sets prompt. (Default: \':\')", 0, [&](int& selected, int argc, char** argv) {
        if (++selected < argc) g_prompt = aux::stringToWideString(argv[selected]);
    }},
    { "--bottom", "-b", "Draws everything horizontally inverted.", 0, [&](int&, int, char**) {
        g_draw_inverted = true;
    }},
    { "--help", "-h", "Prints this helptext.", 2, nullptr },
    { "--debug", "", "Shows possible debug information.", 0, [&](int&, int, char**) {
        g_debug = true;
    }},
    { "--start-index", "-s", "The minimum index of the string to use in match. (Default: 0)", 0, [&](int& selected, int argc, char** argv) {
        if (++selected < argc) g_matchindex_start = std::atoi(argv[selected]);
    }},
    { "--end-index", "-e", "The maximum index of the string to use in match. (Default: The last index of the string)", 0, [&](int& selected, int argc, char** argv) {
        if (++selected < argc) g_matchindex_end = std::atoi(argv[selected]);
    }},
    { "--prefix-focus", "-pf", "Prefix character for focused item in the view. (Only one character is allowed)", 0, [&](int& selected, int argc, char** argv) {
        std::wstring prefix;
        if (++selected < argc) {
            prefix = aux::stringToWideString(argv[selected]);
            if (prefix.size() == 1) g_prefix_focus = prefix;
        }
    }},
    { "--prefix-selected", "-ps", "Prefix character to selected items. (Only one character is allowed)", 0, [&](int& selected, int argc, char** argv) {
        std::wstring prefix;
        if (++selected < argc) {
            prefix = aux::stringToWideString(argv[selected]);
            if (prefix.size() == 1) g_prefix_selected = prefix;
        }
    }},
    { "--color-selected-fg", "", "", 0, [&](int&, int, char**) {
    }},
    { "--color-focused-fg", "", "", 0, [&](int&, int, char**) {
    }},
    { "--color-normal-fg", "", "", 0, [&](int&, int, char**) {
    }},
    { "--color-prefix-fg", "", "", 0, [&](int&, int, char**) {
    }},
    { "--color-input-fg", "", "", 0, [&](int&, int, char**) {
    }},
    { "--color-selected-bg", "", "", 0, [&](int&, int, char**) {
    }},
    { "--color-focused-bg", "", "", 0, [&](int&, int, char**) {
    }},
    { "--color-normal-bg", "", "", 0, [&](int&, int, char**) {
    }},
    { "--color-prefix-bg", "", "", 0, [&](int&, int, char**) {
    }},
    { "--color-input-bg", "", "", 0, [&](int&, int, char**) {
    }}
};
static const size_t g_args_size = sizeof(g_args) / sizeof(Argument);

int main(int argc, char* argv[]) {
    setlocale(LC_CTYPE, "");
    int re = parseArguments(argc, argv);
    if (re == 2) printHelp();
    if (re != 0) {
        return 1;
    }

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

    for (auto i = 2; i < argc; ++i) {
        bool found = false;
        for (int j = 0; j < g_args_size; ++j) {
            if (compareArgument(argv[i], g_args[j].args, g_args[j].argl)) {
                if (g_args[j].func) g_args[j].func(i, argc, argv);
                found = true;
                if (g_args[j].rval != 0) return g_args[j].rval;
            }
        }
        if (!found) return 2;
    }
    return 0;
}

void readIn(std::deque<std::wstring>& lines) {
    std::wstring temp;
    while (std::getline(std::wcin, temp)) {
        if (temp == L".") continue;
        lines.push_front(temp);
    }
}

bool compareArgument(const char* arg, const std::string& choice_first, const std::string& choice_second) {
    return (std::strcmp(arg, choice_first.c_str()) == 0 || std::strcmp(arg, choice_second.c_str()) == 0);
}

int takeInput(const std::deque<std::wstring>& lines) {
    if (lines.empty()) return 1;
    initCurses();
    std::deque<size_t> choices;
    std::multimap<unsigned, size_t> pri_choices;
    std::set<int> selected;
    std::wstring input;
    std::wstring final_choice;
    int key = 0;
    bool done = false;
    bool interrupt = false;
    bool needs_match = true;
    int choice = 0;

    while (!done) {
        if (choice > choices.size() - 1) choice = choices.size() - 1;
        if (needs_match) {
            matchInputToLines(input, choices, lines);
            needs_match = false;
        }
        printChoices(lines, choices, choice, selected);
        printInput(input);
        refresh();
        key = fetchKey();
        if (g_draw_inverted) {
            if (key == 14) key = 16;
            else if (key == 16) key = 14;
        }
        switch (key) {
        case 3:
            done = true;
            interrupt = true;
            break;
        case KEY_BACKSPACE:
        case 127:
            if (!input.empty()) input = input.substr(0, input.size() - 1);
            needs_match = true;
            break;
        case '\r':
        case '\n':
            if (!choices.empty() && !lines.empty() && choice < choices.size())
                final_choice = lines[choices[choice]];
            done = true;
            break;
        case 'b' - 96:
            choice = 0;
            break;
        case 'e' - 96:
            choice = choices.size() - 1;
            break;
        case 'u' - 96:
            choice -= (aux::getRows() - 0) / 2;
            if (choice < 0) choice = 0;
            break;
        case 'd' - 96:
            choice += (aux::getRows() - 2) / 2;
            if (choice > choices.size() - 2) choice = choices.size() - 1;
            break;
        case 'n' - 96:
            if ((unsigned)choice < choices.size() - 1) ++choice;
            break;
        case 'p' - 96:
            if (choice > 0) --choice;
            break;
        case 'o' - 96:
            if (selected.find(choices[choice]) != selected.end()) selected.erase(selected.find(choices[choice]));
            else selected.insert(choices[choice]);
            break;
        case 'w' - 96:
            choice = 0;
            input.clear();
            needs_match = true;
            break;
        case -1:
            break;
        default:
            if (!(1 <= key && key <= 31)) input += key;
            needs_match = true;
            break;
        }
    }

    endwin();

    if (!interrupt) {
        if (!selected.empty()) {
            for (auto it = selected.rbegin(); it != selected.rend(); ++it) {
                std::wcerr << lines[*it] << std::endl;
            }
        }
        else {
            std::wcerr << final_choice;
        }
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
    // aux::setColor(Color::TRANSPARENT, Color::TRANSPARENT);
    aux::attrReset();
    printLine(str);
}

bool matchRegex(const std::wstring& input, const std::wstring& result) {
    if (std::regex_match(result, std::wregex(input))) return true;
    return false;
}

bool matchStraight(const std::wstring& input, const std::wstring& result) {
    size_t iinput = 0;
    size_t amount = g_matchindex_end == -1 || g_matchindex_end > result.size()
        ? result.size()
        : g_matchindex_end;

    for (auto i = g_matchindex_start; i < amount; ++i) {
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
    return a == b;
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
        switch (g_algorithm) {
        case MatchAlgorithm::Regex:
            if (matchRegex(input, lines[i])) choices.push_back(i);
            break;
        case MatchAlgorithm::Exact:
            if (lines[i].find(input) != -1) choices.push_front(i);
            break;
        case MatchAlgorithm::SimpleFuzzy:
            if (lines[i].find(input) != -1) choices.push_front(i);
            else if (matchStraight(input, lines[i])) choices.push_back(i);
            break;
        case MatchAlgorithm::Fuzzy:
            break;
        default:
            break;
        }
    }
}

void printInput(const std::wstring& input) {
    int line = 0;
    if (g_draw_inverted) line = aux::getRows() - 1;
    move(line, 0);
    aux::setColor(g_color_input_fg, g_color_input_bg);
    printLine(g_prompt + input);
}

void printChoices(const std::deque<std::wstring>& lines, const std::deque<size_t>& choices, int selected, const std::set<int>& multiple) {
    if (choices.empty()) return;

    int crows = aux::getRows() - 2;
    int offset = selected > crows / 2 ? selected - crows / 2: 0;
    if (selected > choices.size() - crows / 2) offset = choices.size() - crows - 1;

    for (auto i = 0; i + offset < choices.size() && i < aux::getRows() - 1; ++i) {
        if (g_draw_inverted) move(aux::getRows() - 2 - i, 0);
        else move(i + 1, 0);
        int indexer = i + offset;

        if (selected == indexer) {
            aux::setColor(g_color_focused_fg, g_color_focused_bg);
            if (multiple.find(choices[indexer]) != multiple.end()) printLine(g_prefix_focus + g_prefix_selected + lines[choices[indexer]]);
            else printLine(g_prefix_focus + L" " + lines[choices[indexer]]);
        }
        else if (multiple.find(choices[indexer]) != multiple.end()) {
            aux::setColor(g_color_selected_fg, g_color_selected_bg);
            printLine(L" " + g_prefix_selected + lines[choices[indexer]]);
        }
        else {
            aux::setColor(g_color_normal_fg, g_color_normal_bg);
            printLine(L"  " + lines[choices[indexer]]);
        }
    }

    for (auto i = choices.size(); i < aux::getRows() - 1; ++i) {
        if (g_draw_inverted) move(aux::getRows() - 2 - i, 0);
        else move(i + 1, 0);
        printBlank();
    }
}

void printHelp() {
    std::cout << g_helptext << std::endl;
    for (auto i = 0; i < g_args_size; ++i) {
        std::cout << g_args[i].args << " or " << g_args[i].argl << std::endl;
        std::cout << "    " << g_args[i].description << std::endl << std::endl;
    }
}
