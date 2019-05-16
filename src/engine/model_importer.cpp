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
#include <gfx/vertex_declaration.hpp>
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
            //DEBUG_VALUE_OF(m);
        }
        virtual void OnError(const char* m) {
            DEBUG_VALUE_OF(m);
        }
        virtual void OnInfo(const char* m) {
            //DEBUG_VALUE_OF(m);
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
                //Assimp::Logger::Debugging| 
                //Assimp::Logger::Info| 
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
        aiProcess_JoinIdenticalVertices | 
        aiProcess_OptimizeMeshes | 
        aiProcess_FlipUVs | 
        aiProcess_LimitBoneWeights |
        aiProcess_FixInfacingNormals
        ;

    std::map<std::string, size_t> materialNames;
    std::vector<material> materials;

    using vertex_p3_n3_t2 = vertex<
        attributes::position,
        attributes::normal,
        attributes::texcoord
    >;

        vec3 convert(const aiVector3D& In) {
            return vec3 { In.x, In.y, In.z };
        }

    model* import_impl(const aiScene* scene) {
        /**
         * The following structures hold all of the data across meshes
         * later on, we will compile this data into a single vertex / index buffer.
         */
        std::vector<aiVector3D> positions;
        std::vector<aiVector3D> normals;
        std::vector<aiVector3D> textureCoords;
        std::vector<uint32_t>   indices;

        aabb box;
        
        size_t numVertices = 0, numIndices = 0;
        std::vector<model_mesh> parts(scene->mNumMeshes);

        for(auto j: range(scene->mNumMeshes)) {
            const auto* mesh = scene->mMeshes[j];
            if(!mesh->HasTextureCoords(0)) {
                throw std::runtime_error("models are required to have texture coordinates!");
            }
            if(!mesh->HasNormals()) {
                throw std::runtime_error("models are required to have normals!");
            }

            for(auto i: range(mesh->mNumVertices)) {
                box.add( vec3f { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
                positions.push_back(mesh->mVertices[i]);
                normals.push_back(mesh->mNormals[i]);
                textureCoords.push_back(mesh->mTextureCoords[0][i]);
            }
            for(auto i: range(mesh->mNumFaces)) {
                const auto* face = &mesh->mFaces[i];
                indices.push_back(numIndices + face->mIndices[0]);
                indices.push_back(numIndices + face->mIndices[1]);
                indices.push_back(numIndices + face->mIndices[2]);
            }

            parts[j].base_index  = numIndices;
            parts[j].num_indices = mesh->mNumFaces * 3;
            parts[j].base_vertex = numVertices;
            parts[j].num_vertices= mesh->mNumVertices;
            
            numIndices  += parts[j].num_indices; 
            numVertices += parts[j].num_vertices; 
        }

        vertex_buffer* vb = new vertex_buffer(numVertices * sizeof(vertex_p3_n3_t2));
        index_buffer* ib  = new index_buffer(numIndices * sizeof(uint32_t));
        

        std::vector<vertex_p3_n3_t2> vertices(positions.size());

        for(auto i: range(positions.size())) {
            vec3 tmp = convert(textureCoords[i]);
            vec2 texcoord = vec2 { tmp.x, tmp.y };

            auto& vertex = vertices[i];
            vertex._position = convert(positions[i]);
            vertex._normal = convert(normals[i]);
            vertex._texcoord = texcoord;
        }

        vb->sputn(reinterpret_cast<char*>(&vertices[0]), sizeof(vertex_p3_n3_t2) * vertices.size());
        ib->sputn(reinterpret_cast<char*>(&indices[0]), sizeof(uint32_t) * indices.size());

        std::vector<material> materials;

        for(auto i: range(scene->mNumMaterials)) {
            [[maybe_unused]] const auto* mat = scene->mMaterials[i];
        }

        return new model {
            vb, ib, parts, materials, box
        }; 
    }

    model* import_model_from_file(const std::string& pt) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(pt, kImportOptions);
        if(!scene) {
            throw std::runtime_error(importer.GetErrorString());
        }
        return import_impl(scene);

    }

    model* import_model(std::istream& is) {
        std::vector<char> data((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(&data[0], data.size(), kImportOptions);
        if(!scene) {
            throw std::runtime_error(importer.GetErrorString());
        }
        return import_impl(scene);

    }
}
