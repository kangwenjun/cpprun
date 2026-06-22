// Copyright(c) 2026-present, 康文君 & cpprun contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>

/*
1: --- duration: 0 ms ---
1: wait_for:    0.05 us
1: wait_until:  0.06 us
1: sleep_for:   0.04 us
1: sleep_until: 0.05 us
1: spin_for:    0.04 us
1: spin_until:  0.04 us
1: --- duration: 1 ms ---
1: wait_for:    15586.4 us --- 1/64s(15.625ms)
1: wait_until:  15471.6 us
1: sleep_for:   15650 us
1: sleep_until: 15631.1 us
1: spin_for:    1000 us
1: spin_until:  1000.85 us
*/

namespace cpprun::time 
{

    template <class _Rep, class _Period>
    void wait_for(const std::chrono::duration<_Rep, _Period> &duration)
    {
        std::mutex mtx;
        std::unique_lock<std::mutex> lock(mtx);
        std::condition_variable cv;
        cv.wait_for(lock, duration, [] { return false; });
    }

    void wait_until(const std::chrono::steady_clock::time_point &abs_time)
    {
        std::mutex mtx;
        std::unique_lock<std::mutex> lock(mtx);
        std::condition_variable cv;
        cv.wait_until(lock, abs_time, [] { return false; });
    }

    void sleep_for(const std::chrono::milliseconds &duration)
    {
        std::this_thread::sleep_for(duration);
    }

    void sleep_until(const std::chrono::steady_clock::time_point &abs_time)
    {
        std::this_thread::sleep_until(abs_time);
    }

    template <class _Rep, class _Period>
    void spin_for(const std::chrono::duration<_Rep, _Period> &duration)
    {
        auto end = std::chrono::steady_clock::now() + duration;
        std::size_t spins = 0;
        while (std::chrono::steady_clock::now() < end) {
            ++spins;
            if ((spins & 0xFF) == 0) std::this_thread::yield();
        }
    }

    void spin_until(const std::chrono::steady_clock::time_point &abs_time)
    {
        auto end = abs_time;
        std::size_t spins = 0;
        while (std::chrono::steady_clock::now() < end) {
            ++spins;
            if ((spins & 0xFF) == 0) std::this_thread::yield();
        }
    }

} // namespace cpprun::time


#ifdef __MAIN__
#include <iostream>

using namespace std::chrono;

template <typename F>
double bench(F f, size_t iterations)
{
    // warmup
    for (size_t i = 0; i < 16; ++i) f();

    auto start = steady_clock::now();
    for (size_t i = 0; i < iterations; ++i) f();
    auto end = steady_clock::now();
    return duration_cast<duration<double, std::micro>>(end - start).count() / iterations; // microseconds
}

int main()
{
    const size_t iterations = 10;
    std::vector<milliseconds> durations = { milliseconds(0), milliseconds(1), milliseconds(5), milliseconds(10) };

    std::cout << "Benchmarks (average per call in microseconds)\n";
    for (auto d : durations) {
        std::cout << "--- duration: " << d.count() << " ms ---\n";

        // cpprun::time::wait_for
        auto w_for = [&]() { cpprun::time::wait_for(d); };
        double t_for = bench(w_for, iterations);
        std::cout << "wait_for:    " << t_for << " us\n";

        // cpprun::time::wait_until
        auto w_until = [&]() {
            auto tp = steady_clock::now() + d;
            cpprun::time::wait_until(tp);
        };
        double t_until = bench(w_until, iterations);
        std::cout << "wait_until:  " << t_until << " us\n";

        // cpprun::time::sleep_for
        auto s_for = [&]() { cpprun::time::sleep_for(d); };
        double ts_for = bench(s_for, iterations);
        std::cout << "sleep_for:   " << ts_for << " us\n";

        // cpprun::time::sleep_until
        auto s_until = [&]() {
            auto tp = steady_clock::now() + d;
            cpprun::time::sleep_until(tp);
        };
        double ts_until = bench(s_until, iterations);
        std::cout << "sleep_until: " << ts_until << " us\n";

        // cpprun::time::spin_for
        auto sp_for = [&]() { cpprun::time::spin_for(d); };
        double tsp_for = bench(sp_for, iterations);
        std::cout << "spin_for:    " << tsp_for << " us\n";

        // cpprun::time::spin_until
        auto sp_until = [&]() {
            auto tp = steady_clock::now() + d;
            cpprun::time::spin_until(tp);
        };
        double tsp_until = bench(sp_until, iterations);
        std::cout << "spin_until:  " << tsp_until << " us\n";

        std::cout << std::flush;
    }

    return 0;
}

#endif // __MAIN__