#pragma once

#include <chrono>
#include <cstring>
#include <ctime>
#include <cmath>
#include <random>
#include <cassert>
#include <iostream>
#include <iomanip>

#if defined(MSC_VER)
#define timegm _mkgmtime
#endif

namespace wee {

    using days = std::chrono::duration<double, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period>>;
    using dhours = std::chrono::duration<double, std::ratio<3600>>;

    template <typename Clock, typename Duration>
    std::ostream& operator << (std::ostream& os, const std::chrono::time_point<Clock, Duration>& tp) 
    {
        auto tp2 = std::chrono::time_point_cast<std::chrono::system_clock::duration, Clock, Duration>(tp);
        std::time_t tc = Clock::to_time_t(tp2);
        std::tm* tm = std::gmtime(&tc);
        return os << std::put_time(tm, "%FT%T");
    }


    template <typename Clock = std::chrono::system_clock>
    auto make_time_point(int year, int mon, int mday, int hour = 0, int min = 0, int sec = 0) {
        std::tm tm;
        memset(&tm, 0, sizeof(std::tm));
        tm.tm_year = year - 1900;
        tm.tm_mon  = mon - 1;
        tm.tm_mday = mday;
        tm.tm_hour = hour;
        tm.tm_min  = min;
        tm.tm_sec  = sec;
        return Clock::from_time_t(timegm(&tm));
    }

    template <typename Clock = std::chrono::system_clock>
    auto from_oadate(double offset)
    {
        double i;
        double f = std::modf(offset, &i);
        return make_time_point(1899, 12, 30) + days(i) + days(std::fabs(f));
    }

    template <typename Clock, typename Duration> 
    double to_oadate(const std::chrono::time_point<Clock, Duration>& tp) 
    {
        auto offset = std::chrono::duration_cast<days>(tp - make_time_point<Clock>(1899, 12, 30)).count();
        double i;
        double f = std::modf(offset, &i);
        if(offset < 0.0) {
            return -(fabs(i) + (1.0 - fabs(f))) - 1.0;
        }
        return offset;//copysign(1.0, i) * (fabs(i) + (1.0 - fabs(f)));
    }

    template <typename T, typename Gen>
    auto date_selector(const T& start, const T& end, Gen&& gen) {
        std::chrono::seconds secs = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        return [=] () mutable {
            std::uniform_int_distribution<std::chrono::seconds::rep> d(0, secs.count());
            return start + std::chrono::seconds(d(gen));
        };
    }

}
