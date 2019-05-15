#include <gfx/shader.hpp>
#include <gfx/vertex_buffer.hpp>
#include <gfx/vertex_declaration.hpp>

#include <gfx/texture_sampler.hpp>
#include <base/application.hpp>
#include <base/applet.hpp>
#include <engine/assets.hpp>
#include <fstream>
#include <cstring>
using namespace wee;






template <typename T>
using basic_pixel_buffer = basic_device_buffer<T, GL_PIXEL_PACK_BUFFER, GL_PIXEL_PACK_BUFFER_BINDING>;
typedef basic_pixel_buffer<char> pixel_buffer;

class texture {
    GLuint _handle;
    struct scope {
        GLint prev;
        scope(GLuint t) { 
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev); 
            glBindTexture(GL_TEXTURE_2D, t);
        }

        ~scope() { glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(prev)); }
    };
public:
    texture(size_t w, size_t h) {
        glGenTextures(1, &_handle);
        glBindTexture(GL_TEXTURE_2D, _handle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};








typedef vertex<
    attributes::position,
    attributes::texcoord
> sprite_vertex;


struct game : public applet{
    shader_program* _program;
    sprite_vertex _vertices[4];
    vertex_buffer* _buffer;
    GLuint _vb;
    texture_sampler _sampler;
    int load_content() {
        REGISTER_UNIFORM_FACTORY(uniform_sampler, GL_SAMPLER_2D);
        std::ifstream ifs = open_ifstream("assets/shaders/default.glsl");
        std::string source((std::istreambuf_iterator<char>(ifs)),
                            std::istreambuf_iterator<char>());
                                
        _program = new shader_program;
        _program->compile(source.c_str());
        _buffer = new vertex_buffer(4 * sizeof(sprite_vertex));

        _vertices[0]._position = {   0.f,   0.f, 0.f };
        _vertices[1]._position = { 320.f,   0.f, 0.f };
        _vertices[2]._position = { 320.f, 240.f, 0.f };
        _vertices[3]._position = {  10.f, 240.f, 0.f };
        _vertices[0]._texcoord = { 0.f, 0.f };
        _vertices[1]._texcoord = { 1.f, 0.f };
        _vertices[2]._texcoord = { 1.f, 1.f };
        _vertices[3]._texcoord = { 0.f, 1.f };
        
        {
            std::ostream os(_buffer);

            //vertex_stream vs(_buffer);
            os.write(reinterpret_cast<char*>(&_vertices[0]), sizeof(sprite_vertex) * 4);
            //_buffer->sputn(reinterpret_cast<char*>(&_vertices[0]), sizeof(sprite_vertex) * 4);
        }

        {
            auto is = wee::open_ifstream("assets/img/test.png");
            _sampler.texture = assets<SDL_Texture>::instance().load("@test", is);
            _sampler.unit = 0;
        }

        return 0;
    }

    int update(int) {
        return 0;
    }

    int draw(graphics_device*) {
        mat4f worldViewProjection = mat4f::create_ortho_offcenter(0, 640, 0, 480, -1, 1);
        
        glClearColor(0.0f, 0.0f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        {
            glUseProgram(_program->_handle);
#if 1 
            _program->set_uniform<uniform4x4f>("wvp", worldViewProjection);
            _program->set_uniform<uniform_sampler>("base_sampler", _sampler);
#else
            uniform4x4f* worldViewProjParameter     = _program->get_uniform<uniform4x4f>("wvp");
            uniform_sampler* baseSamplerParameter   = _program->get_uniform<uniform_sampler>("base_sampler");
            worldViewProjParameter->set_value(worldViewProjection);
            baseSamplerParameter->set_value(_sampler);
#endif   
            GLint prev;
            glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &prev);
            glBindBuffer(GL_ARRAY_BUFFER, _buffer->_handle);
            install_vertex_attributes<sprite_vertex, vertex_attribute_installer>();
            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
            glBindBuffer(GL_ARRAY_BUFFER, prev);
        }
        return 0;
    }

};

int main(int, char**) {



    applet* let = new game;
    application app(let);
    //((game*)let)->set_callbacks(&app);
    return app.start();
}
