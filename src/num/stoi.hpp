// Copyright(c) 2026-present, 康文君 & cpprun contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <string>
#include <charconv> // C++17

namespace cpprun::num
{
    template <typename T>
    inline bool stoi(const char* str, const char* end, T& value, int base = 0)
    {
        auto res = std::from_chars(str, end, value, base);
        return res.ec == std::errc();
    }

    template <typename T>
    inline bool stoi(const std::string& s, T& value, int base = 0)
    {
        return stoi(s.data(), s.data() + s.size(), value, base);
    }
}


#ifdef __MAIN__
#include <iostream>
#include <cassert>


int main()
{
    using namespace cpprun::num;

    std::string s = "12345xyz";
    long value = 0;
    if (stoi(s, value))
    {
        assert(value == 12345);
        std::cout << "parsed int: " << value << "\n";
    }
    else
    {
        std::cout << "failed to parse int\n";
    }

    return 0;
}

#endif // __MAIN__