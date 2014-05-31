#pragma once

#include "../lib/cpputils/cu.hpp"
#include "matcher.hpp"

class MatcherFuzzy : public Matcher {
public:
    MatcherFuzzy():
        Matcher([](const std::wstring& x, const std::wstring& y) -> int {
            auto skipper = [](const std::wstring& input, const std::wstring& result) -> bool {
                if (input.size() > result.size()) { return false; }

                const std::size_t rsize = result.size();
                const std::size_t isize = input.size();
                for (std::size_t i = 0, iinput = 0; i < rsize; ++i) {
                    if (input.c_str()[iinput] == result.c_str()[i]) ++iinput;
                    if (iinput == isize) return true;
                }
                return false;
            };

            std::wstring a = x;
            std::wstring b = y;
            // std::wstring a = cs::toLowerCase(x);
            // std::wstring b = cs::toLowerCase(y);

            if (a == b) {
                return 0;
            }
            else if (skipper(b, a)) {
                return 1;
            }
            else if (a.find(b) != std::string::npos) {
                return 2;
            }
            else {
                return cs::levenshteinDistance(a, b, 2, 2, 1) + 1;
            }
        }) {}
};
