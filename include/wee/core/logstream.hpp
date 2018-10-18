#pragma once

#include <sstream>
#include <typeinfo>
//#include <cxxabi.h>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <date/date.h>
#include <fmt/format.h>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

namespace wee {

    template <typename R>
    std::string value_of(const std::string& name, const R& val, bool print_type=false) {
        std::stringstream ss;
        ss << RESET << name;
        if(print_type) {
            //int status = 0;
            auto ti = typeid(val).name();
            //auto realname = abi::__cxa_demangle(ti, 0, 0, &status);
            ss << " (" << WHITE << ti << RESET << ")";
        }
        ss << "=[" << BLUE << val << RESET << "]";
        return ss.str();
    }

    template <typename T>
    void logline(std::ostream& os, const T& what) {
        os << what;
    }


    struct log{
        std::ostream& _os;
        
        log(std::ostream& os, const std::string& context) 
        : _os(os)
        {
            std::string _context = context;
            os << MAGENTA << date::format("%F %T", std::chrono::system_clock::now()) << RESET << " ";
            os << RED "> " << RESET;
        }

        virtual ~log() {
            _os << std::endl;
        }


        template <typename T, typename... Args>
        void write(const T& t, const Args&... rest) {
            logline(_os, t);
            _os << " ";
            write(rest...);//std::forward<Args>(rest)...);
        }
        template <typename T>
        void write(const T& t) {
            logline(_os, t);
        }
        void write() {
        }



    };
}
#define DEBUG_METHOD()              wee::log(std::cout, __FUNCTION__).write(__FUNCTION__) 
#define DEBUG_LOG(...)              wee::log(std::cout, __FUNCTION__).write(fmt::format(__VA_ARGS__))
#define DEBUG_VALUE_OF(x)           wee::log(std::cout, __FUNCTION__).write(wee::value_of(#x, x, false))
#define DEBUG_ERROR(...)            wee::log(std::cout, __FUNCTION__).write(RED + fmt::format(__VA_ARGS__) + RESET)
#define DEBUG_VALUE_AND_TYPE_OF(x)  wee::log(std::cout, __FUNCTION__).write(wee::value_of(#x, x, true))
#define TRACE(...)                  wee::log(std::cout, __FILE__##":"##__LINE__).write(__VA_ARGS__)


/*
#define DEBUG_METHOD()              wee::logstream __logstream(std::cout, __FUNCTION__) //; DEBUG_VALUE_OF(__FUNCTION__)
#define DEBUG_VALUE_OF(x)           __logstream << wee::loglevel::debug << wee::value_of(#x, x, false) << std::endl
#define DEBUG_VALUE_AND_TYPE_OF(x)  __logstream << wee::loglevel::debug << wee::value_of(#x, x, true) << std::endl
#define DEBUG_LOG(...)              __logstream << wee::loglevel::info << __VA_ARGS__ << std::endl;*/
