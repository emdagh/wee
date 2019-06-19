#pragma once

#include <gfx/model.hpp>

namespace wee {
template <typename S, typename T>
class model_builder {
    typedef S vertex_type;
    typedef T index_type;
    size_t _material = 0;

    struct mesh_info {
        size_t _base_index = 0;
        size_t _base_vertex = 0;
        size_t _num_indices = 0;
        size_t _num_vertices = 0;
    };

    aabb _aabb;
    std::vector<vertex_type> _vertices;
    std::vector<index_type> _indices;

    std::unordered_map<size_t, mesh_info> _meshes;
protected:
    template <typename B, typename C>
    B* create_buffer(const std::vector<C>& src) {
        B* dst = new B(sizeof(C) * src.size());
        std::ostream os(dst);
        os.write(reinterpret_cast<const char*>(&src[0]), sizeof(C) * src.size());
        return dst;
    }

    model_mesh* create_mesh(const mesh_info& info) {
        model_mesh* res = new model_mesh();
        res->material_index = _material;
        res->base_vertex = info._base_vertex;
        res->base_index = info._base_index;
        res->num_vertices = info._num_vertices;
        res->num_indices = info._num_indices;
        return res;
    }
public:
    model_builder& material(size_t n) {
        _material = n;
        if(_meshes.count(_material) == 0) {
            _meshes.insert({n, mesh_info{}});
        }
        return *this;
    }
    
    model_builder& base_index(size_t i) {
        _meshes.at(_material)._base_index = i;
        return *this;
    }

    model_builder& base_vertex(size_t i) {
        _meshes.at(_material)._base_vertex = i;
        return *this;
    }

    model_builder& add_index(index_type i) {
        _indices.push_back(i);
        _meshes.at(_material)._num_indices++;
        return *this;
    }

    template <typename... Ts>
    model_builder& add_indices(Ts... args) {
        std::array<ptrdiff_t, sizeof...(Ts) + 1> idx({
            static_cast<index_type>(args)...
        });
        for(auto& i: idx) {
            add_index(i);
        }

        return *this;
    }
    
    model_builder& add_triangle(index_type i[3]) {
        return add_indices(i[0], i[1], i[2]);
    }

    model_builder& add_vertex(const vertex_type& t) {
        _vertices.push_back(t);
        _meshes.at(_material)._num_vertices++;
        _aabb.add(t._position);
        return *this;
    }

    template <typename... Ts>
    model_builder& add_vertices(Ts... ts) {
        return add_vertex(ts...);
    }

    model_builder& triangulate() {
        throw not_implemented("triangulate");
        return *this;
    }
    model* build() {
        assert(_vertices.size());
        assert(_indices.size());

        model* res = new model();
        {
            res->_vertices = create_buffer<vertex_buffer>(_vertices);
            res->_indices  = create_buffer<index_buffer>(_indices);
        }

        for(const auto& info: _meshes) {
            res->_meshes.push_back(create_mesh(info.second));
        }

        return res;
    }
};
} // namespace wee
