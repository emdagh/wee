#include <engine/model_content_reader.hpp>
#include <engine/model_content.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <core/range.hpp>
#include <core/mat4.hpp>
#include <core/vec3.hpp>
#include <core/quaternion.hpp>
#include <core/logstream.hpp>
#include <gfx/vertex_declaration.hpp>
#include <gfx/vertex_buffer.hpp>
#include <vector>
#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <prettyprint.hpp>
#include <nlohmann/json.hpp>
#include <SDL.h>


using namespace wee;

namespace wee {
    using nlohmann::json;
    void to_json(json& j, const SDL_Color& c) {
        uint32_t value = 0;

        value |= static_cast<int>(c.r)  << 24;
        value |= static_cast<int>(c.g) << 16;
        value |= static_cast<int>(c.b) <<  8;
        value |= static_cast<int>(c.a);
    }

    void from_json(const json& j, SDL_Color& c) {
        uint32_t value = j["value"];
        c.r = value & 0xff000000;
        c.g = value & 0x00ff0000;
        c.b = value & 0x0000ff00;
        c.a = value & 0x000000ff;
    }

    void to_json(json& j, const aiColor4D& c) {
        uint32_t value = 0;
        value |= static_cast<int>(c.r * 255.0f) << 24;
        value |= static_cast<int>(c.g * 255.0f) << 16;
        value |= static_cast<int>(c.b * 255.0f) << 8;
        value |= static_cast<int>(c.a * 255.0f);
        j["value"] = value;
    }

    void from_json(const json& j, aiColor4D& c) {
        uint32_t value = j["value"];
        c.r = (value & 0xff000000) / 255.f;
        c.g = (value & 0x00ff0000) / 255.f;
        c.b = (value & 0x0000ff00) / 255.f;
        c.a = (value & 0x000000ff) / 255.f;
    }
}


std::ostream& operator << (std::ostream& os, const aiColor4D& color) {
    json j;
    to_json(j, color);
    return os << std::dec << j;
}
       

mat4f convert(const aiMatrix4x4& val) {
    return mat4f {
        val.a1, val.a2, val.a3, val.a4,
        val.b1, val.b2, val.b3, val.b4,
        val.c1, val.c2, val.c3, val.c4,
        val.d1, val.d2, val.d3, val.d4
    };
}


