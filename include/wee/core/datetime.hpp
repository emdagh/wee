#pragma once

#include <chrono>
#include <cstring>
#include <ctime>
#include <cmath>

namespace wee {
    using days = std::chrono::duration<double, std::ratio_multiply<std::ratio<24>, std::chrono::hours::period> >;
    using ticks = std::chrono::duration<int64_t, std::ratio_multiply<std::ratio<100>, std::nano> >;

    template <typename Clock = std::chrono::system_clock>
    typename Clock::time_point gregorian_date(int y, int mo, int d, int h = 0, int m = 0, int s = 0) {
        std::tm tm;
        std::memset(&tm, 0, sizeof(std::tm));
        tm.tm_year = y - 1900;
        tm.tm_mon  = mo - 1;
        tm.tm_mday = d;
        tm.tm_hour = h;
        tm.tm_min  = m;
        tm.tm_sec  = s;

        std::time_t tt = mktime(&tm);
        return Clock::from_time_t(tt);
    }

    template <typename Clock = std::chrono::system_clock>
    static double to_oadate(const typename Clock::time_point& tp) {
        using time_point = typename Clock::time_point;
        
        static time_point epoch = gregorian_date<Clock>(1899, 12, 30);
        auto dif = std::chrono::duration_cast<days>(tp - epoch).count();
        double i;
        if(auto f = std::modf(dif, &i); f < 0.0) {
            return -(1.0 + i + (1.0 - std::fabs(f)));
        }
        return dif;
    }

    template <typename Clock = std::chrono::system_clock>
    static typename Clock::time_point to_time_point(double d) {
        static const auto e0 = gregorian_date<Clock>(1899, 12, 30);
        static const auto e1 = gregorian_date<Clock>(1970, 1, 1);
        static const auto de = std::chrono::duration_cast<std::chrono::seconds>(e1 - e0).count();

        days dur(d);
        auto dif = std::chrono::round<std::chrono::seconds>(dur).count();
        if(dif < 0) {
            dif += 2 * fabs(fmod(dif, 86400));
        }
        return e1 + std::chrono::seconds(dif - de);
    }
}