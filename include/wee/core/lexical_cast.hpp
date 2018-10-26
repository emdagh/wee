#pragma once

#include <sstream>

namespace wee {

    template <typename T>
    std::string lexical_cast(const T& val) {
        std::ostringstream os;
        os << val;
        return os.str();
    }

    template <typename T>
    T lexical_cast(const std::string& str) {
        T res;
        std::istringstream is;
        is.str(str);
        is >> res;
        return res;
    }

    
}