model_content* model_content_reader::read(std::istream& is) const {
    DEBUG_METHOD();
    std::vector<char> data((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    auto opts = 
        aiProcess_GenSmoothNormals | 
        aiProcess_Triangulate | 
        aiProcess_CalcTangentSpace | 
        aiProcess_ValidateDataStructure | 
        aiProcess_RemoveRedundantMaterials | 
        aiProcess_JoinIdenticalVertices | 
        aiProcess_OptimizeMeshes | 
        aiProcess_FlipUVs | 
        aiProcess_LimitBoneWeights;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFileFromMemory(&data[0], data.size(), opts);
    if(!scene) {
        throw std::runtime_error(importer.GetErrorString());
    }

    /**
     * The result is created here. 
     */

    [[maybe_unused]] auto globalInverseTransform = scene->mRootNode->mTransformation.Inverse();
    /**
     * create_bone_names
     */
    std::unordered_map<std::string, int> boneNames;
    for(auto i: range(scene->mNumAnimations)) {
        const auto* animation = scene->mAnimations[i];
        for(auto j: range(animation->mNumChannels)) {
            const auto* animationChannel = animation->mChannels[j];
            std::string boneName(animationChannel->mNodeName.data);
            if(!boneNames.count(boneName))
                boneNames[boneName] = -1;
        }
    }
    model_content* res = new model_content;
    res->_meshes.resize(scene->mNumMeshes);
    res->_bones.resize(boneNames.size());
    /**
     * recursive function to get the bone node's world transformation
     */
    std::function<void(const aiNode*, const aiMatrix4x4&)> 
    read_node_hierarchy = [&] (const aiNode* node, const aiMatrix4x4& parentTransformation) 
    -> void {
        //DEBUG_VALUE_OF(node->mName.data);
        static int boneIndex = 0;
        //aiMatrix4x4 currentTransformation = node->mTransformation * parentTransformation;
        aiMatrix4x4 currentTransformation = parentTransformation * node->mTransformation;

        if(boneNames.count(node->mName.data)) {
            res->_bones[boneIndex].world = globalInverseTransform * currentTransformation;
            boneNames[node->mName.data] = boneIndex++;
        }
        for(auto i: range(node->mNumChildren)) {
            const auto* child = node->mChildren[i];
            read_node_hierarchy(child, currentTransformation);
        }
    };

    read_node_hierarchy(scene->mRootNode, scene->mRootNode->mTransformation);

    DEBUG_VALUE_OF(boneNames);


    //std::vector<mesh_content> entries(scene->mNumMeshes);

    //std::vector<size_t> indices;
    size_t numVertices = 0;
    size_t numIndices = 0;

    for(auto i: range(scene->mNumMeshes)) {
        const aiMesh* mesh = scene->mMeshes[i];
        for(auto j: range(mesh->mNumBones)) {
            const aiBone* bone = mesh->mBones[j];
            if(boneNames.count(bone->mName.data)) {
                res->_bones[boneNames[bone->mName.data]].offset = bone->mOffsetMatrix;
            }
        }
        res->_meshes[i].numVertices = mesh->mNumVertices;
        res->_meshes[i].numIndices = mesh->mNumFaces * 3;
        res->_meshes[i].baseVertex = numVertices;
        res->_meshes[i].baseIndex  = numIndices;
        res->_meshes[i].meshIndex  = i;

        numVertices += mesh->mNumVertices;
        numIndices  += res->_meshes[i].numIndices;
    }

    res->vertexBoneData.resize(numVertices);

    /**
     * observation:
     *  The vertexbuffer couldn't care less about the data layout, that's been decoupled. The challenge now
     *  is to get the data into the buffer, and get a more or less dynamic vertex layout. It's a bit of a 
     *  shame that I put as much time into the (very nicely) templated version. However, the general idea would
     *  be to select a vertex_p3_n3_t2 as a default, and a skinned_vertex_p3_n3_t2 (or similar syntax) when 
     *  the list of bones is not empty perhaps? Anyway: the selection of vertex layout seems to be a runtume 
     *  issue in general and has very little to do with the step where we create the vertex and index buffers
     *  (e.g.: this step)
     */

    for(auto& entry: res->_meshes) {
        const aiVector3D zero = { 0.f, 0.f, 0.f };
        const aiColor4D opaque_white = { 1.0f, 1.0f, 1.0f, 1.0f };

        const aiMesh* mesh = scene->mMeshes[entry.meshIndex];
        size_t vertex_size = 0;

        entry.vertex_info.push_back({
            kVertexStreamIndex::Position,
            kVertexStreamType::Vector3
        });

        vertex_size += sizeof(float) * 3;

        if(mesh->HasNormals()) {
            entry.vertex_info.push_back({
                kVertexStreamIndex::Normal,
                kVertexStreamType::Vector3
            });
            vertex_size += sizeof(float) * 3;
        }

        if(mesh->HasTextureCoords(0)) {
            entry.vertex_info.push_back({
                kVertexStreamIndex::TexCoord0,
                kVertexStreamType::Vector2
            });
            vertex_size += sizeof(float) * 2;
        }

        if(mesh->HasVertexColors(0)) {
            entry.vertex_info.push_back({
                kVertexStreamIndex::Color0,
                kVertexStreamType::Byte4
            });
        }

        if(mesh->mNumBones) {
            entry.vertex_info.push_back({
                kVertexStreamIndex::BlendWeight,
                kVertexStreamType::Vector4
            });
            vertex_size += sizeof(float) * 4;
            entry.vertex_info.push_back({
                kVertexStreamIndex::BlendIndex,
                kVertexStreamType::Short4
            });
            vertex_size += sizeof(int16_t) * 4;
        }

        [[maybe_unused]] vertex_buffer* vb = new vertex_buffer(vertex_size * mesh->mNumVertices);

        auto convert_vec3 = [] (const aiVector3D& a) {
            return vec3 { a.x, a.y, a.z };
        };

        auto convert_color = [] (const aiColor4D& c) {
            return SDL_Color { 
                static_cast<uint8_t>(c.r * 255.0f),
                static_cast<uint8_t>(c.g * 255.0f),
                static_cast<uint8_t>(c.b * 255.0f),
                static_cast<uint8_t>(c.a * 255.0f)
            };
        };

 
        for(auto j: range(entry.numVertices)) {
            const aiVector3D& position  = mesh->mVertices[j];
            const aiVector3D& normal    = mesh->HasNormals()        ? mesh->mNormals[j]             : zero;
            const aiVector3D& texcoord  = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j]    : zero;
            const aiColor4D& color      = mesh->HasVertexColors(0)  ? mesh->mColors[0][j]           : opaque_white; 
            //
            // TODO: change `res->` to `entry.`
            //
            res->positions.push_back(convert_vec3(position));
            if(mesh->HasNormals()) {
                res->normals.push_back(convert_vec3(normal));
            }
            if(mesh->HasTextureCoords(0)) {
                res->textureCoords.push_back(convert_vec3(texcoord));
            }
            if(mesh->HasVertexColors(0)) {
                res->colors.push_back(convert_color(color));
            }
        }

        for(auto j: range(mesh->mNumFaces)) {
            const aiFace* face = &mesh->mFaces[j];
            res->indices.push_back(face->mIndices[0]);
            res->indices.push_back(face->mIndices[1]);
            res->indices.push_back(face->mIndices[2]);
        }

        for(auto j: range(mesh->mNumBones)) {
            const aiBone* bone = mesh->mBones[j];
            std::string boneName(bone->mName.data);
            if(boneNames.count(boneName)) {
                size_t boneIndex = boneNames[boneName];
                for(auto k: range(bone->mNumWeights)) {
                    const auto& weight = bone->mWeights[k];
                    auto vertexId = entry.baseVertex + weight.mVertexId;
                    vertex_bone_data::add_bone_to(res->vertexBoneData[vertexId], (int)boneIndex, weight.mWeight);
                }
            }
        }
    }

    for(auto i: range(scene->mNumMaterials)) {
        const aiMaterial* material = scene->mMaterials[i];

        aiString name;
        aiGetMaterialString(material, AI_MATKEY_NAME, &name);
        DEBUG_VALUE_OF(std::string(name.C_Str()));
        
        aiColor4D color_ka;
        aiColor4D color_kd;
        aiColor4D color_ks;
        aiColor4D color_ke;
        aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &color_ka);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color_kd);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &color_ks);
        aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &color_ke);
        
        DEBUG_VALUE_OF(color_ka);
        DEBUG_VALUE_OF(color_kd);
        DEBUG_VALUE_OF(color_ks);
        
        aiString path_ka;
        aiString path_kd;
        aiString path_ks;
        aiString path_ke;

        aiGetMaterialTexture(material, aiTextureType_AMBIENT, 0, &path_ka, NULL, NULL, NULL, NULL, NULL);
        aiGetMaterialTexture(material, aiTextureType_DIFFUSE, 0, &path_kd, NULL, NULL, NULL, NULL, NULL);
        aiGetMaterialTexture(material, aiTextureType_SPECULAR, 0, &path_ks, NULL, NULL, NULL, NULL, NULL);
        aiGetMaterialTexture(material, aiTextureType_EMISSIVE, 0, &path_ke, NULL, NULL, NULL, NULL, NULL);
        DEBUG_VALUE_OF(std::string(path_kd.C_Str()));
        //for(auto t: range(material->GetTextureCount(aiTextureType_DIFFUSE))) {
        //TODO: enable texture loading here.... / for loops and alll

        //`}
        DEBUG_VALUE_OF(material);


    }
    return res;
}
