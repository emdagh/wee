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
#include <gfx/texture_sampler.hpp>
#include <prettyprint.hpp>
#include <SDL.h>

namespace wee {


    enum struct shader_type {
        vertex_shader,
        pixel_shader,
        geometry_shader,
        tesselation_shader,
        max_shader_type
    };

    struct uniform_variable_base {

        GLuint _handle = -1;


        constexpr void set_location(GLuint i) {
            _handle = i;

        }
        virtual void validate() = 0;
    };
    
    template <typename T, size_t N> 
    struct uniform_variable : public uniform_variable_base {
        typedef T type;
        static constexpr size_t _size = N;
        type _value;

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
    typedef uniform_variable<texture_sampler, 1> uniform_sampler;
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
    template <>
    void uniform_sampler::validate() {
        if(!_value.texture) 
            return;
        glActiveTexture(GL_TEXTURE0 + _value.unit); 
        SDL_GL_BindTexture(_value.texture, NULL, NULL);
        //glBindTexture(GL_TEXTURE_2D, _value.handle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glUniform1i(_handle, _value.unit);
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
            REGISTER_UNIFORM_FACTORY(uniform_sampler, GL_SAMPLER_2D);
        }
    }_;


    struct shader_program {
        GLuint _handle;
        //GLuint _vs, _ps;
        GLuint _shader[static_cast<int>(shader_type::max_shader_type)] = { 0 };

        struct uniform_info {
            uniform_variable_base* _uniform;
            GLint _location;
        };


        std::unordered_map<std::string, uniform_info> _info;

        shader_program() ;

        template <typename T>
        T* get_uniform(const std::string& key) {
            if(_info.count(key) != 0) {
                return dynamic_cast<T*>(_info[key]._uniform);
            }
            throw std::out_of_range(key);
            return NULL;
        }

        template <typename T>
        void set_uniform(const std::string& key, const typename T::type& x) {
            if(auto* param = get_uniform<T>(key); param) {
                param->set_value(x);
            }
        }

        void cache_all_uniforms() ;

        void compile(const std::vector<const char*>& source, GLuint id) ;

        void compile(const char* source) ;
        

        void set_attribute_locations() ;

        void link() ;

        void validate() ;
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
