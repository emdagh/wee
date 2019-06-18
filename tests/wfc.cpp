#if 1

constexpr static const int CHAR_BIT = sizeof(char) * 8;

template <typename T>
T popcount(T t) {
	auto v = t;
	v = v - ((v >> 1) & (T)~(T)0 / 3);                            // temp
	v = (v & (T)~(T)0 / 15 * 3) + ((v >> 2) & (T)~(T)0 / 15 * 3); // temp
	v = (v + (v >> 4)) & (T)~(T)0 / 255 * 15;                     // temp
	T c = (T)(v * ((T)~(T)0 / 255)) >> (sizeof(T) - 1) * CHAR_BIT;  // count
	return c;
}

int main(int, char**) {
	//wee::ndindexer<3> ix({ 4,3,2 });
	//ix.linearize(0, 1, 2);
	return popcount(4);
	
}
#else 
#include <wee.hpp>
#include <map>
#include <string>
#include <sstream>
#include <numeric>
#include <vector>
#include <functional>

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
#include <hokusai/hokusai.hpp>
#include <prettyprint.hpp>

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
    DEBUG_VALUE_OF(a._data);
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


template <typename OutputIt>
void make_demo2(const std::array<ptrdiff_t, 3>& d_shape, OutputIt d_first) { // = { 16,5,16 };
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
    /*
    md.on_update = [this] (const wave_propagator<uint64_t, 3>& wp) {
        if(wp.is_done()) {
            _is_loading = false;
        }
    };*/
    md.add_constraint(new border_constraint<uint64_t, 3>(to_bitmask(1), {1}));
    md.add_constraint(new fixed_tile_constraint<uint64_t, 3>(to_bitmask(2), { 4, 4, 8 }));
    md.add_constraint(new fixed_tile_constraint<uint64_t, 3>(to_bitmask(3), { 5, 4, 8 }));
    md.add_constraint(new fixed_tile_constraint<uint64_t, 3>(to_bitmask(4), { 6, 4, 8 }));
    md.add_constraint(new fixed_tile_constraint<uint64_t, 3>(to_bitmask(5), { 7, 4, 8 }));
    md.add_constraint(new fixed_tile_constraint<uint64_t, 3>(to_bitmask(6), { 8, 4, 8 }));
    md.add_constraint(new max_consecutive_constraint<uint64_t, 3>(to_bitmask(2), 5, { 1, 4 }));
    std::vector<uint64_t> res;
    md.solve(d_shape, std::back_inserter(res));
    vox* d_vox = vox_from_topology(res, topology<3>{d_shape}, ts);
    vox::set_palette(d_vox, vox::get<vox::rgba>(vx)->colors);
    model* mres = nullptr;
    vox::to_model(d_vox, &mres);
    *d_first++ = mres;
}

