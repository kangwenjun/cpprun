#include <iostream>
#include "current_time.hpp"

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
