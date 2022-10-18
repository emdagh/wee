#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

namespace wee {
    // ref: https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf?msclkid=06363103cf7511eca4c79d1b04d156b9
    template<typename ... Args>
    std::string string_format( const std::string& format, Args&& ... args )
    {
        auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
        std::string output(size + 1, '\0');
        std::sprintf(&output[0], format.c_str(), std::forward<Args>(args)...);
        return output;
    }
    
    template <typename InputIt>
        std::string join(InputIt begin, InputIt end, const char* delim) {
            std::ostringstream ss;
            std::copy(begin, end, std::ostream_iterator<std::string>(ss, delim));
            return ss.str();
        }

    template <typename OutputIt>
        inline void split(const std::string& s, char delim, OutputIt it) {
            std::istringstream ss(s);
            std::string token;
            while(std::getline(ss, token, delim)) {
                *(it++) = token;
            }
        }

    template <typename OutputIt>
        static inline void split(const std::string& in, const std::string& delim, OutputIt it)
        {
            size_t start = 0, end = 0;
            while((end = in.find_first_of(delim, start)) != std::string::npos)
            {
                *it++ = in.substr(start, end - start);
                start = end + delim.length();
            } 
            *it++ = in.substr(start);
        }

    static inline std::vector<std::string> split(const std::string& in, const std::string& delim)
    {
        std::vector<std::string> res;
        split(in, delim, std::back_inserter(res));
        return res;
    }

    // trim from start (in place)
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                    }));
    }

    // trim from end (in place)
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                    return !std::isspace(ch);
                    }).base(), s.end());
    }

    // trim from both ends (in place)
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    // trim from start (copying)
    static inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    // trim from end (copying)
    static inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    // trim from both ends (copying)
    static inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }

    template <typename T>
        T explode(const std::string& s, const char d) {
            T res;
            split(s, d, std::back_inserter(res));
        }

    template <typename T>
        std::string implode(char d, const T& t) {
            return join(std::begin(t), std::end(t), d);
        }
}
