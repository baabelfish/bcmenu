#pragma once

#include <functional>

class Matcher {
    std::function<int(const std::wstring&, const std::wstring&)> algorithm;

public:
    virtual ~Matcher();
    int match(const std::wstring& a, const std::wstring& b);

protected:
    Matcher(decltype(algorithm) algorithm);
};