template <typename OutputIt>
std::vector<uint64_t> make_demo3(std::unordered_map<std::string, size_t>& _names, const std::array<ptrdiff_t, 3>& dim, OutputIt d_first) {
    auto is = open_ifstream("assets/adjacencies.json");
    json j = json::parse(is);
    std::string basepath = j["tileset"]["basepath"];
    /**
     * load any mesh data that is in the adjacencies file
     */
    size_t i=1;
    for(const auto& tile : j["tileset"]["tiles"]) {
        auto mis = open_ifstream(basepath + "/" + std::string(tile["src"]));
        _names[tile["name"]] = i++;
#if 1
        *d_first++ = import_model(mis);
#endif
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

    
    std::vector<uint64_t> res;
    basic_model mm(std::move(ts), std::move(al));

    mm.add_constraint(new border_constraint<uint64_t, 3>(to_bitmask(_names["ground"]), {1}));
    mm.solve(dim, std::back_inserter(res));

    return res;
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


typedef vertex<
    attributes::position,
    attributes::normal,
    attributes::texcoord
> vertex_p3_n3_t2;

#define DEMO_1  0
#define DEMO_2  1
#define DEMO_3  2

#define DEMO_PROGRAM DEMO_2

struct game : public applet {
    model* _model = nullptr;
    camera* _camera = nullptr;
    shader_program* _shader = nullptr;
    vec2f _viewport;
    std::vector<model*> _models;
    std::unordered_map<std::string, size_t> _names;
    std::array<ptrdiff_t, 3> _dim = { 10, 10, 10 };
    bool _is_loading = true;

    std::vector<uint64_t> _grid;


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
#if DEMO_PROGRAM == DEMO_1
            make_demo();
#elif DEMO_PROGRAM == DEMO_2
            make_demo2({32, 8, 32}, std::back_inserter(_models));
            make_shader_from_file("assets/shaders/default_p3c0.glsl", &_shader);
#elif DEMO_PROGRAM == DEMO_3
            _grid = make_demo3(_names, _dim, std::back_inserter(_models));
            make_shader_from_file("assets/shaders/default_p3n3t2.glsl", &_shader);
#endif
        } catch(std::exception& e) {
            DEBUG_LOG(e.what());
            exit(-2);
        }
        return 0;
    }

    void arcball(camera* c, const vec3f& lookat, float d, float rx, float ry) {
        quaternion qa, qb;
        qa = quaternion::axis_angle(vec3f::right(), ry);
        qb = quaternion::axis_angle(vec3f::up(), rx);
        quaternion qc = quaternion::normalized(quaternion::concat(qa, qb));
        auto p  = lookat + vec3f::transform(vec3f::forward(), qc) * d;
        c->set_position(p.x, p.y, p.z);
        c->lookat(lookat.x, lookat.y, lookat.z);
    }

    int update(int dt) {
        static float dist = 25.0f;
        constexpr static const float kS = 0.001f;
        vec3 lookat = { 8, 0, 8 };
        arcball(_camera, lookat, dist, input::instance().mouse_x * kS, input::instance().mouse_y * kS);
        if(input::instance().keydown['w']) dist += 1.0f;//_camera->move_forward( 1.0f);
        if(input::instance().keydown['s']) dist -= 1.0f;//_camera->strafe(-1.0f);
        return 0;
    }

    int draw(graphics_device* dev) {
        //static float t = 0.f;
        //t+=0.5f;
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        dev->clear(SDL_ColorPresetEXT::CornflowerBlue);//, clear_options::kClearAll, 1.0f, 0); 

        mat4 world = mat4::identity();//mat4::mul(mat4::create_scale(0.5f), mat4::mul(mat4::create_rotation_y(t * M_PI / 180.0f), mat4::create_translation(0, 0, 0)));
        mat4 view = _camera->get_transform();
        mat4 projection = mat4::create_perspective_fov(45.0f * M_PI / 180.0f, _viewport.x / _viewport.y, 0.1f, 100.0f);
        
        {
            with_shader_program context(_shader);
            _shader->set_uniform<uniform4x4f>("World", world);
            _shader->set_uniform<uniform4x4f>("View", view);
            _shader->set_uniform<uniform4x4f>("Projection", projection);
#if DEMO_PROGRAM == DEMO_2
            mat4 world = mat4::create_scale(0.5f);
            _shader->set_uniform<uniform4x4f>("World", world);
            for(const auto* model : _models) {
                dev->set_vertex_buffer(model->_vertices);
                dev->set_index_buffer(model->_indices);
                dev->set_vertex_declaration<vertex_voxel>(); // < this goes after setting the buffer objects, apparently..
                static const primitive_type ptype = primitive_type::quads;
                for(const auto* mesh: model->_meshes) {
                    draw_indexed_primitives<ptype, index_type::unsigned_int>(mesh->num_indices, mesh->base_vertex);
                }
            }
#elif DEMO_PROGRAM == DEMO_3
            dev->set_vertex_declaration<vertex_p3_n3_t2>(); // < this goes after setting the buffer objects, apparently..
            ndindexer<3> ix(_dim);
            for(auto i: range(array_product(_dim))) {
                auto coord = ix.delinearize(i);// * 1.0f;
                DEBUG_VALUE_OF(coord);
                world = mat4::create_translation(coord[0], coord[1], coord[2]);
                _shader->set_uniform<uniform4x4f>("World", world);
                size_t tileid = _grid[i];//+ 1;
                auto* model = _models[tileid];//_names[i]];
                for(const auto* mesh: model->_meshes) {
                    draw_indexed_primitives<primitive_type::triangles, index_type::unsigned_int>(mesh->num_indices, mesh->base_vertex);
                }

            }
#endif
        }
        return 0;
    }

    void set_callbacks(application* app) {
        app->on_resize += [this] (int w, int h) {
            DEBUG_LOG("window resized");
            _viewport = { static_cast<float>(w), static_cast<float>(h) };
            glViewport(0, 0, _viewport.x, _viewport.y);
            DEBUG_VALUE_OF(_viewport);
            return 0;
        };
        app->on_keypress += [] (char c) { input::instance().keydown[c] = true; return 0; };
        app->on_keyrelease += [] (char c) { input::instance().keydown[c] = false; return 0; };
        app->on_mousemove += [this, app] (int x, int y) {
            static bool once = false;
            if(!once) {
                once = true;
                return 0;
            }
            input::instance().mouse_x += (_viewport.x * 0.5f - x);// * kSensitivity;
            input::instance().mouse_y += (_viewport.y * 0.5f - y);// * kSensitivity;

            app->set_mouse_position(_viewport.x / 2, _viewport.y / 2);

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
    app.set_mouse_position(400, 300);
    ((game*)let)->set_callbacks(&app);
    //app.resize(640, 480);
    return app.start();
}
#endif