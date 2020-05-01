#include <chrono>
#include <iostream>
#include <ratio>
#include <sstream>
#include <locale>
#include <iomanip>
#include <cassert>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace pt = boost::posix_time;
using date = boost::gregorian::date;


/**
 * Transparent implementation of joaDateTime core functionality conserning OADate (OLE Automation)
 * for fun, see: https://social.msdn.microsoft.com/Forums/office/en-US/f1eef5fe-ef5e-4ab6-9d92-0998d3fa6e14/what-is-story-behind-december-30-1899-as-base-date
 */
struct DateTime {
	// Number of 100ns ticks per time unit
	static const long TicksPerMillisecond = 10000;
	static const long TicksPerSecond = TicksPerMillisecond * 1000;
	static const long TicksPerMinute = TicksPerSecond * 60;
	static const long TicksPerHour = TicksPerMinute * 60;
	static const long TicksPerDay = TicksPerHour * 24;

	// Number of milliseconds per time unit
	static const int MillisPerSecond = 1000;
	static const int MillisPerMinute = MillisPerSecond * 60;
	static const int MillisPerHour = MillisPerMinute * 60;
	static const int MillisPerDay = MillisPerHour * 24;

	// Number of days in a non-leap year
	static const int DaysPerYear = 365;
	// Number of days in 4 years
	static const int DaysPer4Years = DaysPerYear * 4 + 1;       // 1461
	// Number of days in 100 years
	static const int DaysPer100Years = DaysPer4Years * 25 - 1;  // 36524
	// Number of days in 400 years
	static const int DaysPer400Years = DaysPer100Years * 4 + 1; // 146097

	// Number of days from 1/1/0001 to 12/31/1600
	static const int DaysTo1601 = DaysPer400Years * 4;          // 584388
	// Number of days from 1/1/0001 to 12/30/1899
	static const int DaysTo1899 = DaysPer400Years * 4 + DaysPer100Years * 3 - 367;
	// Number of days from 1/1/0001 to 12/31/1969
	static const int DaysTo1970 = DaysPer400Years * 4 + DaysPer100Years * 3 + DaysPer4Years * 17 + DaysPerYear; // 719,162
	// Number of days from 1/1/0001 to 12/31/9999
	static const int DaysTo10000 = DaysPer400Years * 25 - 366;  // 3652059

	static const long MinTicks = 0;
	static const long MaxTicks = DaysTo10000 * TicksPerDay - 1;
	static const long MaxMillis = (long)DaysTo10000 * MillisPerDay;

	static const long FileTimeOffset = DaysTo1601 * TicksPerDay;
	static const long DoubleDateOffset = DaysTo1899 * TicksPerDay;
	// The minimum OA date is 0100/01/01 (Note it's year 100).
	// The maximum OA date is 9999/12/31
	static const long OADateMinAsTicks = (DaysPer100Years - DaysPerYear) * TicksPerDay;
	// All OA dates must be greater than (not >=) OADateMinAsDouble
	constexpr static const double OADateMinAsDouble = -657435.0;
	// All OA dates must be less than (not <=) OADateMaxAsDouble
	constexpr static const double OADateMaxAsDouble = 2958466.0;

	// Converts an OLE Date to a tick count.
	// This function is duplicated in COMDateTime.cpp
	static long OADateToTicks(double value) {
		// The check done this way will take care of NaN
		if (!(value < OADateMaxAsDouble) || !(value > OADateMinAsDouble))
			throw std::runtime_error("OADateToTicks(): Arg_OleAutDateInvalid");

		// Conversion to long will not cause an overflow here, as at this point the "value" is in between OADateMinAsDouble and OADateMaxAsDouble
		long millis = (long)(value * MillisPerDay + (value >= 0 ? 0.5: -0.5));
		// The interesting thing here is when you have a value like 12.5 it all positive 12 days and 12 hours from 01/01/1899
		// However if you a value of -12.25 it is minus 12 days but still positive 6 hours, almost as though you meant -11.75 all negative
		// This line below fixes up the millis in the negative case
		if (millis < 0) {
			millis -= (millis % MillisPerDay) * 2;
		}

		millis += DoubleDateOffset / TicksPerMillisecond;

		if (millis < 0 || millis >= MaxMillis) throw std::runtime_error("OADateToTicks(): Arg_OleAutDateScale");
		return millis * TicksPerMillisecond;
	}

	// This function is duplicated in COMDateTime.cpp
	static double TicksToOADate(long value) {
		if (value == 0)
			return 0.0;  // Returns OleAut's zero'ed date value.
		if (value < TicksPerDay) // This is a fix for VB. They want the default day to be 1/1/0001 rathar then 12/30/1899.
			value += DoubleDateOffset; // We could have moved this fix down but we would like to keep the bounds check.
		if (value < OADateMinAsTicks)
			throw std::out_of_range("TicksToOADate(): Arg_OleAutDateInvalid");
		// Currently, our max date == OA's max date (12/31/9999), so we don't
		// need an overflow check in that direction.
		long millis = (value  - DoubleDateOffset) / TicksPerMillisecond;
		if (millis < 0) {
			long frac = millis % MillisPerDay;
			if (frac != 0) millis -= (MillisPerDay + frac) * 2;
		}
		return (double)millis / MillisPerDay;
	}

