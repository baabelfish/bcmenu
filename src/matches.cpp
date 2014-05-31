#include "matches.hpp"

#include "../lib/cpputils/cu.hpp"
#include <map>

Matches::Matches(Matcher matcher, std::vector<std::wstring>&& initial):
    m_matcher(matcher),
    m_old_input(L""),
    m_initial(std::move(initial)),
    m_matches() {
    }

Matches::~Matches() {}

void Matches::match(std::wstring input) {
    if (input.empty()) {
        clearMatches();
    }
    else if (input != m_old_input) {
        doMatch(input);
    }
}

void Matches::print(std::function<void(const std::vector<std::wstring>&)> f) {
    accessMatches([&](decltype(m_matches)& matches){
        f(cu::values(matches));
    });
}

void Matches::doMatch(const std::wstring& input) {
    m_matches.clear();
    for (auto& x : m_initial) {
        auto cost = m_matcher.match(x, input);
        accessMatches([&](decltype(m_matches)& matches){
            matches.insert(std::make_pair(cost, x));
        });
    }
}

std::size_t Matches::amount() const {
    return m_matches.size();
}

std::vector<std::wstring> Matches::get() {
    std::vector<std::wstring> x;
    accessMatches([&](decltype(m_matches)& matches){
        x = cu::values(matches);
    });
    return cu::values(m_matches);
}

void Matches::clearMatches() {
    for (auto& x : m_initial) {
        m_matches.insert(std::make_pair(0, x));
    }
}
