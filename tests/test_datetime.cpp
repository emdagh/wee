#include <core/datetime.hpp>
#include <core/tinytest.hpp>

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

int main(int, char**) {
    suite("testing OLE automation dates") {
        auto t1 = wee::gregorian_date(1899, 12, 30);
        test(to_oadate(t1) == 0.0);
        auto t2 = wee::gregorian_date(1899, 12, 29, 6, 0, 0);
        test(to_oadate(t2) == -1.25);
        auto t3 = wee::gregorian_date(1900, 1, 1, 6, 0, 0);
        test(to_oadate(t3) == 2.25);
        auto t4 = wee::gregorian_date(1899, 12, 29);
        test(to_oadate(t4) == -1.0);
        
    }
    return 0;
}