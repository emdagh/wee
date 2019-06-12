#include <wee.hpp>
#include <map>
#include <string>
#include <sstream>
#include <numeric>
#include <vector>
#include <functional>
#include <prettyprint.hpp>
#include <core/range.hpp>
#include <core/ndview.hpp>
#include <core/bits.hpp>
#include <core/random.hpp>
#include <core/array.hpp>
#include <core/logstream.hpp>
#include <engine/vox.hpp>
#include <engine/assets.hpp>
#include <engine/model_importer.hpp>
#include <gfx/graphics_initializer.hpp>
using namespace wee;
#include "hokusai/hokusai.hpp"

template <typename InputIt, typename OutputIt>
void array_keys(InputIt first, InputIt last, OutputIt d_first) {
    while(first != last) {
        *d_first++ = (*first++).first;
    }
}
template <typename InputIt, typename OutputIt>
void array_values(InputIt first, InputIt last, OutputIt d_first) {
    while(first != last) {
        *d_first++ = (*first++).second;
    }
}
template <typename T>
void json_keys (const json& j, T d_first) {
    for(auto it=j.begin(); it != j.end(); it++) {
        *d_first++ = it.key();
    }
}

template <typename T, size_t N>
struct mirror_constraint : public basic_constraint<T,N> {

    virtual void init(const wave_propagator<T, N>&, std::vector<size_t>*) {
    }

    virtual void check(const wave_propagator<T, N>& wp, size_t i, std::vector<size_t>*) {
    }
};

template <typename T, size_t N>
struct max_consecutive_constraint : public basic_constraint<T,N> {
    T _tilemask;
    size_t _maxcount;
    std::vector<size_t> _directions;

    max_consecutive_constraint(T t, size_t maxcount, const std::vector<size_t>& directions)
    : _tilemask(t)
    , _maxcount(maxcount)
    , _directions(directions) 
    {
    }

    virtual ~max_consecutive_constraint() {
    }

    virtual void init(const wave_propagator<T, N>&, std::vector<size_t>*) {
    }

    size_t find_first(const wave_propagator<T,N>& wp, size_t i, size_t d, size_t* d_first) {
        static const size_t kNumDimensions = N;
        static const size_t kNumNeighbors = kNumDimensions << 1;

        auto& topo = wp.topo();

        size_t count   = 0;
        size_t d_inv   = (d + kNumDimensions) % kNumNeighbors;
        size_t j       = 0;

        *d_first = i;

        while(topo.try_move(*d_first, d_inv, &j)) {
            if(!(wp.data(j) == _tilemask)) 
                break;
            *d_first = j;
            count++;
        }
        return count;
    }

    virtual void check(const wave_propagator<T, N>& wp, size_t i, std::vector<size_t>*) {
        /**
         * this constraint will check if the current wave has less than the 
         * maximum amount of consecutive tiles in the indicated direction.
         * It could benefit from a monitoring system.
         *
         * wp.pop(nexttile_in_current_direction, _tilemask)
         *
         * update: 2019-06-12:
         *  this will obviously not work in all cases. An edge case would be that a cell collapsed to a 
         *  tile that is in the middle of two disjoint sets of 
         *  similar tiles (low entropy is likely here).
         * 
         *  another case could be where a tile will make a run across emoty space until a disjoint set 
         *  of self similar tiles is encountered. This could be mitigated with a look-ahead of max_consucutive cells...
         */
        size_t current = i;
        auto& topo = wp.topo();
        if(wp.data(i) == _tilemask) {
            for(auto d: _directions) {

                size_t j, first;
                size_t count = find_first(wp, i, d, &first);
                if(count == _maxcount) 
                    break;

                while(topo.try_move(current, d, &j)) {
                    if(wp.data(j) == _tilemask) {
                        count++;
                    }
                    if(count == _maxcount) {
                        size_t k;
                        if(topo.try_move(j, d, &k)) {
                            wp.pop(k, _tilemask);
                        }
                    }
                    current = j;
                }
            }
        }
    }
};

template <typename T, size_t N>
struct border_constraint : public basic_constraint<T,N> {
    T _tile;
    std::vector<size_t> _directions;

    border_constraint(T tile, const std::vector<size_t>& dir) 
    : _tile(tile)
    , _directions(dir) 
    {
    }

