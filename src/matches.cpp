#include "matches.hpp"

#include "../lib/cpputils/cu.hpp"
#include <map>

Matches::Matches(Matcher matcher, std::vector<std::wstring>&& initial):
    m_matcher(matcher),
    m_old_input(L""),
    m_initial(std::move(initial)),
    m_matches(m_initial) {}

Matches::~Matches() {}

void Matches::match(std::wstring input) {
    if (input.empty()) {
        m_matches = m_initial;
    }
    else if (input != m_old_input) {
        m_matches = doMatch(input, m_initial);
    }
}

void Matches::print(std::function<void(const decltype(m_matches)&)> f) const {
    f(m_matches);
}

Matches::LineContainer Matches::doMatch(const std::wstring& input, const Matches::LineContainer& tested) {
    std::multimap<int, std::wstring> mapped;
    for (auto& x : tested) {
        auto cost = m_matcher.match(x, input);
        mapped.insert(std::make_pair(cost, x));
    }
    return cu::values(mapped);
}

std::size_t Matches::amount() const {
    return m_matches.size();
}

std::wstring Matches::get(std::size_t index) const {
    return m_matches[index];
}
