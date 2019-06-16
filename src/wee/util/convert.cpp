#include <core/logstream.hpp>
#include <core/range.hpp>
#include <engine/assets.hpp>
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/Logger.hpp>
#include <assimp/LogStream.hpp>

using namespace wee;

int main(int argc, char** argv) {
    DEBUG_VALUE_OF(argc);
    for(auto i: range(argc)) {
        DEBUG_VALUE_OF(argv[i]);
    }

    if(argc < 2) {
        DEBUG_LOG("invalid number of arguments");
    }
    
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


    Assimp::Importer importer;
    Assimp::Exporter exporter;
    /*
    for(auto i: range(exporter.GetExportFormatCount())) {
        auto* formatDescription = exporter.GetExportFormatDescription(i);
        DEBUG_VALUE_OF(formatDescription->description);
        DEBUG_VALUE_OF(formatDescription->fileExtension);
        DEBUG_VALUE_OF(formatDescription->id);
    }
    */
    auto fileExtensionOut = extension(argv[2]);
    
    [[maybe_unused]] const aiScene* scene = importer.ReadFile(argv[1], kImportOptions);
    return exporter.Export(
        scene, 
        fileExtensionOut.c_str(), 
        argv[2]
    );
}
