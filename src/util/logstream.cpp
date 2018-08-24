#include <util/logstream.hpp>

using namespace wee;

std::size_t logstream::_depth = 0;

namespace std { 
    wee::logstream & endl(wee::logstream & out) { 
        out.put('\n'); 
        out.flush(); 
        return out; 
    } 
}
