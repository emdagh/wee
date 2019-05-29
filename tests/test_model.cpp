#include <gfx/graphics_device.hpp>
#include <gfx/shader.hpp>
#include <gfx/model.hpp>
#include <gfx/model_builder.hpp>
#include <gfx/mesh_generator.hpp>
#include <gfx/draw.hpp>
#include <base/application.hpp>
#include <base/applet.hpp>
#include <engine/assets.hpp>
#include <engine/model_importer.hpp>
#include <engine/assets.hpp>
#include <engine/camera.hpp>
#include <engine/vox.hpp>
#include <fstream>
#include <cstring>
#include <weegl.h>
#include <nlohmann/json.hpp>
#include <core/singleton.hpp>
#include <core/binary_reader.hpp>
#include <core/ndview.hpp>
#include "voxel.hpp"
#include "nami.hpp"

using namespace wee;
using nlohmann::json;
using namespace nami;

template <typename T, size_t N>
struct border_constraint : public basic_constraint<T, N> {
    size_t _axis;
    T _tile;

    border_constraint(size_t axis, T tile) : _axis(axis), _tile(tile)  {
    }

    void init(wave<T>& wave, const topology<N>& topo, std::vector<size_t>* res) {
        std::array<ptrdiff_t, N> ary = { 0 };
        std::copy(std::begin(topo._shape), std::end(topo._shape), ary.begin());
        ndindexer<N> ix(ary);

        ix.iterate(_axis, 0, [&](auto... coord) {
            auto idx = ix.linearize(coord...);
            wave._coeff[idx] = nami::bitmaskof<T>(_tile);
            res->push_back(idx);
        });
    }
};
typedef vertex<
    attributes::position
> vertex_p3;

typedef vertex<
    attributes::position,
    attributes::normal,
    attributes::texcoord
> vertex_p3_n3_t2;

typedef vertex<
    attributes::position,
    attributes::normal,
    attributes::primary_color
> vertex_voxel;

