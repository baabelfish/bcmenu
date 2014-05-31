#pragma once

#include <functional>
#include <map>
#include <functional>
#include <string>
#include <vector>
#include <mutex>
#include "matcher.hpp"

class Matches {
    typedef std::vector<std::wstring> LineContainer;
    Matcher m_matcher;
    std::wstring m_old_input;
    LineContainer m_initial;
    std::multimap<int, std::wstring> m_matches;
    std::mutex m_mutex;

    void doMatch(const std::wstring& input);
    void clearMatches();

public:
    Matches(Matcher matcher, LineContainer&& initial);
    virtual ~Matches();

    void match(std::wstring input);
    void print(std::function<void(const LineContainer&)> f);
    std::size_t amount() const;
    std::vector<std::wstring> get();

    void accessMatches(std::function<void(decltype(m_matches)&)> f) {
        std::lock_guard<std::mutex> l(m_mutex);
        f(m_matches);
    }
};
