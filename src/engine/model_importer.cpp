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
#include <engine/model_importer.hpp>
#include <vector>
#include <cstddef>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <prettyprint.hpp>

#define kMaxBonesPerVertex  4

using namespace wee;
       
namespace wee {
    struct vertex_channel_info {
        kVertexStreamIndex      index;
        kVertexStreamType       type;
    };
    using vertex_declaration_info = std::vector<vertex_channel_info>; 

    struct bone_info {
        int _parentIndex; // int bcause we need -1 for root bone
        aiMatrix4x4 world, offset;
        std::string _name;
    };
    struct vertex_bone_data {
        int   _id[kMaxBonesPerVertex];
        float _weight[kMaxBonesPerVertex];
        static void add_bone_to(vertex_bone_data& data, int bone, float weight) {
            for(auto i: range(kMaxBonesPerVertex)) {
                if(data._weight[i] == 0.0f) {
                    data._id[i] = bone;
                    data._weight[i] = weight;
                    return;
                }
            }
        }
    };


    struct mesh_entry {
        size_t baseVertex;
        size_t baseIndex;
        size_t numVertices;
        size_t numIndices;
        size_t meshIndex;
        vertex_declaration_info vertex_info;
    };
    struct material_entry;
}

mat4f convert(const aiMatrix4x4& val) {
    return mat4f {
        val.a1, val.a2, val.a3, val.a4,
        val.b1, val.b2, val.b3, val.b4,
        val.c1, val.c2, val.c3, val.c4,
        val.d1, val.d2, val.d3, val.d4
    };
}

model* model_importer::import(std::istream& is) const {
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

    [[maybe_unused]] auto globalInverseTransform = scene->mRootNode->mTransformation.Inverse();
    /**
     * create_bone_names
     */
    std::unordered_map<std::string, int> bones;
    for(auto i: range(scene->mNumAnimations)) {
        const auto* animation = scene->mAnimations[i];
        for(auto j: range(animation->mNumChannels)) {
            const auto* animationChannel = animation->mChannels[j];
            std::string boneName(animationChannel->mNodeName.data);
            if(!bones.count(boneName))
                bones[boneName] = -1;
        }
    }
    /** 
     * this will hold the bone's information
     */
    std::vector<bone_info> info(bones.size());
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

        if(bones.count(node->mName.data)) {
            info[boneIndex].world = globalInverseTransform * currentTransformation;
            bones[node->mName.data] = boneIndex++;
        }
        for(auto i: range(node->mNumChildren)) {
            const auto* child = node->mChildren[i];
            read_node_hierarchy(child, currentTransformation);
        }
    };

    read_node_hierarchy(scene->mRootNode, scene->mRootNode->mTransformation);

    DEBUG_VALUE_OF(bones);

    std::vector<mesh_entry> entries(scene->mNumMeshes);
    std::vector<aiVector3D> positions, normals, textureCoords;
    std::vector<vertex_bone_data> vertexBoneData;
    std::vector<size_t> indices;
    size_t numVertices = 0;
    size_t numIndices = 0;

    for(auto i: range(scene->mNumMeshes)) {
        const aiMesh* mesh = scene->mMeshes[i];
        for(auto j: range(mesh->mNumBones)) {
            const aiBone* bone = mesh->mBones[j];
            if(bones.count(bone->mName.data)) {
                info[bones[bone->mName.data]].offset = bone->mOffsetMatrix;
            }
        }
        entries[i].numIndices = mesh->mNumFaces * 3;
        entries[i].baseVertex = numVertices;
        entries[i].baseIndex  = numIndices;
        entries[i].meshIndex  = i;

        numVertices += mesh->mNumVertices;
        numIndices  += entries[i].numIndices;
    }

    vertexBoneData.resize(numVertices);

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

    for(auto& entry: entries) {
        const aiVector3D zero = { 0.f, 0.f, 0.f };

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
 
        for(auto j: range(entry.numVertices)) {
            const aiVector3D& position  = mesh->mVertices[j];
            const aiVector3D& normal    = mesh->HasNormals() ? mesh->mNormals[j] : zero;
            const aiVector3D& texcoord  = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][j] : zero;
            
            positions.push_back(position);
            if(mesh->HasNormals()) {
                normals.push_back(normal);
            }
            if(mesh->HasTextureCoords(0)) {
                textureCoords.push_back(texcoord);
            }
        }

        for(auto j: range(mesh->mNumFaces)) {
            const aiFace* face = &mesh->mFaces[j];
            indices.push_back(face->mIndices[0]);
            indices.push_back(face->mIndices[1]);
            indices.push_back(face->mIndices[2]);
        }

        for(auto j: range(mesh->mNumBones)) {
            const aiBone* bone = mesh->mBones[j];
            std::string boneName(bone->mName.data);
            if(bones.count(boneName)) {
                size_t boneIndex = bones[boneName];
                for(auto k: range(bone->mNumWeights)) {
                    const auto& weight = bone->mWeights[k];
                    auto vertexId = entry.baseVertex + weight.mVertexId;
                    vertex_bone_data::add_bone_to(vertexBoneData[vertexId], (int)boneIndex, weight.mWeight);
                }
            }
        }
    }

    for(auto i: range(scene->mNumMaterials)) {
        const aiMaterial* material = scene->mMaterials[i];
        //for(auto t: range(material->GetTextureCount(aiTextureType_DIFFUSE))) {
        //TODO: enable texture loading here.... / for loops and alll

        //`}
        DEBUG_VALUE_OF(material);
    }

     

    return nullptr;
}
