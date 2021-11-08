#include <core/plane.hpp>

using namespace wee;

plane plane::normalized(const plane& p) {
    plane copy(p);
    copy.normal = vec3f::normalized(copy.normal);
    return copy;
}
