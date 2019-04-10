#include <gfx/mesh_generator.hpp>
#include <gfx/model.hpp>
#include <gfx/vertex_buffer.hpp>
#include <gfx/index_buffer.hpp>
#include <gfx/vertex_declaration.hpp>

#include <core/vec3.hpp>
#include <cmath>

using namespace wee;


typedef vertex<
    attributes::position,
    attributes::normal,
    attributes::texcoord
> vertex_p3_n3_t2;

model* mesh_generator::ico_sphere(float radius, int n) {
    float t = 1.0f + std::sqrt(5.0f) / 2.0f;
   
    std::vector<vertex_p3_n3_t2> vertices;

    std::vector<vec3f> list = {
        { -1.0f,  t, 0.0f },
        {  1.0f,  t, 0.0f },
        { -1.0f, -t, 0.0f },
        {  1.0f, -t, 0.0f },
        
        {  0.0f, -1.0f,  t },
        {  0.0f,  1.0f,  t },
        {  0.0f, -1.0f, -t },
        {  0.0f,  1.0f, -t },
        
        {  t, 0.0f, -1.0f },
        {  t, 0.0f,  1.0f },
        { -t, 0.0f, -1.0f },
        { -t, 0.0f,  1.0f }
    };

    vertices.resize(list.size());

    for(auto i : range(list.size())) {
        auto& v = vertices[i];
        v._position = list[i];
        v._normal = vec3f::normalized(list[i]);
        v._texcoord = { 0.f, 0.f };
        //vertices.push_back(v);
    }

    std::vector<uint32_t> ix = {
         0, 11,  5,
         0,  5,  1,
         0,  1,  7,
         0,  7, 10,
         0, 10, 11,
         1,  5,  9,
         5, 11,  4,
        11, 10,  2,
        10,  7,  6,
         7,  1,  8,
         3,  9,  4,
         3,  4,  2,
         3,  2,  6,
         3,  6,  8,
         3,  8,  9,
         4,  9,  5,
         2,  4, 11,
         6,  2, 10,
         8,  6,  7,
         9,  8,  1
    };

    auto* vbo = new vertex_buffer(vertices.size() * sizeof(decltype(vertices[0])));//vertex_p3_n3_t2));
    auto* ibo = new index_buffer(ix.size() * sizeof(decltype(ix[0])));//uint32_t));

    vbo->sputn(reinterpret_cast<char*>(&vertices[0]), sizeof(vertex_p3_n3_t2) * vertices.size());
    ibo->sputn(reinterpret_cast<char*>(&ix[0]), sizeof(uint32_t) * ix.size());


    return new model {
        { 
            model_mesh {  
                vbo,
                ibo,
                {   
                    { 0, model_mesh_part { 0, 0, 0, vertices.size(), ix.size() } }
                }
            }
        }
    }; 
}
