#include <gfx/graphics_device.hpp>
#include <gfx/shader.hpp>
#include <gfx/model.hpp>
#include <gfx/mesh_generator.hpp>
#include <gfx/draw.hpp>
#include <base/application.hpp>
#include <base/applet.hpp>
#include <engine/assets.hpp>
#include <engine/model_importer.hpp>
#include <engine/assets.hpp>
#include <engine/camera.hpp>
#include <fstream>
#include <fstream>
#include <cstring>
#include <weegl.h>
#include <nlohmann/json.hpp>
#include <core/singleton.hpp>
#include <core/binary_reader.hpp>
#include "wfc.hpp"
#include "voxel.hpp"
#include "nami.hpp"

using namespace wee;
using nlohmann::json;


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
typedef wee::basic_vec2<int> coord;
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
                    coords = std::array<coord, 2> { r-dh, c-minw+1, r, c };
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
        draw_indexed_primitives<primitive_type::kLineList, index_type::kUnsignedInt>(kNumElements, 0);

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
        { 112, '~' }
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
    auto test = nami::basic_model(ts, 2);

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

#include "vox.hpp"
size_t index_of_voxel(const vox::voxel& v, const std::array<int, 3>& dim) {
    return v.z + dim[2] * (v.y + dim[1] * v.x); // row major linearize, just like the wee::linearize for ndarrays
}
std::vector<int> demo2() {
    auto ifs = wee::open_ifstream("assets/8x8x8.vox");
    if(!ifs.is_open()) {
        throw file_not_found("file not found");
    }
    binary_reader rd(ifs);
    [[maybe_unused]] vox::vox* v = vox::vox_reader::read(rd);


    int w, h, d;

    for(const auto* ptr: v->chunks) {
        if(const auto* a = dynamic_cast<const vox::size*>(ptr); a != nullptr) {
            //DEBUG_VALUE_OF(*a);
            w = a->x;
            h = a->y;
            d = a->z;
        }
    }
    size_t example_len = w * h * d;;
    std::vector<int> example(example_len, 0);

    for(const auto* ptr: v->chunks) {
        if(const auto* a = dynamic_cast<const vox::xyzi*>(ptr); a != nullptr) {
            for(const auto& v: a->voxels) {
                example[index_of_voxel(v, { w, h, d})] = v.i;
            }
        }
    }

    static int OUT_D = 4;
    static int OUT_H = 4;
    static int OUT_W = 4;

    std::vector<int> res;

    nami::tileset ts = nami::tileset::from_example(&example[0], example_len);
    nami::basic_model test(ts, 3);
    test.on_done += [&res] (const std::vector<int>& a) {
        res = a;
    };

    test.add_example(&example[0], { d, h, w });
    test.solve_for({OUT_D, OUT_H, OUT_W});

    return res;
}

#include "ndview.hpp"

struct game : public applet {
    model* _model;
    texture_sampler _sampler;
    float _time = 0.0f;
    camera _camera;

    aabb_renderer* _renderer;

    std::unordered_map<std::string, shader_program*> _shaders;
    std::vector<model*> _models;
    std::vector<int> _voxels;
       

    void vox_to_mesh(const vox::vox& v) {
        /**
         * first, convert all chunks to a structured 3-D array;
         */
        vox::size len = vox::vox::get_size(v);
        std::vector<int> data(len.z * len.y * len.x, 0);
        for(const auto* ptr: v.chunks) {
            if(const auto* a = dynamic_cast<const vox::xyzi*>(ptr); a != nullptr) {
                for(const auto& v: a->voxels) {
                    data[index_of_voxel(v, {len.x, len.y, len.z})] = v.i;
                }
            }
        }
        DEBUG_VALUE_OF(data);

        using wee::range;
        size_t nrows = len.y;
        size_t ncols = len.x;


        //template <typename T>

        auto zero_vec = [] (std::vector<int>& d, int nrows, int ncols, const coord& a, const coord& b) {

            int h = b.x - a.x + 1; // x = row
            int w = b.y - a.y + 1;

            for(int r: range(h)) {
                for(int c: range(w)) {
                    int row = r + a.x;
                    int col = c + a.y;

                    int i = col + row * ncols;
                    d[i] = 0;
                }
            }
        };

        typedef ndview<std::vector<int>, 3> ndview3i;
        ndview3i view(data, { 8, 8, 8 });
        DEBUG_VALUE_OF(view);

        for(auto dim: range(3)) {
            for(auto depth: range(view.shape()[dim])) {
                std::vector<std::tuple<int, coord, coord> > coords;
                std::vector<int> plane, bin;
                view.slice(dim, depth, std::back_inserter(plane));

                std::transform(std::begin(plane), std::end(plane), std::back_inserter(bin), [] (int x) { return x > 0 ? 1 : 0; });
                DEBUG_VALUE_OF(bin);

                while(std::accumulate(bin.begin(), bin.end(), 0) != 0) {
                    int area;
                    coord coord_min, coord_max;
                    max_submatrix(bin, nrows, ncols, 0, &area, &coord_min, &coord_max);
                    if(area > 1) {
                        coords.push_back(std::make_tuple(area, coord_min, coord_max));
                    }
                    zero_vec(bin, nrows, ncols, coord_min, coord_max);
                }
            }

        }
        /**
        for(auto z: range(len.z)) {
            std::vector<std::tuple<int, coord, coord> > coords;
            auto slice = std::valarray<int>(data[std::gslice(z * nrows * ncols, { nrows, ncols }, { nrows, 1 })]);
            std::vector<int> bin;
            std::transform(std::begin(slice), std::end(slice), std::back_inserter(bin), [] (int x) { return x > 0 ? 1 : 0; });

            while(std::accumulate(bin.begin(), bin.end(), 0) != 0) {
                int area;
                coord coord_min, coord_max;
                max_submatrix(bin, nrows, ncols, 0, &area, &coord_min, &coord_max);
                if(area > 1) {
                    coords.push_back(std::make_tuple(area, coord_min, coord_max));
                }
                zero_vec(bin, nrows, ncols, coord_min, coord_max);
            }
            DEBUG_VALUE_OF(coords);
        }
        */
    }

