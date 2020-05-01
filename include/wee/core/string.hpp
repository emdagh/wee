#pragma once

#include <string>
#include <sstream>

namespace wee {
    template <typename InputIt>
    std::string join(InputIt begin, InputIt end, const char* delim) {
        std::ostringstream ss;
        std::copy(begin, end, std::ostream_iterator<std::string>(ss, delim));
        return ss.str();
    }

    template <typename OutputIt>
    void split(const std::string& s, char delim, OutputIt it) {
        std::istringstream ss(s);
        std::string token;
        while(std::getline(ss, token, delim)) {
            *(it++) = token;
        }
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
