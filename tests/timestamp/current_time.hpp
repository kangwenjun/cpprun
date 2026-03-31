#ifndef CURRENT_TIME_HPP_
#define CURRENT_TIME_HPP_

#include <chrono>
#include <ctime>
#include <type_traits>

class current_time
{
public:
    struct TIME
    {
        int hours;
        int minutes;
        int seconds;

        int milliseconds;
    };

    struct DATE_TIME
    {
        int year;
        int month;
        int day;

        int hours;
        int minutes;
        int seconds;

        int milliseconds;
    };

    static void transform_time(struct tm &dst, time_t &src)
    {
#ifdef _WIN32
        localtime_s(&dst, &src);
#else
        localtime_r(&src, &dst);
#endif
    }

    template <typename T>
    static void set_current_time(T &data)
    {
        struct tm local_time;
        auto now = std::chrono::system_clock::now();
        auto t = std::chrono::system_clock::to_time_t(now);
        transform_time(local_time, t);

        if constexpr (std::is_same_v<T, DATE_TIME>)
        {
            data.year = local_time.tm_year + 1900;
            data.month = local_time.tm_mon + 1;
            data.day = local_time.tm_mday;
        }

        data.hours = local_time.tm_hour;
        data.minutes = local_time.tm_min;
        data.seconds = local_time.tm_sec;
        auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        data.milliseconds = static_cast<int>(milliseconds_since_epoch % 1000);
    }

};

#endif // CURRENT_TIME_HPP_

#ifdef __MAIN__
#include <iostream>

int main()
{
    current_time::TIME t;
    current_time::set_current_time(t);

    std::cout << "Current Time: "
              << t.hours << ":"
              << t.minutes << ":"
              << t.seconds << "."
              << t.milliseconds << std::endl;

    current_time::DATE_TIME tt;
    current_time::set_current_time(tt);

    std::cout << "Current Date and Time: "
              << tt.year << "-"
              << tt.month << "-"
              << tt.day << " "
              << tt.hours << ":"
              << tt.minutes << ":"
              << tt.seconds << "."
              << tt.milliseconds << std::endl;

    return 0;
}
#endif // __MAIN__
