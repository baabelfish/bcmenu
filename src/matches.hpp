#pragma once

#include "matcher.hpp"
#include <functional>
#include <string>
#include <vector>

class Matches {
    typedef std::vector<std::wstring> LineContainer;
    Matcher m_matcher;
    std::wstring m_old_input;
    LineContainer m_initial;
    LineContainer m_matches;

    LineContainer doMatch(const std::wstring& input, const LineContainer& tested);

public:
    Matches(Matcher matcher, LineContainer&& initial);
    virtual ~Matches();

    void match(std::wstring input);
    void print(std::function<void(const LineContainer&)> f) const;
    std::size_t amount() const;
    std::wstring get(std::size_t index) const;
};
