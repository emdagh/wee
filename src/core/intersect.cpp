#include <core/intersect.hpp>
#include <core/vec3.hpp>
#include <core/ray.hpp>
#include <core/sphere.hpp>
#include <cmath>

namespace wee {
    int solve_quadratic(float a, float b, float c, float* t0, float* t1) {
        float d = b * b - 4.f * a * c;
        if (d < 0.f) {
            return -1;
        } else if (d == 0.f) {
            *t0 = *t1 = -.5f * b / a;
        } else {
            float q = (b > 0.f) 
                ? -.5f * (b + std::sqrt(d)) 
                : -.5f * (b - std::sqrt(d));

            *t0 = q / a;
            *t1 = c / q;
        }
        if (*t0 > *t1) {
            std::swap(*t0, *t1);
        }
        return 0;
    }
    template <>
    int intersect<ray, ray>(const ray&, const ray&, float*) {
        return 0;
    }

    template <>
    int intersect<ray, sphere>(const ray& r, const sphere& s, float* d) {
        float t0, t1;
        float radius_sq = s.radius * s.radius;
#if 0
        vec3f L = b.center - a.origin;
        float tca = vec3f::dot(L, ray.direction);
        float d2  = vec3f::dot(L, L) - tca * tca;
        if(d2 > radius_sq) {
            return -1;
        }
        float tch = std::sqrt(radius_sq - d2);
        t0 = tca - tch;
        t1 = tca + thc;
#else
        vec3f L = r.origin - s.center;
        float a = vec3f::dot(r.direction, r.direction);
        float b = 2.0f * vec3f::dot(r.direction, L);
        float c = vec3f::dot(L, L) - radius_sq;
        if(!solve_quadratic(a, b, c, &t0, &t1)) {
            return -1;
        }
#endif
        if(t0 > t1) {
            std::swap(t0, t1);
        }
        if(t0 < 0) {
            t0 = t1;
            if(t0 < t1) {
                return -1;
            }
        }
        *d = t0;
        return 0;
    }
}




