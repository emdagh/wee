#include <core/datetime.hpp>
#include <core/tinytest.hpp>
#include <random>
#include <iostream>

using namespace wee;
/**
 * An OLE Automation date is implemented as a floating-point number whose integral component is the 
 * number of days before or after midnight, 30 December 1899, and whose fractional component represents 
 * the time on that day divided by 24. For example, 
 * midnight, 31 December 1899 is represented by 1.0; 
 * 6 A.M., 1 January 1900 is represented by 2.25; 
 * midnight, 29 December 1899 is represented by -1.0; and 
 * 6 A.M., 29 December 1899 is represented by -1.25.
 */
template <typename T, typename Gen>
auto date_selector(const T& start, const T& end, Gen&& gen) {
    std::chrono::seconds secs = std::chrono::duration_cast<std::chrono::seconds>(end - start);
    return [=] () mutable {
        std::uniform_int_distribution<std::chrono::seconds::rep> d(0, secs.count());
        return start + std::chrono::seconds(d(gen));
    };
}

int main(int, char**) {
    suite("testing known OLE automation dates") {
        auto t1 = wee::gregorian_date(1899, 12, 30);
        test(to_oadate(t1) == 0.0);
        auto t2 = wee::gregorian_date(1899, 12, 29, 6, 0, 0);
        test(to_oadate(t2) == -1.25);
        auto t3 = wee::gregorian_date(1900, 1, 1, 6, 0, 0);
        test(to_oadate(t3) == 2.25);
        auto t4 = wee::gregorian_date(1899, 12, 29);
        test(to_oadate(t4) == -1.0);

        test(to_time_point( 0.0)  == t1);
        test(to_time_point(-1.25) == t2);
        test(to_time_point( 2.25) == t3);
        test(to_time_point(-1.0)  == t4);
    }
    suite("testing random OLE automation dates") {

        static std::random_device rd;
        static std::mt19937 gen(rd());

        auto select_random_date = date_selector(
            std::chrono::system_clock::now(), 
            std::chrono::system_clock::now() + std::chrono::hours(24 * 365 * 10),
            gen
        );
        for(int i=0; i < 100; i++) {
            auto p = select_random_date();
            auto q = wee::to_time_point(wee::to_oadate(p));
            test(std::chrono::duration_cast<std::chrono::minutes>(p - q).count() == 0);
            test(std::chrono::duration_cast<std::chrono::seconds>(p - q).count() == 0);
            test(!std::chrono::duration_cast<std::chrono::milliseconds>(p - q).count() == 0);
        }

    }
    return 0;
}