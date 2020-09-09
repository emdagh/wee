#include <limits>
#include <chrono>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <core/datetime.hpp>
#include <core/tinytest.hpp>

using namespace wee;

int main(int, char**) {
    using namespace std::chrono;
    //std::cout << chrono_time(42298.6868055556) << std::endl; // 2015-10-21T16:29:00.000Z

    double offset = 42298.6868055556;

    auto t1 = make_time_point(1899,12,28,6,0,0);
    auto t2 = make_time_point(2015,10,21,16,29,0);

    std::cout << std::setprecision(16);

    std::cout << to_time_point(offset) << std::endl;
    std::cout << to_time_point(-1.25) << std::endl;
    std::cout << t1 << "=" << wee::to_oadate(t1) << std::endl;
    std::cout << t2 << "=" << wee::to_oadate(t2) << std::endl;
    std::cout << wee::to_oadate(to_time_point(-1.25)) << std::endl;

    return 0;
}