    int load_content() {

        auto ifs = wee::open_ifstream("assets/8x8x8.vox");
        if(!ifs.is_open()) {
            throw file_not_found("file not found");
        }
        binary_reader rd(ifs);
        vox_to_mesh(*vox::vox_reader::read(rd));
        exit(1);

        std::vector<int> a = {
            0, 0, 0, 0, 1, 0,
            0, 0, 1, 0, 0, 1,
            0, 0, 0, 0, 0, 0,
            1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 1,
            0, 0, 1, 0, 0, 0,
        };

        coord min_coord, max_coord;

        max_submatrix(a, 6, 6, 1, NULL, &min_coord, &max_coord);
        DEBUG_VALUE_OF(min_coord);
        DEBUG_VALUE_OF(max_coord);

        exit(1);

        _voxels = demo2();
        //exit(1);
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
            {
                auto is = open_ifstream("assets/models.json");
                json j = json::parse(is);

                std::string basePath = j["basePath"];
                for(auto& node: j["models"]) {
                    std::string nodePath = node["path"];
                    std::string fullPath = basePath + "/" + nodePath;
#if 1
                    DEBUG_VALUE_OF(fullPath);
                    auto _ = open_ifstream(fullPath);
                    _models.push_back(import_model(_));
#else
                    auto resource_path = get_resource_path(basePath) + nodePath;
                    _models.push_back(import_model_from_file(resource_path));
#endif
                }
            }

            //_model = mesh_generator::ico_sphere(1.0f, 0);
            _model = _models[0];

            _camera.set_position(0, 10, 10);
            _camera.lookat(0, 0, 0);
            _camera.set_viewport(640, 480);

            _renderer = new aabb_renderer;
        } catch(const std::exception& e) {
            DEBUG_LOG(e.what());
        }
        return 0;
    }


    int update(int dt) { 
        _time += static_cast<float>(dt) * 0.001f;
#if 0 
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
        
        _camera.lookat(0, 0, 0);
#endif
        return 0; 
    }

    int draw(graphics_device* dev) {
        glEnable(GL_DEPTH_TEST);
        dev->clear(SDL_ColorPresetEXT::CornflowerBlue, clear_options::kClearAll, 1.0f, 0); 

        mat4 world = mat4::mul(mat4::create_scale(2.f), mat4::mul(mat4::create_rotation_y(_time), mat4::create_translation(0, 0, 0)));
        mat4 view = _camera.get_transform();
        mat4 projection = mat4::create_perspective_fov(45.0f * M_PI / 180.0f, 640.0f / 480.0f, 0.1f, 100.0f);
        
        shader_program* _program = _shaders["@default_notex"];
        glUseProgram(_program->_handle);
        _program->set_uniform<uniform4x4f>("wvp", mat4::mul(world, mat4::mul(view, projection)));
        _renderer->draw(_model->_aabb);


        _program = _shaders["@default_model"];
        glUseProgram(_program->_handle);
        _program->set_uniform<uniform4x4f>("World", world);//worldViewProjection);
        _program->set_uniform<uniform4x4f>("View", view);//worldViewProjection);
        _program->set_uniform<uniform4x4f>("Projection", projection);//worldViewProjection);
        //_program->set_uniform<uniform_sampler>("base_sampler", _sampler);

        install_vertex_attributes<vertex_p3_n3_t2, vertex_attribute_installer>();
#if 0 
        
#else  
        glPointSize(4.0f);
        GLint prev;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev);
        glBindBuffer(GL_ARRAY_BUFFER, _model->_vertices->_handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _model->_indices->_handle);
        install_vertex_attributes<vertex_p3_n3_t2, vertex_attribute_installer>();
        for(const auto& mesh: _model->_meshes) {
            draw_indexed_primitives<primitive_type::kTriangles, index_type::kUnsignedInt>(mesh.num_indices, mesh.base_vertex);
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
