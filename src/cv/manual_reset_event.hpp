// Copyright(c) 2026-present, 康文君 & cpprun contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>


namespace cpprun {

class manual_reset_event
{
public:
    manual_reset_event() = default;
    manual_reset_event(const manual_reset_event &) = delete;
    manual_reset_event(manual_reset_event &&) = delete;
    manual_reset_event &operator=(const manual_reset_event &) = delete;
    manual_reset_event &operator=(manual_reset_event &&) = delete;

    void reset()
    {
        _notified.store(false, std::memory_order_release);
    }

    void wait()
    {
        bool expected = true;
        if (_notified.compare_exchange_strong(expected, false,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire))
        {
            return;
        }

        std::unique_lock<std::mutex> locker(_mutex);
        _cv.wait(locker, [this] { return this->_notified.load(std::memory_order_acquire); });
    }

    template <class _Rep, class _Period>
    bool wait_for(const std::chrono::duration<_Rep, _Period> &duration)
    {
        bool expected = true;
        if (_notified.compare_exchange_strong(expected, false,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire))
        {
            return true;
        }

        std::unique_lock<std::mutex> locker(_mutex);
        bool ret = _cv.wait_for(locker, duration, [this]{ return _notified.load(std::memory_order_acquire); });
        return ret;
    }

    bool wait_until(const std::chrono::steady_clock::time_point &abs_time)
    {
        bool expected = true;
        if (_notified.compare_exchange_strong(expected, false,
                                              std::memory_order_acq_rel,
                                              std::memory_order_acquire))
        {
            return true;
        }

        std::unique_lock<std::mutex> locker(_mutex);
        bool ret = _cv.wait_until(locker, abs_time, [this]{ return _notified.load(std::memory_order_acquire); });
        return ret;
    }

    void notify_one()
    {
        _notified.store(true, std::memory_order_release);
        _cv.notify_one();
    }

    void notify_all()
    {
        _notified.store(true, std::memory_order_release);
        _cv.notify_all();
    }

private:
    std::mutex _mutex;
    std::condition_variable _cv;
    std::atomic<bool> _notified{ false };
};

} // namespace cpprun


#ifdef __MAIN__

#include <iostream>
#include <thread>

int main()
{
    using namespace cpprun;

    manual_reset_event event;

    std::thread t1([&event] {
        std::cout << "Thread 1 waiting..." << std::endl;
        event.wait();
        std::cout << "Thread 1 received signal!" << std::endl;
    });

    std::thread t2([&event] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Thread 2 setting event..." << std::endl;
        event.notify_one();
    });

    t1.join();
    t2.join();

    return 0;
}

#endif // __MAIN__