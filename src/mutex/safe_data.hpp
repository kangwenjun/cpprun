// Copyright(c) 2026-present, 康文君 & cpprun contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <shared_mutex>
#include <utility>

template<typename T>
class safe_data
{
public:
    template<typename... Args>
    safe_data(Args&&... args) : data(std::forward<Args>(args)...)
    {
    }

    void set(const T& v)
    {
        std::unique_lock<std::shared_mutex> lk(mtx);
        data = v;
    }
    
    void set(T&& v)
    {
        std::unique_lock<std::shared_mutex> lk(mtx);
        data = std::move(v);
    }

    T get() const
    {
        std::shared_lock<std::shared_mutex> lk(mtx);
        return data;
    }

    template<typename F>
    auto with_lock(F&& f) -> decltype(std::declval<F>()(std::declval<T&>()))
    {
        std::unique_lock<std::shared_mutex> lk(mtx);
        return f(data);
    }

private:
    T data;
    mutable std::shared_mutex mtx;
};

#ifdef __MAIN__
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

int main()
{
    safe_data<int> sd(0);

    const int writers = 4;
    const int readers = 4;
    const int ops_per_thread = 10000;

    std::atomic<int> reader_checks{0};

    auto writer = [&](int id){
        for (int i = 0; i < ops_per_thread; ++i) {
            sd.set(id * ops_per_thread + i);
        }
    };

    auto reader = [&]() {
        for (int i = 0; i < ops_per_thread; ++i) {
            volatile int v = sd.get(); (void)v;
            ++reader_checks;
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(writers + readers);

    for (int i = 0; i < writers; ++i)
        threads.emplace_back(writer, i);
    for (int i = 0; i < readers; ++i)
        threads.emplace_back(reader);

    for (auto &t : threads) t.join();

    std::cout << "safe_data threaded test completed. ";
    std::cout << "final value=" << sd.get() << ", reader checks=" << reader_checks << '\n';

    return 0;
}

#endif // __MAIN__