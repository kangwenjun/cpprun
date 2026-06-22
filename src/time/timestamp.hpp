// Copyright(c) 2026-present, 康文君 & cpprun contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <chrono>
// timestamp.hpp - utilities for time points, formatting and parsing

#pragma once

#include <chrono>
#include <ctime>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>


namespace cpprun::time
{

class Timestamp
{
public:
    using clock = std::chrono::system_clock;
    using time_point = clock::time_point;

    static time_point now() { return clock::now(); }

    template <typename Duration = std::chrono::milliseconds>
    static typename Duration::rep now_since_epoch()
    {
        // 1970-01-01 00:00:00 UTC is the epoch, so we get the duration since then and convert to desired units.
        return std::chrono::duration_cast<Duration>(clock::now().time_since_epoch()).count();
    }
    static long long now_ns() { return now_since_epoch<std::chrono::nanoseconds>(); }
    static long long now_us() { return now_since_epoch<std::chrono::microseconds>(); }
    static long long now_ms() { return now_since_epoch<std::chrono::milliseconds>(); }
    static long long now_s() { return now_since_epoch<std::chrono::seconds>(); }
    static long long now_minutes() { return now_since_epoch<std::chrono::minutes>(); }
    static long long now_hours() { return now_since_epoch<std::chrono::hours>(); }
#if _HAS_CXX20
    static long long now_days() { return now_since_epoch<std::chrono::days>(); }
    static long long now_weeks() { return now_since_epoch<std::chrono::weeks>(); }
    static long long now_years() { return now_since_epoch<std::chrono::years>(); }
#endif

    static time_point from_time_t(time_t t) { return clock::from_time_t(t); }
    static time_t to_time_t(time_point tp) { return clock::to_time_t(tp); }

    static time_t utc() { return to_time_t(now()); }
    static time_t utc(time_point tp) { return to_time_t(tp); }

    // Format a time_point to string using strftime-style format.
    // If utc_flag==true uses UTC/GMT, otherwise localtime.
    static std::string to_string(time_point tp, const std::string &format = "%Y-%m-%d %H:%M:%S", bool utc_flag = false)
    {
        time_t t = to_time_t(tp);
        struct tm tm_buf;

#ifdef _WIN32
        if (utc_flag)
            gmtime_s(&tm_buf, &t);
        else
            localtime_s(&tm_buf, &t);
#else
        if (utc_flag)
            gmtime_r(&t, &tm_buf);
        else
            localtime_r(&t, &tm_buf);
#endif

        char buf[128];
        if (std::strftime(buf, sizeof(buf), format.c_str(), &tm_buf) == 0)
            return std::string();
        return std::string(buf);
    }

    // Parse a time string according to format into time_point (interpreted as local time)
    // Returns a time_point at epoch -1 on failure.
    static time_point parse(const std::string &time_str, const std::string &format = "%Y-%m-%d %H:%M:%S")
    {
        struct tm tm_time;
        std::memset(&tm_time, 0, sizeof(tm_time));
        std::istringstream ss(time_str);
        ss >> std::get_time(&tm_time, format.c_str());
        if (ss.fail())
            return time_point::min();

        time_t tt = std::mktime(&tm_time); // interprets tm as local time
        if (tt == (time_t)-1)
            return time_point::min();
        return from_time_t(tt);
    }

    // Parse a time string interpreted as UTC/GMT into time_point.
    // Uses platform-specific _mkgmtime / timegm when available.
    static time_point parse_utc(const std::string &time_str, const std::string &format = "%Y-%m-%d %H:%M:%S")
    {
        struct tm tm_time;
        std::memset(&tm_time, 0, sizeof(tm_time));
        std::istringstream ss(time_str);
        ss >> std::get_time(&tm_time, format.c_str());
        if (ss.fail())
            return time_point::min();

        time_t tt = (time_t)-1;
#ifdef _WIN32
        tt = _mkgmtime(&tm_time);
#else
        tt = timegm(&tm_time);
#endif
        if (tt == (time_t)-1)
            return time_point::min();
        return from_time_t(tt);
    }

    // Return a `struct tm` in local time for given time_t
    static struct tm to_local_tm(time_t t)
    {
        struct tm out;
#ifdef _WIN32
        localtime_s(&out, &t);
#else
        localtime_r(&t, &out);
#endif
        return out;
    }

    struct T_TIME_DATA
    {
        int year;
        int month;
        int day;
        int hour;
        int minute;
        int second;
        int millisecond;
        int microsecond;
        int nanosecond;
    };

    static T_TIME_DATA to_time_data(time_point tp = now()) 
    {
        auto tse = tp.time_since_epoch();
        auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(tse).count();
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(tse).count();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tse).count();

        auto tm = to_local_tm(to_time_t(tp));
        struct T_TIME_DATA td = {0};
        td.year = tm.tm_year + 1900;
        td.month = tm.tm_mon + 1;
        td.day = tm.tm_mday;
        td.hour = tm.tm_hour;
        td.minute = tm.tm_min;
        td.second = tm.tm_sec;
        td.millisecond = ms % 1000;
        td.microsecond = us % 1000;
        td.nanosecond = ns % 1000;
        return td;
    }
};

} // namespace cpprun::time


// Small test main when compiled with -D__MAIN__
#ifdef __MAIN__
#include <iostream>

int main()
{
    using namespace cpprun::time;
    
    std::cout << "utc(): " << Timestamp::utc() << std::endl;
    
    std::cout << "now() ns since epoch: \t" << Timestamp::now_ns() << std::endl;
    std::cout << "now() us since epoch: \t" << Timestamp::now_us() << std::endl;
    std::cout << "now() ms since epoch: \t" << Timestamp::now_ms() << std::endl;
    std::cout << "now() s since epoch: \t" << Timestamp::now_s() << std::endl;

    auto tp = Timestamp::now();
    std::cout << "Formatted local: " << Timestamp::to_string(tp) << std::endl;
    std::cout << "Formatted UTC:   " << Timestamp::to_string(tp, "%Y-%m-%d %H:%M:%S", true) << std::endl;
    auto parsed = Timestamp::parse("2024-01-02 03:04:05");
    if (parsed != Timestamp::time_point::min())
        std::cout << "Parsed local -> time_t: " << Timestamp::utc(parsed) << std::endl;
    return 0;
}
#endif // __MAIN__
