#include <engine/model_content_writer.hpp>
#include <engine/model_content.hpp>

using namespace wee;

template <typename S, typename T>
S& _write(S& w, const T& t) {
    throw not_implemented();
}

template <>
model_content_writer& _write<model_content_writer, vec3> (model_content_writer& w, const vec3& v) {
    w.write<float>(v.x);
    w.write<float>(v.y);
    w.write<float>(v.z);
    return w;
}



int model_content_writer::write_content() {
    /**
     * NOTE: _content contains the model content data.
     */

    write_7bit_encoded_int(_content->_meshes.size());
    for(const auto& mesh: _content->_meshes) {

        write(mesh.baseVertex);
        write(mesh.baseIndex);
        write(mesh.numVertices);
        write(mesh.numIndices);
        write(mesh.meshIndex);
        //vertex_declaration_content vertex_info;
    }


    write(_content->positions.size());
    for(const auto& pos: _content->positions) {
        _write(*this, pos);
    }

    write(_content->normals.size());
    for(const auto& normal: _content->normals) {
        _write(*this, normal);
    }

    write(_content->textureCoords.size());
    for(const auto& texcoord: _content->textureCoords) {
        _write(*this, texcoord);
    }

    write(_content->vertexBoneData.size());
    for(const auto& boneData: _content->vertexBoneData) {
        write(boneData._id[0]);
        write(boneData._id[1]);
        write(boneData._id[2]);
        write(boneData._id[3]);
    }
    write(_content->vertexBoneData.size());
    for(const auto& boneData: _content->vertexBoneData) {
        write(boneData._weight[0]);
        write(boneData._weight[1]);
        write(boneData._weight[2]);
        write(boneData._weight[3]);
    }

    write(_content->indices.size());
    for(const auto& index: _content->indices) {
        write(index);
    }

    write(_content->_materials.size());
    for(const auto& mat: _content->_materials) {
        write<SDL_Color>(mat._ka);
        write<SDL_Color>(mat._kd);
        write<SDL_Color>(mat._ks);
        write<SDL_Color>(mat._ke);
        write_string(mat._ta);
        write_string(mat._td);
        write_string(mat._ts);
        write_string(mat._te);
        write_string(mat._tn);
    }

    return 0;
}
