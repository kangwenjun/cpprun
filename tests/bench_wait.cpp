#include <iostream>
#include <chrono>
#include <functional>
#include <vector>
#include <numeric>
#include <thread>

#include "../src/wait.hpp"

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

        // cpprun::wait_for
        auto w_for = [&]() { cpprun::wait_for(d); };
        double t_for = bench(w_for, iterations);
        std::cout << "wait_for:    " << t_for << " us\n";

        // cpprun::wait_until
        auto w_until = [&]() {
            auto tp = steady_clock::now() + d;
            cpprun::wait_until(tp);
        };
        double t_until = bench(w_until, iterations);
        std::cout << "wait_until:  " << t_until << " us\n";

        // sleep_for
        auto s_for = [&]() { cpprun::sleep_for(d); };
        double ts_for = bench(s_for, iterations);
        std::cout << "sleep_for:   " << ts_for << " us\n";

        // sleep_until
        auto s_until = [&]() {
            auto tp = steady_clock::now() + d;
            cpprun::sleep_until(tp);
        };
        double ts_until = bench(s_until, iterations);
        std::cout << "sleep_until: " << ts_until << " us\n";

        // spin_for
        auto sp_for = [&]() { cpprun::spin_for(d); };
        double tsp_for = bench(sp_for, iterations);
        std::cout << "spin_for:    " << tsp_for << " us\n";

        // spin_until
        auto sp_until = [&]() {
            auto tp = steady_clock::now() + d;
            cpprun::spin_until(tp);
        };
        double tsp_until = bench(sp_until, iterations);
        std::cout << "spin_until:  " << tsp_until << " us\n";

        std::cout << std::flush;
    }

    return 0;
}
