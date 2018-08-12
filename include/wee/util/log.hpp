#pragma once

#include <sstream>
#include <typeinfo>
#include <cxxabi.h>
#include <iterator>
#include <iostream>
#include <iomanip>
#include <date.h>

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

    enum class loglevel : uint8_t {
        quiet = 0,
        error,
        warn,
        info,
        all,
        none
    };

    class logstream : public std::ostringstream {
        std::ostream& _out;
        std::string _dtstring;
        std::string _context;
        loglevel _curlevel;
        loglevel _loglevel;
        static std::size_t _depth;
    public:
        logstream(std::ostream& out, const std::string& context, loglevel logl=loglevel::all) 
        : _out(out)
        , _dtstring("%F %T) //%Y%m%d %H:%M:%S") 
        , _context(context)
        , _curlevel(loglevel::none)
        , _loglevel(logl)
        {
            _out << logline((RED "+" RESET) + _context) << std::endl;
            _depth++;
        }

        virtual ~logstream() {
            flush();
            _depth--;
            _out << logline((RED "-" RESET) + _context) << std::endl;
        }

        static std::string logline(const std::string& what) {
            std::stringstream os;
            os << MAGENTA << date::format("%F %T", std::chrono::system_clock::now()) << RESET << " ";
            os << std::string(_depth, ' ');
            //os << std::setfill(' ') << std::setw(1) << " ";
            os << what;
            return os.str();
            //os.flush();
        }

        void flush() {
            if(_curlevel <= _loglevel) {
                _out << logline(str());
            }
            str("");
            _curlevel = loglevel::none;
        }

        template <typename T>
        inline logstream& operator << (const T& t)
        {
            (*(std::ostringstream*)this) << t;
            return *this;
        }

        
        logstream& operator << (const loglevel& l) {
            _curlevel = l;
            return *this;
        }

        typedef logstream & (*logstream_manip)(logstream &);
        logstream & operator<<(logstream_manip manip) { return manip(*this); }
    };

    template <typename R>
    static std::string value_of(const std::string& name, const R& val, bool print_type=false) {
        std::stringstream ss;
        ss << RESET << name;
        if(print_type) {
            int status = 0;
            auto ti = typeid(val).name();
            auto realname = abi::__cxa_demangle(ti, 0, 0, &status);
            ss << " (" << WHITE << realname << RESET << ")";
        }
        ss << "=[" << BLUE << val << RESET << "]";
        return ss.str();
    }

    std::size_t logstream::_depth = 0;

    //logstream& operator << (logstream& out, logstream::level lvl);
}
namespace std { 
    io::logstream & endl(io::logstream & out) { 
        out.put('\n'); 
        out.flush(); 
        return out; 
    } 
}

#define DEBUG_METHOD()              io::logstream __logstream(std::cout, __FUNCTION__) //; DEBUG_VALUE_OF(__FUNCTION__)
#define DEBUG_VALUE_OF(x)           __logstream << io::loglevel::info << io::value_of(#x, x, false) << std::endl
#define DEBUG_VALUE_AND_TYPE_OF(x)  __logstream << io::loglevel::info << io::value_of(#x, x, true) << std::endl
