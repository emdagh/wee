#pragma once

#include <core/range.hpp>
#include <core/vec2.hpp>
#include <core/vec3.hpp>
#include <gfx/model_builder.hpp>

struct meshify {
    using vec3f = wee::vec3f;
    using coord = wee::vec2i;
    using model = wee::model;
    using range = wee::range;

    typedef typename std::array<vec3f, 4> coord_type;
    typedef typename std::array<int, 3> shape_type;

    static const int XY = 2;
    static const int XZ = 1;
    static const int YZ = 0;

    static void calculate_coords(int dim, int depth, const coord& a, const coord& b, std::array<vec3f, 4>& quad, float offset) {
        /**
         * dim=0 -> x/y plane (back to front) depth = z
         * dim=1 -> x/z plane (top to bottom) depth = y
         * dim=2 -> y/z plane (left to right) depth = x
         */

        float x0 = static_cast<float>(a.x) - 0.5f;
        float x1 = static_cast<float>(b.x) + 0.5f;
        float y0 = static_cast<float>(a.y) - 0.5f;
        float y1 = static_cast<float>(b.y) + 0.5f;
        float z  = static_cast<float>(depth) + offset;

        switch(dim) {
            case YZ:
                quad[0] = {z, x0, y0};
                quad[1] = {z, x1, y0};
                quad[2] = {z, x1, y1};
                quad[3] = {z, x0, y1};
                break;
            case XZ:
                quad[0] = {x0, z, y0};
                quad[1] = {x1, z, y0};
                quad[2] = {x1, z, y1};
                quad[3] = {x0, z, y1};
                break;
            case XY:
                quad[0] = {x0, y0, z};
                quad[1] = {x1, y0, z};
                quad[2] = {x1, y1, z};
                quad[3] = {x0, y1, z};
                break;
            default:
                break;
        }
    }
    template <typename T>
    static void zero_vec(
        std::vector<T>& d, 
        int nrows, 
        int ncols, 
        const coord& a, 
        const coord& b
    ) {

        int h = b.x - a.x + 1; // x = row
        int w = b.y - a.y + 1;

        for(int r: range(h)) {
            for(int c: range(w)) {
                int row = r + a.x;
                int col = c + a.y;

                int i = col + row * ncols;
                d[i] = static_cast<T>(0);
            }
        }
    }
    static model* vox_to_mesh(const wee::vox* v_in) {
    }
};