typedef wee::basic_vec2<int> coord;
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
void max_submatrix(const std::vector<T>& a, int nrows, int ncols, const T& skipval, int* d_max_area, coord* d_min, coord* d_max) {
    std::vector<T> w(a.size(), 0);
    std::vector<T> h(a.size(), 0);
    auto res = std::make_tuple(0, std::array<coord, 2> {}); 

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
                    coords = std::array<coord, 2> { 
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

struct aabb_renderer {
    vertex_buffer* _vb = nullptr;
    index_buffer* _ib = nullptr;
    shader_program* _shader = nullptr;
    

    using vertex_p3 =  vertex<
        attributes::position
    > ;

	static const int kNumElements = 24;

    aabb_renderer() {
        _vb = new vertex_buffer(8 * sizeof(vertex_p3));
        _ib = new index_buffer(kNumElements * sizeof(uint32_t));
        
        std::vector<uint32_t> indices = {
            0, 1, 1, 3, 3, 2, 2, 0,
            4, 5, 5, 7, 7, 6, 6, 4,
            0, 4, 1, 5, 3, 7, 2, 6
        };

        _ib->sputn(reinterpret_cast<char*>(&indices[0]), sizeof(uint32_t) * kNumElements);
    }

    

    void draw(const aabb& box) {


        vec3 corners[] = {
            box.get_corner(0),
            box.get_corner(1),
            box.get_corner(2),
            box.get_corner(3),
            box.get_corner(4),
            box.get_corner(5),
            box.get_corner(6),
            box.get_corner(7),
        };



        glBindBuffer(GL_ARRAY_BUFFER, _vb->_handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ib->_handle);
        _vb->sputn(reinterpret_cast<char*>(&corners[0]), 8 * sizeof(vec3));
        install_vertex_attributes<vertex_p3, vertex_attribute_installer>();
        draw_indexed_primitives<primitive_type::line_list, index_type::unsigned_int>(kNumElements, 0);

		//glDrawElements(GL_POINTS, kNumElements, GL_UNSIGNED_INT, NULL);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 
    }
};

/**
 * it's a bit of a weak move. But this seems like the shortest path 
 */
struct input : wee::singleton<input> {
    bool mouse_down;
    int mouse_x = 0, mouse_y = 0;   
    std::map<char, bool> keydown = {
        {'w', false},
        {'a', false},
        {'s', false},
        {'d', false}
    };
};
void demo3() {
    /**
     * this demonstrates the most basic of outputs: the console.
     */

    std::unordered_map<int, const char*> tile_colors = {
        { 110, GREEN },
        { 111, YELLOW },
        { 112, BLUE },
        { 1, RED },
        { 0, BLACK }
    };

    std::unordered_map<int, char> tiles = {
        { 110, '+' },
        { 111, '|' },
        { 112, '-' },
        { 1, '#' },
        { 0, ' ' }
    };
#if 1
    std::vector<int> example = {
        110, 110, 110, 110,
        110, 110, 110, 110,
        110, 110, 110, 110,
        110, 111, 111, 110,
        111, 112, 112, 111,
        112, 112, 112, 112,
        112, 112, 112, 112
    };
#else
    std::vector<int> example = {
        110, 112, 112, 110,
        111,   1,   1, 111,
        111,   1,   1, 111,
        111,   1,   1, 111,
        111,   1,   1, 111,
        111,   1,   1, 111,
        110, 112, 112, 110
    };
#endif
    std::vector<int> res;

    auto ts = nami::tileset::from_example(&example[0], example.size());
    auto test = nami::basic_model<uint64_t, 2>(ts, 2);

    auto copy_coeff = [&res] (const std::vector<int>& w) {
        res = w;
    };

    test.on_update += copy_coeff;
    test.on_done   += copy_coeff;
    
    test.add_example(&example[0], { 7, 4 });
    static const int OUT_W =8;
    static const int OUT_H =8;
    test.solve_for({OUT_H, OUT_W});


    for(int y=0; y < OUT_H; y++) {
        for(int x=0; x < OUT_W; x++) {
            auto t = res[x + y * OUT_W];
            std::cout << tile_colors[t] << tiles[t]; 
        }
        std::cout << std::endl;
    }

}
void demo1() {
    /**
     * this demonstrates the most basic of outputs: the console.
     */

    std::unordered_map<int, const char*> tile_colors = {
        { 110, GREEN },
        { 111, YELLOW },
        { 112, BLUE }
    };

    std::unordered_map<int, char> tiles = {
        { 110, '#' },
        { 111, '.' },
        { 112, '~' },
        { 0, ' ' }
    };

    std::vector<int> example = {
        110, 110, 110, 110,
        110, 110, 110, 110,
        110, 110, 110, 110,
        110, 111, 111, 110,
        111, 112, 112, 111,
        112, 112, 112, 112,
        112, 112, 112, 112
    };

    std::vector<int> res;

    auto ts = nami::tileset::from_example(&example[0], example.size());
    auto test = nami::basic_model<uint64_t, 2>(ts, 2);

    auto copy_coeff = [&res] (const std::vector<int>& w) {
        res = w;
    };

    test.on_update += copy_coeff;
    test.on_done   += copy_coeff;
    
    test.add_example(&example[0], { 7, 4 });
    static const int OUT_W =114;
    static const int OUT_H =13;
    test.solve_for({OUT_H, OUT_W});


    for(int y=0; y < OUT_H; y++) {
        for(int x=0; x < OUT_W; x++) {
            auto t = res[x + y * OUT_W];
            std::cout << tile_colors[t] << tiles[t]; 
        }
        std::cout << std::endl;
    }

}
size_t index_of_voxel(const vox::voxel& v, const std::array<int, 3>& dim) {
    return v.z + dim[2] * (v.y + dim[1] * v.x); // row major linearize, just like the wee::linearize for ndarrays
}


struct meshify {
    static void calculate_coords(int dim, int depth, const coord& a, const coord& b, std::array<vec3f, 4>& quad, float offset) {
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
    template <typename T>
    static void zero_vec(std::vector<T>& d, int nrows, int ncols, const coord& a, const coord& b) {

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
        using wee::range;
        typedef ndview<std::vector<int>, 3> ndview3i;
        /**
         * first, convert all chunks to a structured 3-D array;
         */
        const vox::size* len = vox::get<vox::size>(v_in);
        DEBUG_VALUE_OF(len->x);
        DEBUG_VALUE_OF(len->y);
        DEBUG_VALUE_OF(len->z);
        std::vector<int> data(len->x * len->y * len->z, 0);
        ndview3i view(&data, { len->z, len->y, len->x });
        for(const auto* ptr: v_in->chunks) {
            if(const auto* a = dynamic_cast<const vox::xyzi*>(ptr); a != nullptr) {
                for(const auto& v: a->voxels) {
                    size_t idx = view.linearize(v.z, v.y, v.x);
                    data[idx] = v.i;
                }
            }
        }

        //DEBUG_VALUE_OF(data);

        std::map<int, std::vector<std::tuple<int, int, coord, coord> > > coords_info;
        for(auto dim: range(3)) {
            for(auto depth: range(view.shape()[dim])) {
                std::vector<int> plane, colors;
                std::array<ptrdiff_t, 2> aux;
                view.slice(dim, depth, aux, std::back_inserter(plane));
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
                        coord coord_min, coord_max;
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
        return builder.build();
    }
};


template <typename T, size_t N>
void print_ndview(const ndview<T, N>& view) {
    for(auto dim: range(view.shape().size())) {
        //DEBUG_VALUE_OF(dim);

        for(auto depth: range(view.shape()[dim])) {
            //DEBUG_VALUE_OF(depth);
            std::vector<int> plane;
            std::array<ptrdiff_t, 2> aux;
            view.slice(dim, depth, aux, std::back_inserter(plane));
            //DEBUG_VALUE_OF(aux);
            for(auto y: range(aux[0])) {
                for(auto x: range(aux[1])) {
                    int t = plane[x + y * aux[1]];
                    std::cout << t << ",";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}
/*template <typename T>
void print_ndview<T, 2>(const ndview<T, 2>& view) {
}*/

model* demo2() {
    typedef ndview<std::vector<int>, 3> ndview3i;
    /**
     * 1.) Load content
     */
    //auto ifs = wee::open_ifstream("assets/test_01.vox");
    //auto ifs = wee::open_ifstream("assets/test_02.vox");
    auto ifs = wee::open_ifstream("assets/test_09.vox");
    if(!ifs.is_open()) {
        throw file_not_found("file not found");
    }
    binary_reader rd(ifs);
    vox* vx = vox_reader::read(rd);
    /**
     * 2.) regularize to grid.
     */
    auto* len = vox::get<vox::size>(vx);
    size_t example_len = len->x * len->y * len->z; 
    std::vector<int> example(example_len, 0);
    ndview3i view(&example, { len->y, len->z, len->x });
    DEBUG_VALUE_OF(view.shape());
    for(const auto* ptr: vx->chunks) {
        if(const auto* a = dynamic_cast<const vox::xyzi*>(ptr); a != nullptr) {
            for(const auto& v: a->voxels) {
                size_t idx = view.linearize(v.y, v.z, v.x);
                example[idx] = v.i;
            }
        }
    }


    //std::reverse(example.begin(), example.end());
    /**
     * 3.) Apply WFC
     */

    static int OUT_D = 5;
    static int OUT_H = 14;
    static int OUT_W = 14;

    std::vector<int> res;

    nami::tileset ts = nami::tileset::from_example(&example[0], example_len);
    ts.set_frequency(0, 100);
    nami::basic_model<uint64_t, 3> test(ts, 3);
    test.on_done += [&res] (const std::vector<int>& a) {
        res = a;
        for(auto x: a) {
            std::cout << x;;
        }

        ndview3i res_view(&res, { OUT_D, OUT_H, OUT_W });

        for(auto depth: range(res_view.shape()[0])) {
            std::vector<int> plane;
            std::array<ptrdiff_t, 2> aux;
            res_view.slice(0, depth, aux, std::back_inserter(plane));
            DEBUG_VALUE_OF(aux);
            for(auto y: range(aux[0])) {
                for(auto x: range(aux[1])) {
                    int t = plane[x + y * aux[1]];
                    //char c = t == 5 ? '#' : t == 3 ? '.' : '~';
                    std::cout << t;
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }

    };

    test.add_constraint(new border_constraint<uint64_t, 3>(1, 1));
    test.ban(1);


    test.add_example(&example[0], { len->z, len->y, len->x});//, len->z });
    test.solve_for({OUT_W, OUT_H, OUT_D});




#if 0
    ndview3i res_view(&res, { OUT_D, OUT_H, OUT_W });

    for(auto depth: range(res_view.shape()[0])) {
        std::vector<int> plane;
        std::array<ptrdiff_t, 2> aux;
        res_view.slice(0, depth, aux, std::back_inserter(plane));
        DEBUG_VALUE_OF(aux);
        for(auto y: range(aux[0])) {
            for(auto x: range(aux[1])) {
                int t = plane[x + y * OUT_W];
                char c = t == 2 ? '#' : t == 3 ? '.' : '~';
                std::cout << c;
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
#endif
    /**
     * 4.) Convert back to magicavoxel
     */
    vox* vx_res = new vox();
    vx_res->version = 150;
    
    vox::set_size(vx_res, OUT_W, OUT_H, OUT_D);
    vox::set_pack(vx_res, 1);
    vox::set_palette(vx_res, vox::get<vox::rgba>(vx)->colors);

    vox::xyzi* data = new vox::xyzi();

    ndview3i view_res(&res, {OUT_D, OUT_H, OUT_W});

    for(auto i: range(OUT_W * OUT_H * OUT_D)) {
        auto coord = view_res.delinearize(i);
        vox::voxel voxl;
        voxl.x = coord[2];
        voxl.y = coord[1];
        voxl.z = coord[0];
        voxl.i = test.tiles().tile(res[i]);
        data->voxels.push_back(voxl);
    }
    vx_res->chunks.push_back(data);
    //binary_writer writer(std::cout);
    //vox_writer::write(vx_res, writer);
    /**
     * 5.) Convert voxels to mesh
     */
    return meshify::vox_to_mesh(vx_res);
}

struct game : public applet {
    model* _model;
    model* _voxel_mesh;
    texture_sampler _sampler;
    float _time = 0.0f;
    camera _camera;

    aabb_renderer* _renderer;

    std::unordered_map<std::string, shader_program*> _shaders;
    std::vector<model*> _models;
    std::vector<int> _voxels;

    int load_content() {

// DIMENSION ORDERING IS SLOWEST CHANGING --> FASTEST CHANGING (i.e. in D3: SLICES --> ROWS --> COLUMNS)
#if 0 
#define N1 2 
#define N2 3 
#define N3 4 

        [[maybe_unused]] static const int DIM = 0;
        std::vector<int> test(N1 * N2 * N3);
        std::iota(test.begin(), test.end(), 0);
        ndview<std::vector<int>, 3> view(&test, { N1, N2, N3 });
        for(auto depth: range(view.shape()[DIM])) {
            std::vector<int> plane;
            std::array<ptrdiff_t, 2> aux;
            view.slice(DIM, depth, aux, std::back_inserter(plane));
            for(auto y: range(aux[0])) {
                for(auto x: range(aux[1])) {
                    auto t = plane[x + y * aux[1]];
                    std::cout << t << ", ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
#else
#endif


        //demo1();
        //demo3();
        //exit(-1);
#if 1
        _voxel_mesh = demo2();
#else
        auto ifs = wee::open_ifstream("assets/monu10.vox");
        if(!ifs.is_open()) {
            throw file_not_found("file not found");
        }
        binary_reader rd(ifs);
        _voxel_mesh = meshify::vox_to_mesh(vox_reader::read(rd));
#endif

        try {
            {
                auto is = open_ifstream("assets/shaders.json");
                json j = json::parse(is);
                std::string basePath = j["basePath"];
                for(auto& node: j["shaders"]) {
                    DEBUG_VALUE_OF(node["name"]);
                    std::string nodePath = node["path"];
                    auto ifs = open_ifstream(basePath + "/" + nodePath);
                    std::string source((std::istreambuf_iterator<char>(ifs)),
                                        std::istreambuf_iterator<char>());
                    _shaders[node["name"]] = new shader_program;
                    _shaders[node["name"]]->compile(source.c_str());
                }
            }
#if 0
            {
                auto is = open_ifstream("assets/models.json");
                json j = json::parse(is);

                std::string basePath = j["basePath"];
                for(auto& node: j["models"]) {
                    std::string nodePath = node["path"];
                    std::string fullPath = basePath + "/" + nodePath;
                    DEBUG_VALUE_OF(fullPath);
                    auto _ = open_ifstream(fullPath);
                    _models.push_back(import_model(_));
                }
            }
            _model = _models[0];
#endif

            _camera.set_position(5, 0, 5);
            _camera.lookat(5, 0, 0);
            _camera.set_viewport(640, 480);

            _renderer = new aabb_renderer;
        } catch(const std::exception& e) {
            DEBUG_LOG(e.what());
            exit(-1);
        }
        return 0;
    }


    int update(int dt) { 
        //_time += static_cast<float>(dt) * 0.001f;
#if 1 
        static float kSensitivity = 0.01f;
        _camera.set_rotation(
            (input::instance().mouse_x) * kSensitivity, 
            (input::instance().mouse_y) * kSensitivity,
            0.0f
        );

        if(input::instance().keydown['w']) _camera.move_forward( 1.0f);
        if(input::instance().keydown['a']) _camera.strafe(-1.0f);
        if(input::instance().keydown['s']) _camera.move_forward(-1.0f);
        if(input::instance().keydown['d']) _camera.strafe( 1.0f);
#else
        
        //_camera.lookat(0, 0, 0);
#endif
        return 0; 
    }

    int draw(graphics_device* dev) {
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        dev->clear(SDL_ColorPresetEXT::IndianRed, clear_options::kClearAll, 1.0f, 0); 

        mat4 world = mat4::mul(mat4::create_scale(0.5f), mat4::mul(mat4::create_rotation_z(-0.0f * M_PI / 180.0f), mat4::create_translation(0, 0, 0)));
        //mat4 world = mat4::mul(mat4::create_scale(0.5f), mat4::mul(mat4::create_rotation_z(-90.0f * M_PI / 180.0f), mat4::create_translation(0, 0, 0)));
        mat4 view = _camera.get_transform();
        mat4 projection = mat4::create_perspective_fov(45.0f * M_PI / 180.0f, 640.0f / 480.0f, 0.1f, 100.0f);
        
            shader_program* _program = _shaders["@voxel_shader"];
        {
            //glPolygonMode( GL_BACK, GL_LINE );
            glUseProgram(_program->_handle);
            _program->set_uniform<uniform4x4f>("wvp", mat4::mul(world, mat4::mul(view, projection)));
            install_vertex_attributes<vertex_voxel, vertex_attribute_installer>();
            GLint prev;
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev);
            glBindBuffer(GL_ARRAY_BUFFER, _voxel_mesh->_vertices->_handle);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _voxel_mesh->_indices->_handle);
            install_vertex_attributes<vertex_voxel, vertex_attribute_installer>();
            for(const auto* mesh: _voxel_mesh->_meshes) {
                draw_indexed_primitives<primitive_type::quads, index_type::unsigned_int>(mesh->num_indices, mesh->base_vertex);
            }
            glBindBuffer(GL_ARRAY_BUFFER, prev);
        }


#if 0 
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

        _program = _shaders["@default_model"];
        glUseProgram(_program->_handle);
        _program->set_uniform<uniform4x4f>("World", world);//worldViewProjection);
        _program->set_uniform<uniform4x4f>("View", view);//worldViewProjection);
        _program->set_uniform<uniform4x4f>("Projection", projection);//worldViewProjection);
        //_program->set_uniform<uniform_sampler>("base_sampler", _sampler);

        install_vertex_attributes<vertex_p3_n3_t2, vertex_attribute_installer>();
        
        glPointSize(4.0f);
        GLint prev;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev);
        glBindBuffer(GL_ARRAY_BUFFER, _model->_vertices->_handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _model->_indices->_handle);
        install_vertex_attributes<vertex_p3_n3_t2, vertex_attribute_installer>();
        for(const auto* mesh: _model->_meshes) {
            draw_indexed_primitives<primitive_type::triangles, index_type::unsigned_int>(mesh->num_indices, mesh->base_vertex);
        }
        glBindBuffer(GL_ARRAY_BUFFER, prev);


#endif
        return 0; 
    }
    void set_callbacks(application* app) {
        app->set_mouse_position(320, 240);

        app->on_keypress += [] (char c) {
            input::instance().keydown[c] = true;
            return 0;
        };
        app->on_keyrelease += [] (char c) {
            input::instance().keydown[c] = false;
            return 0;
        };

        app->on_mousedown += [&] (char) {
            input::instance().mouse_down = true;
            return 0;
        };
        app->on_mouseup += [] (char) {
            input::instance().mouse_down = false;
            return 0;
        };
        app->on_mousemove += [app] (int x, int y) {
            static bool once = false;
            if(once) {
                
                input::instance().mouse_x += (320 - x);// * kSensitivity;
                input::instance().mouse_y += (240 - y);// * kSensitivity;
            }
            once = true;

            app->set_mouse_position(320, 240);

            return 0;
        };
    }
};



int main(int, char**) {
    DEBUG_METHOD();
    applet* let = new game;
    application app(let);
    app.set_mouse_position(320, 240);
    ((game*)let)->set_callbacks(&app);
    return app.start();
}
