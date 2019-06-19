#pragma once

#include <wee/wee.hpp>

namespace wee {

    enum struct intersection : uint8_t {
        kContains,
        kIntersects,
        kDisjoint
    };

    enum struct plane_intersection {
        kFront,
        kIntersects,
        kBehind
    };

    struct ray;
    struct aabb;
    struct mat4;
    struct sphere;
    struct plane;
    struct frustum;
    struct triangle;

    int solve_quadratic(float, float, float, float*, float*);

    template <typename S, typename T>
    int intersect(const S&, const T&, float*) {
        throw not_implemented();
    }
    /*
    struct intersect {
        int ray_aabb(const ray&, const aabb&, float*);
        int ray_ray(const ray&, const ray&, float*);
        int ray_sphere(const ray&, const sphere&, float*);
        int ray_plane(const ray&, const plane&, float*);
        int ray_triangle(const ray&, const triangle&, float*);
        int ray_ray(const ray&, const ray&, float*);
    };
    */
}
