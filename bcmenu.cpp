#include <curses.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <sstream>

static std::string tempfile;
enum Color {
    Normal = 1
    , Typeline
    , TypelinePrefix
    , Selection
    , SelectionMark
    , Count
};
static const int DefaultColorsFg[] = {
    253
    , 142
    , 148
    , 154
    , 190
}
static const int DefaultColorsBg[] = {
    -1
    , -1
    , -1
    , 246
    , -1
}

bool match_straigth(const std::wstring& input, const std::wstring& result);
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
    if (argc < 2) {
        std::cerr << "Give the bloody file. Idiot." << std::endl;
        return 1;
    }
    tempfile = argv[1];
    std::vector<std::wstring> lines;
    readIn(lines);
    return takeInput(lines);
}

void readIn(std::vector<std::wstring>& lines) {
    std::string temp;
    while (getline(std::cin, temp)) {
        std::wstringstream ws;
        ws << temp.c_str();
        lines.push_back(ws.str());
    }
}

int takeInput(const std::vector<std::wstring>& lines) {
    if (lines.empty()) return 1;
    initCurses();
    printLine(L":");
    std::vector<size_t> choises;
    std::wstring input;
    std::wstring final_choise;
    int key = 0;
    int cx, cy;
    bool done = false;
    int choise = 0;

    move(1, 0);
    printLine(L"i  " + lines[0]);
    for (size_t i = 1; i < lines.size(); ++i) {
        printLine(L"  " + lines[i]);
    }

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

        move(0, 1);
        printLine(input + L"<");
        int done = 1;
        choises.clear();
        for (size_t i = 0; i < lines.size(); ++i) {
            // if (lines[i].find(input) != -1) {
            //     choises.push_back(i);
            // }
            if (match_straigth(input, lines[i])) {
                choises.push_back(i);
            }
        }
        if (choise > choises.size() - 1) choise = choises.size() - 1;
        for (auto i = 0; i < choises.size(); ++i) {
            if (i == choise) printLine(L"> " + lines[choises[i]]);
            else printLine(L"  " + lines[choises[i]]);
        }
        for (auto i = getRows() - done; i > 0; --i) printBlank();
    }
    endwin();
    std::wofstream myfile;
    myfile.open(tempfile);
    if (myfile.is_open()) {
        myfile << final_choise << std::endl;
        myfile.close();
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
    for (auto i = 1; i < cols; ++i) {
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

bool match_straigth(const std::wstring& input, const std::wstring& result) {
    size_t iinput = 0;
    for (auto i = 0; i < result.size(); ++i) {
        if (input[iinput] == result[i]) ++iinput;
        if (iinput == input.size()) return true;
    }
    return false;
}

void initPairs() {
    for (auto i = 0; i < Color::Count; ++i) {
        init_pair(i + 1, DefaultColorsFg[i], -DefaultColorsBg[i]);
    }
}