	static long EpochToTicks(long sec) {
        auto t0 = DaysTo1970 * TicksPerDay;
        auto t1 = sec * TicksPerSecond;
        return t0 + t1;


    }

    static long TicksToEpoch(long ticks) {
        constexpr auto t0 = (DaysTo1970 * TicksPerDay) / TicksPerSecond;
        auto t1 = ticks / TicksPerSecond;
        return (t1 - t0);
    }

    static std::string EpochToString(long epoch) {
        std::time_t temp = epoch;
        std::tm* t = std::gmtime(&temp);
        std::stringstream ss; // or if you're going to print, just input directly into the output stream
        ss << std::put_time(t, "%Y-%m-%d %I:%M:%S %p");
        return ss.str();
    }

    static int64_t PosixTimeToEpoch(const pt::ptime& t) {
        static const auto epoch = pt::ptime(date {1970, 1, 1});
        return (t - epoch).total_seconds();
    }

    static pt::ptime EpochToPosixTime(int64_t value) {
        static const auto epoch = pt::ptime(date {1970, 1, 1});
        return epoch + pt::seconds(value);
    }

    /**
     * An OLE Automation date is implemented as a floating-point number whose integral component 
     * is the number of days before or after midnight, 30 December 1899, and whose fractional component 
     * represents the time on that day divided by 24. 
     * For example, 
     * midnight, 31 December 1899 is represented by 1.0; 
     * 6 A.M., 1 January 1900 is represented by 2.25; 
     * midnight, 29 December 1899 is represented by -1.0; and 
     * 6 A.M., 29 December 1899 is represented by -1.25.
     */
    static pt::ptime OADateToPosixTime(double value) {
        return EpochToPosixTime(TicksToEpoch(OADateToTicks(value)));
    }

    static double PosixTimeToOADate(const pt::ptime& value) {
        return TicksToOADate(EpochToTicks(PosixTimeToEpoch(value)));
    }
};

void test_fixed_timepoints() {
    /**
     * https://docs.microsoft.com/en-us/dotnet/api/system.datetime.tooadate?view=netframework-4.8
     */
    pt::ptime de = pt::ptime(date { 1899, 12, 30 }); //ticks: 599264352000000000
    pt::ptime ue = pt::ptime(date { 1970, 1, 1 });
    assert(DateTime::PosixTimeToEpoch(ue) == 0);
    assert(DateTime::PosixTimeToOADate(de) == 0.0);

    //midnight, 31 December 1899 is represented by 1.0; 
    pt::ptime _1899_12_31_00_00_00 = pt::ptime(date { 1899, 12, 31 } );
    assert(DateTime::PosixTimeToOADate(_1899_12_31_00_00_00) == 1.0);
    // 6 A.M., 1 January 1900 is represented by 2.25; 
    pt::ptime _1900_01_01_06_00_00 = pt::ptime(pt::from_iso_string("19000101T060000"));
    assert(DateTime::PosixTimeToOADate(_1900_01_01_06_00_00) == 2.25);
    //midnight, 29 December 1899 is represented by -1.0; and 
    pt::ptime _1899_12_29_00_00_00 = pt::ptime(date { 1899, 12, 29 } );
    assert(DateTime::PosixTimeToOADate(_1899_12_29_00_00_00) == -1.0);
    //6 A.M., 29 December 1899 is represented by -1.25.
    pt::ptime _1899_12_29_06_00_00 = pt::ptime(pt::from_iso_string("18991229T060000"));
    assert(DateTime::PosixTimeToOADate(_1899_12_29_06_00_00) == -1.25);

}

int main(int argc, char* argv[]) {
    test_fixed_timepoints();

    //pt::ptime t = pt::ptime(date { 1970, 1, 1 });
    pt::ptime t = pt::ptime(date { 1899, 12, 30 });
    pt::ptime t1 = pt::ptime(date { 1970, 1, 1 });

    std::cout << DateTime::PosixTimeToEpoch(t1) << std::endl;
    std::cout << DateTime::PosixTimeToOADate(t1) << std::endl;


    auto epoch = DateTime::PosixTimeToEpoch(t);


    auto ticks = DateTime::EpochToTicks(epoch);
    auto oadate = DateTime::TicksToOADate(ticks);

    std::cout << "date:   " << DateTime::EpochToString(epoch) << std::endl;
    std::cout << "epoch:  " << epoch << std::endl;
    std::cout << "ticks:  " << ticks << std::endl;
    std::cout << "oadate: " << std::fixed << oadate << std::endl;

    auto test_oadate = DateTime::OADateToTicks(oadate);
    auto test_epoch = DateTime::TicksToEpoch(test_oadate);
    
    std::cout << "ticks:  " << std::fixed << test_oadate << std::endl;
    std::cout << "epoch:  " << test_epoch << std::endl;

    assert(test_oadate == ticks);
    assert(test_epoch == epoch);
    

    return 0;
}

