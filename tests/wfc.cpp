#include <wee.hpp>
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
using namespace wee;
#include <prettyprint.hpp>
#include "hokusai/hokusai.hpp"

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
        ndindexer<N> ix(prop.topo().shape());
        for(size_t i=0; i < _directions.size(); i++) {
            auto neighbor   = prop.topo().neighbor(_directions[i]);
            size_t axis     = _directions[i] % N;
            
            auto is_signed  = std::signbit(array_sum(neighbor));
            auto slice      = is_signed * (ix.shape()[axis] - 1);
            
            ix.iterate_axis(axis, slice, [&] (auto idx) {
                prop.limit(idx, (_tile));
                res->push_back(idx);
            });
        }
    }

    virtual void check(const wave_propagator<T, N>&, std::vector<size_t>*) {
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
    virtual void check(const wave_propagator<T, N>&, std::vector<size_t>*) {
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
    virtual void check(const wave_propagator<T, N>&, std::vector<size_t>*) {
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
vox* vox_from_topology(std::vector<T>& data, const ndindexer<3>& topo, tileset<T>& ts) {
    const int A = 2;
    const int B = 1;
    const int C = 0;

    vox* d_vox = new vox();
    d_vox->version = 150;
    vox::set_size(d_vox, topo.shape()[A], topo.shape()[B], topo.shape()[C]);// reverse??
    vox::set_pack(d_vox, 1);

    vox::xyzi* d_data = new vox::xyzi();
    DEBUG_VALUE_OF(topo.shape());
    DEBUG_VALUE_OF(array_product(topo.shape()));
    for(auto i: range(array_product(topo.shape()))) {
        auto coord = topo.delinearize(i);
        DEBUG_VALUE_OF(i);
        DEBUG_VALUE_OF(coord);
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


void make_demo2(model** d_model) {
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
    DEBUG_VALUE_OF(ts._data);
    DEBUG_VALUE_OF(ts._frequency);
    adjacency_list<uint64_t, 3> adj(ts.length());
    adj.add_example(example.begin(), ts, topology<3> { vdim }); 
    DEBUG_VALUE_OF(adj._data);
    basic_model<uint64_t, 3> md(std::move(ts), std::move(adj));
    md.add_constraint(new border_constraint<uint64_t, 3>(to_bitmask(1), {1}));//{ 5 })); // direction index 5 = 0, -1, 0
    //md.add_constraint(new border_constraint(1, 1)); // direction index 5 = 0, -1, 0
    //md.ban(1);
    std::array<ptrdiff_t, 3> d_shape = { 15,5,15 };
    std::vector<uint64_t> res;
    md.solve(d_shape, std::back_inserter(res));
    DEBUG_VALUE_OF(res);
    vox* d_vox = vox_from_topology(res, ndindexer<3>(d_shape), ts);
    vox::set_palette(d_vox, vox::get<vox::rgba>(vx)->colors);
    vox::to_model(d_vox, d_model);
   
    //DEBUG_VALUE_OF(mdl);    
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

    game() {
        _camera = new camera();
        _camera->set_position(15, 10, 15);
        _camera->lookat(0, 0, 0);
        _viewport = { 1.f, 1.f };
    }

    virtual ~game() {
    }

    int load_content() {
        try {
            make_shader_from_file("assets/shaders/default_p3c0.glsl", &_shader);
            make_demo();
            make_demo2(&_model);
        } catch(...) {
            exit(-2);
        }
        return 0;
    }

    int update(int dt) {
        return 0;
    }

    int draw(graphics_device* dev) {
#if 1
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        dev->clear(SDL_ColorPresetEXT::IndianRed, clear_options::kClearAll, 1.0f, 0); 

        mat4 world = mat4::mul(mat4::create_scale(0.5f), mat4::mul(mat4::create_rotation_z(-0.0f * M_PI / 180.0f), mat4::create_translation(0, 0, 0)));
        mat4 view = _camera->get_transform();
        mat4 projection = mat4::create_perspective_fov(45.0f * M_PI / 180.0f, 640.0f / 480.0f, 0.1f, 100.0f);
        
            shader_program* _program =  _shader;
        {
            //glPolygonMode( GL_BACK, GL_LINE );
            glUseProgram(_program->_handle);
            _program->set_uniform<uniform4x4f>("World", world);//mat4::mul(world, mat4::mul(view, projection)));
            _program->set_uniform<uniform4x4f>("View", view);//mat4::mul(world, mat4::mul(view, projection)));
            _program->set_uniform<uniform4x4f>("Projection", projection);//mat4::mul(world, mat4::mul(view, projection)));
            install_vertex_attributes<vertex_voxel, vertex_attribute_installer>();
            GLint prev;
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev);
            glBindBuffer(GL_ARRAY_BUFFER, _model->_vertices->_handle);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _model->_indices->_handle);
            install_vertex_attributes<vertex_voxel, vertex_attribute_installer>();
            for(const auto* mesh: _model->_meshes) {
                draw_indexed_primitives<primitive_type::quads, index_type::unsigned_int>(mesh->num_indices, mesh->base_vertex);
            }
            glBindBuffer(GL_ARRAY_BUFFER, prev);
        }
        return 0;
#else
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        mat4 world, view, projection;
        world = mat4::identity();
        view = _camera->get_transform();
        projection = mat4::create_perspective_fov(45.f * M_PI / 180.0f, _viewport.x / _viewport.y, 0.1f, 1000.0f); 

        dev->clear(SDL_ColorPresetEXT::CornflowerBlue);
        {
            with_shader_program s(_shader); 
            {
                _shader->set_uniform<uniform4x4f>("World", world);
                _shader->set_uniform<uniform4x4f>("View", view);
                _shader->set_uniform<uniform4x4f>("Projection", projection);
            }
            { 
                //with_vertex_declaration<vertex_voxel> _decl();
                install_vertex_attributes<vertex_voxel, vertex_attribute_installer>();
                with_vertex_buffer _vb(_model->_vertices);
                with_index_buffer _ib(_model->_vertices);
                for(auto* mesh : _model->_meshes) {
                    draw_indexed_primitives<primitive_type::points, index_type::unsigned_int>(mesh->num_indices, mesh->base_vertex);
                }
            }
        }
        return 0;

#endif
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
    applet* let = new game;
    application app(let);
    app.set_mouse_position(320, 240);
    ((game*)let)->set_callbacks(&app);
    app.resize(640, 480);
    return app.start();
}
