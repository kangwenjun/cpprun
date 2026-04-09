#pragma once

#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>


namespace cpprun {

class auto_reset_event
{
public:
    auto_reset_event() = default;
    auto_reset_event(const auto_reset_event &) = delete;
    auto_reset_event(auto_reset_event &&) = delete;
    auto_reset_event &operator=(const auto_reset_event &) = delete;
    auto_reset_event &operator=(auto_reset_event &&) = delete;

    void wait()
    {
        std::unique_lock<std::mutex> locker(_mutex);
        _cv.wait(locker, [this] { return this->_notified; });
        _notified = false;
    }

    template <class _Rep, class _Period>
    bool wait_for(const std::chrono::duration<_Rep, _Period> &duration)
    {
        std::unique_lock<std::mutex> locker(_mutex);
        bool result = _cv.wait_for(locker, duration, [this]{ return _notified; });
        if (result) _notified = false;
        return result;
    }

    bool wait_until(const std::chrono::steady_clock::time_point &abs_time)
    {
        std::unique_lock<std::mutex> locker(_mutex);
        bool result = _cv.wait_until(locker, abs_time, [this]{ return _notified; });
        if (result) _notified = false;
        return result;
    }

    void set() noexcept
    {
        {
            std::lock_guard<std::mutex> locker(_mutex);
            _notified = true;
        }
        _cv.notify_one();
    }

private:
    std::mutex _mutex;
    std::condition_variable _cv;
    bool _notified{ false };
};

} // namespace cpprun


#ifdef __MAIN__

#include <iostream>
#include <thread>

int main()
{
    using namespace cpprun;

    auto_reset_event event;

    std::thread t1([&event] {
        std::cout << "Thread 1 waiting..." << std::endl;
        event.wait();
        std::cout << "Thread 1 received signal!" << std::endl;
    });

    std::thread t2([&event] {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        std::cout << "Thread 2 setting event..." << std::endl;
        event.set();
    });

    t1.join();
    t2.join();

    return 0;
}

#endif // __MAIN__