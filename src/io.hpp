#pragma once

#include "aux.hpp"
#include <map>
#include <set>

class IO {
public:
    IO();
    virtual ~IO();

    static std::vector<std::wstring> readIn();
    static int fetchKey();
    static void initCurses();
    int takeInput(std::vector<std::wstring>&& lines);

private:
    static void printInput(const std::wstring& input);
    void printChoices(const std::vector<std::wstring>& choices, int selected, const std::set<int>& multiple);
};
