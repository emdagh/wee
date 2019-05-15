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

//template <size_t N>
//nami::direction nami::topology::_directions = nami::topology::build_directions<N>();

//nami::model<int, unsigned int, 2> ;
//
//
/*
nami::model::from_example({ 0, 0, 0, 
                            0, 0, 0,
                            0, 0, 0,
                            0, 0, 0,
                            0, 1, 0,                    
                            0, 0, 0,
                            0, 0, 0,
                            0, 0, 0,
                            0, 0, 0 },
                            { 3, 3, 3 },
                            { 6, 6, 6 }
        );
        */

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



struct game : public applet {
    model* _model;
    texture_sampler _sampler;
    float _time = 0.0f;
    camera _camera;

    aabb_renderer* _renderer;

    std::unordered_map<std::string, shader_program*> _shaders;
    std::vector<model*> _models;

    int load_content() {

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

        auto ts = nami::tileset::from_example(&example[0], example.size());
        auto test = nami::basic_model(ts);
        //test.weights_from_example(&example[0], example.size());
        test.add_example(&example[0], { 3, 3 }); // {7, 4 });
        test.solve_for({80, 80});
        exit(0);

        std::vector<uint16_t> voxels;

        std::ifstream is = open_ifstream("assets/exported.vox", std::ios::in| std::ios::binary);
        binary_reader rd(is);
        from_magica_voxel(rd, std::back_inserter(voxels));
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
        typedef vertex<
            attributes::position,
            attributes::normal,
            attributes::texcoord
        > vertex_p3_n3_t2;

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
