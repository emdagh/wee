#pragma once
#define GL_GLEXT_PROTOTYPES 1
#include <GL/gl.h>
#include <GL/glext.h>
#include <wee.hpp>
#include <core/mat4.hpp>
#include <core/vec2.hpp>
#include <core/vec3.hpp>
#include <unordered_map>
#include <functional>
#include <core/range.hpp>
#include <core/logstream.hpp>
#include <core/factory.hpp>
#include <gfx/vertex_declaration.hpp>


namespace wee {


    enum struct kShaderType {
        VertexShader,
        PixelShader,
        GeometryShader,
        TesselationShader,
        MaxShaderType
    };

    struct uniform_variable_base {
        virtual void validate() = 0;
    };
    
    template <typename T, size_t N> 
    struct uniform_variable : public uniform_variable_base {
        GLuint _handle;
        static constexpr size_t _size = N;
        T _value;

        void set_value(const T& a) {
            _value = a;
            validate();
        }

        virtual void validate() { throw not_implemented(); }
    };

    typedef uniform_variable<float, 1> uniform1f;
    typedef uniform_variable<float, 2> uniform2f;
    typedef uniform_variable<float, 3> uniform3f;
    typedef uniform_variable<float, 4> uniform4f;
    typedef uniform_variable<mat4, 1> uniform4x4f;
    //typedef uniform_variable<vec2, 1> uniform2fv;
    //typedef uniform_variable<vec3, 1> uniform3fv;

    template <>
    void uniform1f::validate() {
        glUniform1fv(_handle, _size, &_value);
    }
    template <>
    void uniform2f::validate() {
        glUniform2fv(_handle, _size, &_value);
    }
    template <>
    void uniform3f::validate() {
        glUniform3fv(_handle, _size, &_value);
    }
    template <>
    void uniform4f::validate() {
        glUniform4fv(_handle, _size, &_value);
    }
    template <>
    void uniform4x4f::validate() {
        glUniformMatrix4fv(_handle, _size, GL_TRUE, &_value.cell[0]);
    }

#define REGISTER_UNIFORM_FACTORY(Type, Id) \
    register_factory<uniform_variable_base, Type, int>(Id)

    typedef factory<uniform_variable_base, int> uniform_factory;
    static struct gRegisterUniformFactories {
        gRegisterUniformFactories() {
            REGISTER_UNIFORM_FACTORY(uniform1f,   GL_FLOAT);
            REGISTER_UNIFORM_FACTORY(uniform2f,   GL_FLOAT_VEC2);
            REGISTER_UNIFORM_FACTORY(uniform3f,   GL_FLOAT_VEC3);
            REGISTER_UNIFORM_FACTORY(uniform4f,   GL_FLOAT_VEC4);
            REGISTER_UNIFORM_FACTORY(uniform4x4f, GL_FLOAT_MAT4);
        }
    }_;


    struct shader_program {
        GLuint _handle;
        //GLuint _vs, _ps;
        GLuint _shader[static_cast<int>(kShaderType::MaxShaderType)] = { 0 };

        struct uniform_info {
            uniform_variable_base* _uniform;
            GLint _location;
        };

        std::unordered_map<std::string, uniform_info> _info;

        shader_program() {
            _handle = glCreateProgram();
            _shader[static_cast<int>(kShaderType::VertexShader)] = glCreateShader(GL_VERTEX_SHADER);
            _shader[static_cast<int>(kShaderType::PixelShader)]  = glCreateShader(GL_FRAGMENT_SHADER);
        }

        template <typename T>
        T* get_uniform(const std::string& key) {
            return dynamic_cast<T*>(_info[key]._uniform);
        }

        void cache_all_uniforms() {
            DEBUG_LOG("caching uniforms...");
            GLint n = 0;
            glGetProgramiv(_handle, GL_ACTIVE_UNIFORMS, &n);
            for(auto it: range(n)) {
                GLenum type;
                std::string name(1024, '\0');
                GLsizei len;

                glGetActiveUniform(_handle, it, 1024, &len, NULL /*size*/, &type, name.data());
                auto* u = uniform_factory::instance().create(type);
                _info.insert(std::make_pair(name.substr(0, len), (uniform_info){u,glGetUniformLocation(_handle, name.c_str())}));
            }
        }

        void compile(const std::vector<const char*>& source, GLuint id) {
            DEBUG_LOG("compiling shader...");
            size_t n = source.size();
            glShaderSource(id, n, &source[0], nullptr);
            glCompileShader(id);


            GLint status;
            glGetShaderiv(id, GL_COMPILE_STATUS, &status);
            if(!status) {
                std::string err(1024, '\0');
                glGetShaderInfoLog(id, 1024, nullptr, err.data());
                throw std::runtime_error(err);
            }
        }

        void compile(const char* source) {

            //logstream::instance(std::cout) << loglevel::warn<< "foo" << std::endl; 
            static const char* glsl_version = "#version 130\n";

            compile({
                    glsl_version,
                "#undef  FRAGMENT\n",
                "#define VERTEX 1\n",
                source
                }, _shader[static_cast<int>(kShaderType::VertexShader)]);
            compile({
                    glsl_version,
                "#define FRAGMENT 1\n",
                "#undef  VERTEX\n",
                source
                }, _shader[static_cast<int>(kShaderType::PixelShader)]);
            set_attribute_locations();
            cache_all_uniforms();

        }

        void set_attribute_locations() {
            DEBUG_LOG("setting attribute locations");
            for(auto i: range(static_cast<int>(kVertexStreamIndex::StreamIndexMax))) 
                glBindAttribLocation(_handle, i, kVertexStreamSemantic[i]);
            link();
        }

        void link() {
            DEBUG_LOG("linking shader...");
             
            for(auto i: range(static_cast<int>(kShaderType::MaxShaderType))) 
                if(_shader[i] > 0) 
                    glAttachShader(_handle, _shader[i]);            

            glLinkProgram(_handle);
            GLint res;
            glGetProgramiv(_handle, GL_LINK_STATUS, &res);
            if(!res) {
                std::string err(1024, '\0');
                glGetProgramInfoLog(_handle, 1024, nullptr, err.data());
                throw std::runtime_error(err);
            }
        }

        void validate() {
            GLint res;
            glGetProgramiv(_handle, GL_LINK_STATUS, &res);
            
            if(res != GL_TRUE) {
                link();
                validate();
            }
            glValidateProgram(_handle);
            glGetProgramiv(_handle, GL_VALIDATE_STATUS, &res);
            if(!res) {
                std::string err;
                glGetProgramInfoLog(_handle, 1024, nullptr, err.data());
                throw std::runtime_error(err);
            }
        }
    };

    struct scoped_shader_program {
        GLint _cached;

        scoped_shader_program(shader_program* p) {
            glGetIntegerv(GL_CURRENT_PROGRAM, &_cached);
            glUseProgram(p->_handle);
        }

        ~scoped_shader_program() {
            glUseProgram(_cached);
        }
    };

}
