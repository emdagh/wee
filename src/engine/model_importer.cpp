#include <engine/model_importer.hpp>
#include <core/range.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/Logger.hpp>
#include <assimp/LogStream.hpp>
#include <vector>
#include <string>
#include <gfx/model.hpp>
#include <functional>
#include <prettyprint.hpp>

using namespace wee;

class ailogstream:  public Assimp::LogStream {
public:
    void write(const char* message) {
        DEBUG_LOG(message);
    }
};

#define kMaxBonesPerVertex  4
/*
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
};*/

namespace wee {

    class AssimpLogger : public Assimp::Logger {
        Assimp::LogStream* _stream = nullptr;
    public:
        virtual bool attachStream(Assimp::LogStream* s, unsigned int i) {
            if(_stream != nullptr) {
                detatchStream(_stream, i);
            }
            return (_stream = s);
        }

        virtual bool detatchStream(Assimp::LogStream* s, unsigned int) {
            if(s == _stream) {
                return (_stream = nullptr);

            }
            return false;
        }
        virtual void OnDebug(const char* m) {
            DEBUG_VALUE_OF(m);
        }
        virtual void OnError(const char* m) {
            DEBUG_VALUE_OF(m);
        }
        virtual void OnInfo(const char* m) {
            DEBUG_VALUE_OF(m);
        }
        virtual void OnWarn(const char* m) {
            DEBUG_VALUE_OF(m);
        }
    };

    static class gRegisterAssimpLogstream {
    public:
        gRegisterAssimpLogstream() {
            Assimp::DefaultLogger::set(new AssimpLogger);
            const unsigned int severity = 
                Assimp::Logger::Debugging| 
                Assimp::Logger::Info| 
                Assimp::Logger::Err| 
                Assimp::Logger::Warn;
            // Attaching it to the default logger
            Assimp::DefaultLogger::get()->attachStream( new ailogstream(), severity );
        }
    } _registerAssimpLogStream;

    const static auto kImportOptions = 
        aiProcess_GenSmoothNormals | 
        aiProcess_Triangulate | 
        aiProcess_CalcTangentSpace | 
        aiProcess_ValidateDataStructure | 
        aiProcess_RemoveRedundantMaterials | 
        // aiProcess_JoinIdenticalVertices | 
        aiProcess_OptimizeMeshes | 
        aiProcess_FlipUVs | 
        aiProcess_LimitBoneWeights;

    std::map<std::string, size_t> materialNames;
    std::vector<material> materials;

    void import_mesh(const aiScene* scene, const aiMesh* mesh) {
        size_t vertex_size = sizeof(vec3f);
        /**
         * TODO: split model mesh into model mesh parts
         * these should be groupable by material index so they may
         * be batched.
         */

        DEBUG_VALUE_OF(mesh->mMaterialIndex);
        /*using int4 = int[4];
          using float4 = float[4];

          if(mesh->HasNormals()) {
          vertex_size += sizeof(vec3f); 
          }

          if(mesh->HasTextureCoords(0)) {
          vertex_size += sizeof(vec3f); 
          }

          if(mesh->mNumBones > 0) {
          vertex_size += sizeof(float4);
          vertex_size += sizeof(int4);
          }*/

        using index_type = uint32_t;
        static const size_t index_size = sizeof(index_type);

        vertex_buffer* vb = new vertex_buffer(vertex_size * mesh->mNumVertices);
        index_buffer* ib  = new index_buffer(index_size * mesh->mNumFaces * 3);

        std::vector<vec3> positions;

        for(auto i: range(mesh->mNumVertices)) {
            const auto position = mesh->mVertices[i];
            positions.push_back(vec3 {position.x, position.y, position.z});
        }

        vb->sputn(reinterpret_cast<char*>(&positions[0]), vertex_size * positions.size());

        std::vector<index_type> indices;
        for(auto i: range(mesh->mNumFaces)) {
            const auto* face = &mesh->mFaces[i];
            indices.push_back(face->mIndices[0]);
            indices.push_back(face->mIndices[1]);
            indices.push_back(face->mIndices[2]);
        }
        ib->sputn(reinterpret_cast<char*>(&indices[0]), index_size * indices.size());

        for(auto i: range(mesh->mNumBones)) {
            [[maybe_unused]] const auto* bone = mesh->mBones[i];
        }
    }

    void import_material(const aiScene* scene, aiMaterial* mat) {
        DEBUG_METHOD();
        auto convert_color = [] (const aiColor4D& c) {
            return SDL_Color { 
                static_cast<uint8_t>(c.r * 255.0f),
                    static_cast<uint8_t>(c.g * 255.0f),
                    static_cast<uint8_t>(c.b * 255.0f),
                    static_cast<uint8_t>(c.a * 255.0f)
            };
        };
        std::string key(mat->GetName().C_Str());
        size_t value = materials.size();
        materialNames.insert({key, value});

        aiColor4D color_kd;
        aiGetMaterialColor(mat, AI_MATKEY_COLOR_DIFFUSE, &color_kd);
        materials.push_back({ 
                convert_color(color_kd)

                });
    }

    model* import(const aiScene* scene) {

        materialNames.clear();
        materials.clear();
        // Select the kinds of messages you want to receive on this log stream

        DEBUG_VALUE_OF(scene->mNumMaterials);
        for(auto i: range(scene->mNumMaterials)) {
            import_material(scene, scene->mMaterials[i]);
        }
        DEBUG_VALUE_OF(materialNames);

        for(auto i: range(scene->mNumMeshes)) {
            import_mesh(scene, scene->mMeshes[i]);
        }


        for(auto i: range(scene->mNumAnimations)) {
            [[maybe_unused]] const auto* mat = scene->mMaterials[i];
        }

        [[maybe_unused]] const auto* node = scene->mRootNode;

        return new model;
    }

    model* import_model_from_file(const std::string& pt) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(pt, kImportOptions);
        if(!scene) {
            throw std::runtime_error(importer.GetErrorString());
        }
        return import(scene);

    }

    model* import_model(std::istream& is) {
        std::vector<char> data((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(&data[0], data.size(), kImportOptions);
        if(!scene) {
            throw std::runtime_error(importer.GetErrorString());
        }
        return import(scene);
        /*
           std::function<void(const aiNode*, std::function<void(const aiNode*)>)>
           iterate = [&] (const aiNode* node, std::function<void(const aiNode*)> fn)
           {
           fn(node);
           for(auto i: range(node->mNumChildren)) {
           iterate(node->mChildren[i], fn);
           }
           };
           iterate(scene->mRootNode, [] (const aiNode* node) {
           DEBUG_VALUE_OF(node->mName.C_Str());
           DEBUG_VALUE_OF(node->mNumMeshes);
           });
           */


    }
}
