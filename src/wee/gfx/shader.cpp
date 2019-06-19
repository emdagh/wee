#include <gfx/shader.hpp>

using namespace wee;


shader_program::shader_program() {
    _handle = glCreateProgram();
    _shader[static_cast<int>(shader_type::vertex_shader)] = glCreateShader(GL_VERTEX_SHADER);
    _shader[static_cast<int>(shader_type::pixel_shader)]  = glCreateShader(GL_FRAGMENT_SHADER);
}


void shader_program::cache_all_uniforms() {
    GLint n = 0;
    glGetProgramiv(_handle, GL_ACTIVE_UNIFORMS, &n);
    for(auto it: range(n)) {
        GLenum type;
        std::string name(1024, '\0');
        GLsizei len;

        glGetActiveUniform(_handle, it, 1024, &len, NULL /*size*/, &type, name.data());
        auto* u = uniform_factory::instance().create(type);
        u->set_location(glGetUniformLocation(_handle, name.c_str()));

        _info.insert(std::make_pair(name.substr(0, len), uniform_info{u,glGetUniformLocation(_handle, name.c_str())}));
        DEBUG_VALUE_OF(name);
        DEBUG_VALUE_OF(_info[name]._location);
    }
}

void shader_program::compile(const std::vector<const char*>& source, GLuint id) {
    glShaderSource(id, source.size(), &source[0], nullptr);
    glCompileShader(id);


    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if(!status) {
        std::string err(1024, '\0');
        glGetShaderInfoLog(id, 1024, nullptr, err.data());
        throw std::runtime_error(err);
    }
}

void shader_program::compile(const char* source) {

    //logstream::instance(std::cout) << loglevel::warn<< "foo" << std::endl; 
    static const char* glsl_version = "#version 130\n";

    compile({
            glsl_version,
            "#undef  FRAGMENT\n",
            "#define VERTEX 1\n",
            source
            }, _shader[static_cast<int>(shader_type::vertex_shader)]);
    compile({
            glsl_version,
            "#define FRAGMENT 1\n",
            "#undef  VERTEX\n",
            source
            }, _shader[static_cast<int>(shader_type::pixel_shader)]);
    set_attribute_locations();
    cache_all_uniforms();

}

void shader_program::set_attribute_locations() {
    for(auto i: range(static_cast<int>(vertex_stream::StreamIndexMax)))  {
        glBindAttribLocation(_handle, i, vertex_stream_semantic[i]);
    }
    link();
}

void shader_program::link() {

    for(auto i: range(static_cast<int>(shader_type::max_shader_type))) 
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

void shader_program::validate() {
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

std::ostream& operator << (std::ostream& os, const shader_program::uniform_info& info) {
    return os << "ptr: " << info._uniform << ", location:" << info._location << std::endl;
}
