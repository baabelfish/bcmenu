#include "io.hpp"

#include "matcher_fuzzy.hpp"
#include "matches.hpp"

IO::IO() {
}

IO::~IO() {
}

void IO::printInput(const std::wstring& input) {
    int line = 0;
    move(line, 0);
    aux::setColor(Color::BRIGHT_GREEN, Color::TRANSPARENT);
    aux::printLine(Misc::Prompt + input);
}

std::vector<std::wstring> IO::readIn() {
    std::vector<std::wstring> lines;
    std::wstring temp;
    while (std::getline(std::wcin, temp)) {
        if (temp == L".") continue;
        lines.push_back(temp);
    }
    return lines;
}

int IO::fetchKey() {
    nodelay(stdscr, false);
    wint_t ch;
    int key = get_wch(&ch);
    key = ch;
    nodelay(stdscr, true);
    return key;
}

void IO::initCurses() {
    freopen("/dev/tty", "rw", stdin);
    initscr();
    raw();
    noecho();
    nodelay(stdscr, true);
    wtimeout(stdscr, 10000);
    keypad(stdscr, true);
    use_default_colors();
    start_color();
    curs_set(0);

    for (int i = 0; i < Color::COLOR_MODIFIER; ++i) {
        init_pair(Color::DEFAULT_OFFSET + i, i, -1);
        for (int j = 0; j < Color::COLOR_MODIFIER; ++j) {
            init_pair(i * Color::COLOR_MODIFIER + j, i, j);
        }
    }
}

int IO::takeInput(std::vector<std::wstring>&& lines) {
    if (lines.empty()) return 1;

    initCurses();

    Matches matches(MatcherFuzzy(), std::move(lines));
    std::wstring input;
    bool done = false;
    bool interrupt = true;
    bool needs_match = true;
    std::size_t choice = 0;

    while (!done) {
        if (needs_match) {
            matches.match(input);
            needs_match = false;
        }

        matches.print([&](const std::vector<std::wstring>& lines) {
            printChoices(lines, static_cast<int>(choice), {});
        });

        printInput(input);
        refresh();
        int key = fetchKey();

        switch (key) {
        case 3:
            done = true;
            interrupt = false;
            break;
        case KEY_BACKSPACE:
        case 127:
            if (!input.empty()) input = input.substr(0, input.size() - 1);
            needs_match = true;
            break;
        case '\r':
        case '\n':
            interrupt = false;
            done = true;
            break;
        case 'b' - 96:
            choice = 0;
            break;
        case 'e' - 96:
            break;
        case 'u' - 96:
            break;
        case 'd' - 96:
            break;
        case 'n' - 96:
            if (choice < matches.amount() - 1) ++choice;
            break;
        case 'p' - 96:
            if (choice > 0) --choice;
            break;
        case 'o' - 96:
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
        std::wcerr << matches.get()[choice];
    }

    return 0;
}

void IO::printChoices(const std::vector<std::wstring>& choices,
                      int selected, const std::set<int>& multiple) {
    if (choices.empty()) return;

    int crows = aux::getRows() - 2;
    int offset = selected > crows / 2 ? selected - crows / 2: 0;
    if (selected > choices.size() - crows / 2) offset = choices.size() - crows - 1;

    for (auto i = 0; i + offset < choices.size() && i < aux::getRows() - 1; ++i) {
        move(i + 1, 0);
        int indexer = i + offset;

        if (selected == indexer) {
            aux::setColor(Color::BRIGHT_GREEN);
            if (multiple.find(indexer) != multiple.end()) {
                aux::printLine(Misc::PromptFocused + Misc::PromptSelected + choices[indexer]);
            }
            else {
                aux::printLine(Misc::PromptFocused + L" " + choices[indexer]);
            }
        }
        else if (multiple.find(indexer) != multiple.end()) {
            aux::setColor(Color::GREEN);
            aux::printLine(L" " + Misc::PromptSelected + choices[indexer]);
        }
        else {
            aux::setColor(Color::WHITE);
            aux::printLine(L"  " + choices[indexer]);
        }
    }

    for (std::size_t i = choices.size(); i < static_cast<std::size_t>(aux::getRows() - 1); ++i) {
        move(i + 1, 0);
        aux::printBlank();
    }
}