    virtual void init(const wave_propagator<T, N>& prop, std::vector<size_t>* res) {
        /**
         * 2019-06-01
         *
         * first of all, dermine which axis the iteration needs to be
         * processed across. This should be based on direction index
         * from topology<N>::sides.
         *
         * The mapping should look something like this:
         * [direction_index] => (axis, min/max)
         * the axis can be determined by it's index
         *      axis = index % N (where N is the number of dimensions)
         * the sign can probably be done getting the sign of the array sum of
         * the direction vector. We can multiply the result of this sum by the max
         * extent of the dimension and reach a final set of variables to into into
         * a ndindexer::slice function.
         */
        const topology<N>& topo = prop.topo();
        ndindexer<N> ix(topo.shape());
        for(size_t i=0; i < _directions.size(); i++) {
            auto neighbor   = topo.neighbor(_directions[i]);
            size_t axis     = _directions[i] % N;
            
            auto is_signed  = std::signbit(array_sum(neighbor));
            auto slice      = is_signed * (ix.shape()[axis] - 1);
            
            ix.iterate_axis(axis, slice, [&] (auto idx) {
                prop.limit(idx, (_tile));
                res->push_back(idx);
            });
        }
    }

    virtual void check(const wave_propagator<T, N>&, size_t, std::vector<size_t>*) {
    }
};
template <typename T, size_t N>
struct corner_constraint {
    static const size_t kNumCorners = 1 << N;
    typedef typename std::array<T, kNumCorners> corners_type;
    typedef typename topology<N>::value_type shape_type;
    corners_type _corners = { 0 };

    corner_constraint(size_t axis, const corners_type& corners) { 
    }
    virtual void init(const wave_propagator<T, N>& prop, std::vector<size_t>* res) {
    }
    virtual void check(const wave_propagator<T, N>&, size_t, std::vector<size_t>*) {
    }

    static corners_type make_corners(const shape_type& shape) {
        ndindexer<N> ix(shape);
        //for(auto dim : range(N)) {
        //    std::array<ptrdiff_t, N-1> aux; 
        //}
    }
        
};

template <typename T, size_t N>
struct fixed_tile_constraint : public basic_constraint<T, N> {

    typedef typename topology<N>::value_type coord_type;

    T _tilemask;
    coord_type _at;

    fixed_tile_constraint(T tilemask, const coord_type& at) : _tilemask(tilemask), _at(at) {}

    virtual void init(const wave_propagator<T, N>& prop, std::vector<size_t>* res) {
        size_t idx = prop.topo().to_index(_at);
        prop.limit(idx, _tilemask);
        res->push_back(idx);
    }
    virtual void check(const wave_propagator<T, N>&, size_t, std::vector<size_t>*) {
    }
};

void make_demo() {
    static const size_t ND = 2;
    static const std::array<ptrdiff_t, ND> d_shape = { 15, 55 };

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

    auto ts = tileset<uint64_t>::make_tileset(example.begin(), example.end());
    adjacency_list<uint64_t, ND> a(ts.length());
    a.add_example(example.begin(), ts, topology<ND> { { 7, 4 } }); 
    basic_model<uint64_t, ND> model(std::move(ts), std::move(a));
    std::vector<uint64_t> res;
    /**
     * 2D peninsula generator
     */
    model.add_constraint(new fixed_tile_constraint<uint64_t, 2>(to_bitmask(2), { 7, 23 }));
    model.add_constraint(new border_constraint<uint64_t, 2>(to_bitmask(3), { 1, 2, 3 }));

    model.solve(d_shape, std::back_inserter(res));

    for(auto r: range(d_shape[0])) {
        for(auto c: range(d_shape[1])) {
            std::cout << tiles.at(res[r * d_shape[1] + c]);
        }
        std::cout << std::endl;
    }
}
template <typename T>
vox* vox_from_topology(std::vector<T>& data, const topology<3>& topo, tileset<T>& ts) {
    const int A = 2;
    const int B = 1;
    const int C = 0;

    vox* d_vox = new vox();
    d_vox->version = 150;
    vox::set_size(d_vox, topo.shape()[A], topo.shape()[B], topo.shape()[C]);// reverse??
    vox::set_pack(d_vox, 1);

    vox::xyzi* d_data = new vox::xyzi();
    for(auto i: range(array_product(topo.shape()))) {
        auto coord = topo.to_coordinate(i);
        vox::voxel vx;
        vx.x = coord[A];
        vx.y = coord[B];
        vx.z = coord[C];// reverse??
        vx.i = ts.to_tile(ts.to_index(data[i]));
        d_data->voxels.push_back(vx);
    }
    d_vox->chunks.push_back(d_data);
    return d_vox;
}

