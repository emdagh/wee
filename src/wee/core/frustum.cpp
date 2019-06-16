#include <core/frustum.hpp>
#include <core/plane.hpp>
#include <core/mat4.hpp>
#include <core/range.hpp>

using namespace wee;

void frustum(plane* planes, const mat4f& wvp, bool normalize) {
    // Left clipping plane
    planes[0].a = wvp.m14 + wvp.m11;
    planes[0].b = wvp.m24 + wvp.m21;
    planes[0].c = wvp.m34 + wvp.m31;
    planes[0].d = wvp.m44 + wvp.m41;
    // Right clipping plane
    planes[1].a = wvp.m14 - wvp.m11;
    planes[1].b = wvp.m24 - wvp.m21;
    planes[1].c = wvp.m34 - wvp.m31;
    planes[1].d = wvp.m44 - wvp.m41;
    // Top clipping plane
    planes[2].a = wvp.m14 - wvp.m12;
    planes[2].b = wvp.m24 - wvp.m22;
    planes[2].c = wvp.m34 - wvp.m32;
    planes[2].d = wvp.m44 - wvp.m42;
    // Bottom clipping plane
    planes[3].a = wvp.m14 + wvp.m12;
    planes[3].b = wvp.m24 + wvp.m22;
    planes[3].c = wvp.m34 + wvp.m32;
    planes[3].d = wvp.m44 + wvp.m42;
    // Near clipping plane
    planes[4].a = wvp.m13;
    planes[4].b = wvp.m23;
    planes[4].c = wvp.m33;
    planes[4].d = wvp.m43;
    // Far clipping plane
    planes[5].a = wvp.m14 - wvp.m13;
    planes[5].b = wvp.m24 - wvp.m23;
    planes[5].c = wvp.m34 - wvp.m33;
    planes[5].d = wvp.m44 - wvp.m43;
    // Normalize the plane equations, if requested
    
    if (normalize) {
        for(auto i : range(6)) {
            planes[i] = plane::normalized(planes[i]);
        }
    } 
}
