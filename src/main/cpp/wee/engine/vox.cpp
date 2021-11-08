#include <engine/vox.hpp>
#include <core/range.hpp>
#include <core/vec3.hpp>
#include <core/vec2.hpp>
#include <core/ndview.hpp>
#include <gfx/model_builder.hpp>
#include <gfx/vertex_declaration.hpp>
#include <nlohmann/json.hpp>


namespace wee {
	
}
using nlohmann::json;
using namespace wee;

static void calculate_coords(int dim, int depth, const vec2i& a, const vec2i& b, std::array<vec3f, 4>& quad, float offset) {
    /**
     * dim=0 -> x/y plane (back to front) depth = z
     * dim=1 -> x/z plane (top to bottom) depth = y
     * dim=2 -> y/z plane (left to right) depth = x
     */
    static const int XY = 2;
    static const int XZ = 1;
    static const int YZ = 0;

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
#if 1
template <typename T>
static void zero_vec(std::vector<T>& d, int nrows, int ncols, const vec2i& a, const vec2i& b) {

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
#else
#endif

template <typename T, size_t N, typename InputIt>
void max_submatrixnd(InputIt first, InputIt last, const std::array<T, N>& shape, const T& ignore) {
    size_t n = std::distance(first, last);

    ndindexer<N> ix(shape);
    for(size_t i=0; i < n; i++) {
        auto at = ix.delinearize(i);
        //auto j = ix.linearize(at + 
    }
}

/**
 * TODO: this should be generalizable to higher dimensions. However, for now; we can keep it at 2-D
 *
 * Helper class to find the largest submatrix in a 2-D matrix that doesn't contain the value defined by `skipval`
 * @param - input matrix
 * @param - number of rows in input
 * @param - number of columns in input
 * @param - value the submatrix is not allowed to contain
 * @param_out - area of maximum submatrix
 * @param_out - top-left coordinate of submatrix
 * @param_out - bottom-right coordinate of submatrix
 */
template <typename T>
void max_submatrix(const std::vector<T>& a, int nrows, int ncols, const T& skipval, int* d_max_area, vec2i* d_min, vec2i* d_max) {
    std::vector<T> w(a.size(), 0);
    std::vector<T> h(a.size(), 0);
    auto res = std::make_tuple(0, std::array<vec2i, 2> {}); 

    for(auto r: wee::range(nrows)) {
        for(auto c: wee::range(ncols)) {
            auto i = c + r * ncols;
            auto j = (c + (r-1) * ncols);
            auto k = ((c-1) + r * ncols);
            if(a[i] == skipval) continue;
            if(r == 0)          h[i] = 1;
            else                h[i] = h[j] + 1; // increment
            if(c == 0)          w[i] = 1;
            else                w[i] = w[k] + 1; // increment
            auto minw = w[i];
            for(auto dh: wee::range(h[i])) {
                auto m = c + (r-dh) * ncols;
                minw = std::min(minw, w[m]);
                auto area = (dh + 1) * minw;

                auto& [max_area, coords] = res; // LEARNING MOMENT: structured binding on tuples!
                if(area > max_area) {
                    max_area = area;
                    coords = std::array<vec2i, 2> { 
                        r-dh, 
                        c-minw+1, 
                        r, 
                        c 
                    };
                }
            }
        }
    }
    const auto& [max_area, coords] = res;
    if(d_max_area) *d_max_area = max_area;
    if(d_min) *d_min = coords[0];
    if(d_max) *d_max = coords[1];
}


void vox::to_model(const vox* v_in, model** d_model) {
typedef vertex<
    attributes::position,
    attributes::normal,
    attributes::primary_color
> vertex_voxel;
    DEBUG_METHOD();
        typedef ndindexer<3> ndview3i;
        /**
         * first, convert all chunks to a structured 3-D array;
         */
        const vox::size* len = vox::get<vox::size>(v_in);
        DEBUG_VALUE_OF(len->x);
        DEBUG_VALUE_OF(len->y);
        DEBUG_VALUE_OF(len->z);
        std::vector<int> data(len->x * len->y * len->z, 0);
        ndview3i view({ len->z, len->y, len->x });
        for(const auto* ptr: v_in->chunks) {
            if(const auto* a = dynamic_cast<const vox::xyzi*>(ptr); a != nullptr) {
                for(const auto& v: a->voxels) {
                    size_t idx = view.linearize(v.z, v.y, v.x);
                    data[idx] = v.i;
                }
            }
        }

        std::map<int, std::vector<std::tuple<int, int, vec2i, vec2i> > > coords_info;
        for(auto dim: range(3)) {
            for(auto depth: range(view.shape()[dim])) {
                std::vector<int> plane, colors;
                std::array<ptrdiff_t, 2> aux;
                view.slice(dim, depth, aux, [&plane, &data] (auto s) { plane.push_back(data[s]); } );//std::back_inserter(plane));
                colors = plane;
                std::sort(colors.begin(), colors.end());
                auto last = std::unique(colors.begin(), colors.end());
                colors.erase(last, colors.end());
                for(int color : colors) {
                    if(color == 0) 
                        continue;
                    std::vector<int> bin;
                    std::transform(std::begin(plane), std::end(plane), std::back_inserter(bin), [&color] (int x) { return x == color ? 1 : 0; });
                    size_t is_empty = std::accumulate(bin.begin(), bin.end(), 0);
                    while(is_empty != 0) {
                        int area;
                        vec2i coord_min, coord_max;
                        max_submatrix(bin, aux[0], aux[1], 0, &area, &coord_min, &coord_max);
                        if(area > 0) {
                            coords_info[color].push_back(std::make_tuple(dim, depth, coord_min, coord_max));
                        }
                        zero_vec(bin, aux[0], aux[1], coord_min, coord_max);
                        is_empty = std::accumulate(bin.begin(), bin.end(), 0);
                    }
                }
            }
        }
        const vox::rgba* palette = vox::vox::get<vox::rgba>(v_in);

        size_t num_vertices = 0;//, num_indices = 0;
        size_t num_indices = 0;
         model_builder<vertex_voxel, uint32_t> builder;
#define _USE_BUILDER

        for(const auto& [color, coords] : coords_info) {
            DEBUG_VALUE_OF(color);
            builder
                .material(color)
                .base_vertex(num_vertices)
                .base_index(num_indices);

            for(const auto& coord: coords) {
                /**
                 * we should emit two primitives per plane: one for the front-face, and one for the
                 * back-face. If we don't do this, we'd have a mesh that is open on one side. 
                 */
                for(auto face : range(2)) {
                    std::array<vec3f, 4> quad_face; 
                    calculate_coords(std::get<0>(coord), std::get<1>(coord), std::get<2>(coord), std::get<3>(coord), quad_face, (((face & 1) == 1) ? 0.5f : -0.5f));
                    for(auto ii : range(4)) {
                        vertex_voxel v0;
                        v0._position = quad_face[ii];
                        v0._color    = palette->colors[color - 1];
                        v0._normal   = std::get<0>(coord) == 0 ? vec3f::right() : std::get<0>(coord)== 1 ? vec3f::up() : vec3f::forward();
                        builder
                            .add_vertex(v0)
                            .add_index(num_indices++);
                        num_vertices++;
                    }
                }
            }
        }
        DEBUG_VALUE_OF(num_vertices);
        DEBUG_VALUE_OF(num_indices);
        *d_model = builder.build();
}


void to_json(json& j, const vox::voxel& v) {
	j = {
		{ "x", v.x },
		{ "y", v.y },
		{ "z", v.z },
		{ "i", v.i },
	};
}
std::ostream& operator << (std::ostream& os, const vox::voxel& v) {
	json j;
	to_json(j, v);
	return os << j;
}
void to_json(json& j, const vox::size& s) {
	j = {
		{ "x", s.x },
		{ "y", s.y },
		{ "z", s.z },
	};
}
std::ostream& operator << (std::ostream& os, const vox::size& s) {
	json j;
	to_json(j, s);
	return os << j;
}