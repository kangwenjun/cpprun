// Copyright(c) 2026-present, 康文君 & cpprun contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once
#include <chrono>

namespace cpprun::time
{

class stopwatch
{
public:
    stopwatch()
    {
        reset();
    }

    void reset()
    {
        start_time = std::chrono::steady_clock::now();
    }

    template <typename DurationType>
    long long elapsed() const
    {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<DurationType>(now - start_time);
        return elapsed.count();
    }

    long long elapsed_nanoseconds() const
    {
        return elapsed<std::chrono::nanoseconds>();
    }

    long long elapsed_microseconds() const
    {
        return elapsed<std::chrono::microseconds>();
    }

    long long elapsed_milliseconds() const
    {
        return elapsed<std::chrono::milliseconds>();
    }

    long long elapsed_seconds() const
    {
        return elapsed<std::chrono::seconds>();
    }

    long long elapsed_minutes() const
    {
        return elapsed<std::chrono::minutes>();
    }

    long long elapsed_hours() const
    {
        return elapsed<std::chrono::hours>();
    }

#if _HAS_CXX20
    long long elapsed_days() const
    {
        return elapsed<std::chrono::days>();
    }

    long long elapsed_weeks() const
    {
        return elapsed<std::chrono::weeks>();
    }

    long long elapsed_months() const
    {
        return elapsed<std::chrono::months>();
    }

    long long elapsed_years() const
    {
        return elapsed<std::chrono::years>();
    }
#endif

private:
    std::chrono::steady_clock::time_point start_time;
};

} // namespace cpprun::time


#ifdef __MAIN__
#include <iostream>

int main()
{
    cpprun::time::stopwatch sw;
    for (volatile int i = 0; i < 1000000; ++i);
    std::cout << "Elapsed time: " << sw.elapsed_milliseconds() << " ms" << std::endl;
    return 0;
}
#endif // __MAIN__