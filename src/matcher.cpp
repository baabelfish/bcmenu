#include "matcher.hpp"

Matcher::Matcher(decltype(algorithm) algorithm):
    algorithm(algorithm) {
}

Matcher::~Matcher() {
}

int Matcher::match(const std::wstring& a, const std::wstring& b) {
    return algorithm(a, b);
}
