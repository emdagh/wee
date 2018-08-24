#pragma once


namespace wee {

    struct aabb;
    struct plane;
    struct ray;

    struct intersect {
        int test(const ray&); 
        int test(const aabb&, const aabb&, float*);
        int test(const aabb&, const plane&, float*);

    };


}
