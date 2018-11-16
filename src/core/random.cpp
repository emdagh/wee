#include <core/random.hpp>

namespace wee {

float randf(float a , float b ) {
    return random::instance().next_real(a, b);
}
}