template <typename T>
void zero_vec(std::vector<T>& first, const std::array<T, 2>& size, const std::array<T, 2>& at, const std::array<T, 2>& sub_size) {
    ndindexer<2> ix(size);
    ix.submatrix(at, sub_size, [&] (auto i) {
        first[i] = 0;
    });
}


void make_demo2(model** d_model, const std::array<ptrdiff_t, 3>& d_shape) { // = { 16,5,16 };
    auto ifs = wee::open_ifstream("assets/test_09.vox");
    if(!ifs.is_open()) {
        throw file_not_found("file not found");
    }
    binary_reader rd(ifs);
    vox* vx = vox_reader::read(rd);
    auto* extents = vox::get<vox::size>(vx);
    std::array<ptrdiff_t, 3> vdim = {
        extents->y,
        extents->z, 
        extents->x
    };
    size_t len = array_product(vdim);
    std::vector<int> example(len, 0);
    ndindexer<3> ix(vdim);//len->y, len->z, len->x });
    for(const auto* ptr: vx->chunks) {
        if(const auto* a = dynamic_cast<const vox::xyzi*>(ptr); a != nullptr) {
            for(const auto& v: a->voxels) {
                size_t idx = ix.linearize(v.y, v.z, v.x);
                example[idx] = v.i;
            }
        }
    }
    
    auto ts = tileset<uint64_t>::make_tileset(example.begin(), example.end());
    ts.set_frequency(0, 900);
    DEBUG_VALUE_OF(ts._frequency);
    //ts.set_frequency(6, 300);
    adjacency_list<uint64_t, 3> adj(ts.length());
    adj.add_example(example.begin(), ts, topology<3> { vdim }); 
    DEBUG_VALUE_OF(adj._data);
    basic_model<uint64_t, 3> md(std::move(ts), std::move(adj));
    md.add_constraint(new border_constraint<uint64_t, 3>(to_bitmask(1), {1}));
    md.add_constraint(new fixed_tile_constraint<uint64_t, 3>(to_bitmask(2), { 4, 4, 8 }));
    md.add_constraint(new fixed_tile_constraint<uint64_t, 3>(to_bitmask(3), { 5, 4, 8 }));
    md.add_constraint(new fixed_tile_constraint<uint64_t, 3>(to_bitmask(4), { 6, 4, 8 }));
    md.add_constraint(new fixed_tile_constraint<uint64_t, 3>(to_bitmask(5), { 7, 4, 8 }));
    md.add_constraint(new fixed_tile_constraint<uint64_t, 3>(to_bitmask(6), { 8, 4, 8 }));
    md.add_constraint(new max_consecutive_constraint<uint64_t, 3>(to_bitmask(2), 1, { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4 }));
    std::vector<uint64_t> res;
    md.solve(d_shape, std::back_inserter(res));
    vox* d_vox = vox_from_topology(res, topology<3>{d_shape}, ts);
    vox::set_palette(d_vox, vox::get<vox::rgba>(vx)->colors);
    vox::to_model(d_vox, d_model);
}

void make_demo3(auto& _names, auto& _models) {
    auto is = open_ifstream("assets/adjacencies.json");
    json j = json::parse(is);
    std::string basepath = j["tileset"]["basepath"];
    /**
     * load any mesh data that is in the adjacencies file
     */
    for(const auto& tile : j["tileset"]["tiles"]) {
        auto mis = open_ifstream(basepath + "/" + std::string(tile["src"]));
        _names[tile["name"]] = _models.size();
        _models.push_back(import_model(mis));
    }
    std::vector<size_t> values;
    array_values(_names.begin(), _names.end(), std::back_inserter(values));
    tileset ts = tileset<uint64_t>::make_tileset(values.begin(), values.end());
    DEBUG_VALUE_OF(ts._data);


    std::map<std::string, int> direction_mapping = {
        {"right",  0}, //topology<3>::sides[0] },
        {"below",  1},  //topology<3>::sides[1] }
        {"behind", 2},
        {"left",   3}, //topology<3>::sides[3] },
        {"above",  4}, //topology<3>::sides[4] },
        {"in_front_of", 5}
    };

    std::map<std::string, int> axis_mapping = {
        { "xmax", 0 },
        { "ymax", 1 },
        { "zmax", 2 },
        { "xmin", 3 },
        { "ymin", 4 },
        { "zmin", 5 }
    };


    adjacency_list<uint64_t, 3> al(ts.length());
    for(auto& a : j["adjacencies"]) {
        std::vector<std::string> keys;
        json_keys(a, std::back_inserter(keys));
        assert(keys.size() == 2);
        for(auto it: a[keys[0]]) {
            DEBUG_VALUE_OF(keys[0]);
            DEBUG_VALUE_OF(it);
            for(auto it_second: a[keys[1]]) {
                DEBUG_VALUE_OF(keys[1]);
                DEBUG_VALUE_OF(it_second);

                auto d_a = direction_mapping[keys[0]]; // left
                auto d_b = direction_mapping[keys[1]]; // right

                auto tile_a = _names[it];
                auto tile_b = _names[it_second];

                al.add(tile_a, tile_b, d_a);
                al.add(tile_b, tile_a, d_b);
            }
        }
    }
    DEBUG_VALUE_OF(al._data);
}


#include <gfx/graphics_device.hpp>
#include <gfx/shader.hpp>
#include <gfx/model.hpp>

#include <engine/camera.hpp>
#include <base/application.hpp>
#include <base/applet.hpp>

using namespace wee;

void make_shader_from_file(const std::string& path, shader_program** res) {
    auto ifs = open_ifstream(path);
    std::string source(
            (std::istreambuf_iterator<char>(ifs)),
            std::istreambuf_iterator<char>());
    *res = new shader_program();
    (*res)->compile(source.c_str());
}

typedef vertex<
    attributes::position,
    attributes::normal,
    attributes::primary_color
> vertex_voxel;

struct game : public applet {
    model* _model = nullptr;
    camera* _camera = nullptr;
    shader_program* _shader = nullptr;
    vec2f _viewport;
    std::vector<model*> _models;
    std::unordered_map<std::string, size_t> _names;


    game() {
        _camera = new camera();
        _camera->set_position(20, 15, 20);
        _camera->lookat(0, 0, 0);
        _viewport = { 1.f, 1.f };
    }

    virtual ~game() {
    }

    int load_content() {
        try {

            make_shader_from_file("assets/shaders/default_p3c0.glsl", &_shader);
            make_demo();
            make_demo2(&_model, {32, 5, 32});
            make_demo3(_names, _models);
        } catch(std::exception& e) {
            DEBUG_LOG(e.what());
            exit(-2);
        }
        return 0;
    }

    int update(int dt) {
        return 0;
    }

    int draw(graphics_device* dev) {
        static float t = 0.f;
        //t+=0.5f;
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        dev->clear(SDL_ColorPresetEXT::CornflowerBlue);//, clear_options::kClearAll, 1.0f, 0); 

        mat4 world = mat4::mul(mat4::create_scale(0.5f), mat4::mul(mat4::create_rotation_y(t * M_PI / 180.0f), mat4::create_translation(0, 0, 0)));
        mat4 view = _camera->get_transform();
        mat4 projection = mat4::create_perspective_fov(45.0f * M_PI / 180.0f, 640.0f / 480.0f, 0.1f, 100.0f);
        
        {
            with_shader_program context(_shader);
            _shader->set_uniform<uniform4x4f>("World", world);//mat4::mul(world, mat4::mul(view, projection)));
            _shader->set_uniform<uniform4x4f>("View", view);//mat4::mul(world, mat4::mul(view, projection)));
            _shader->set_uniform<uniform4x4f>("Projection", projection);//mat4::mul(world, mat4::mul(view, projection)));
            dev->set_vertex_buffer(_model->_vertices);
            dev->set_index_buffer(_model->_indices);
            dev->set_vertex_declaration<vertex_voxel>();
            for(const auto* mesh: _model->_meshes) {
                draw_indexed_primitives<primitive_type::quads, index_type::unsigned_int>(mesh->num_indices, mesh->base_vertex);
            }
        }
        return 0;
    }

    void set_callbacks(application* app) {
        app->on_resize += [this] (int w, int h) {
            DEBUG_LOG("window resized");
            // wee::debug << "window resized" << std::endl;

            _viewport = { static_cast<float>(w), static_cast<float>(h) };
            return 0;
        };
    }

};


int main(int argc, char** argv) {
    graphics_initializer init;
    init.width(800)
        .height(600);
    applet* let = new game;
    application app(let, std::move(init));
    app.set_mouse_position(320, 240);
    ((game*)let)->set_callbacks(&app);
    //app.resize(640, 480);
    return app.start();
}
