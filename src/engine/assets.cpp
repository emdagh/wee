#include <engine/assets.hpp>
#include <util/logstream.hpp>
#include <SDL.h>
#include <SDL_mixer.h>

#ifdef _WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP  "/"
#endif

#if defined(__IOS__)
extern "C" const char* iOS_getDataPath();
#endif


SDL_RWops* SDL_RWFromStream(std::istream& is) {
    std::istreambuf_iterator<char> eos;
    std::string contents(std::istreambuf_iterator<char>(is),
        (std::istreambuf_iterator<char>())
    );

    auto* ptr = SDL_RWFromConstMem(contents.c_str(), (int)contents.length());
    if(!ptr) {
        throw std::runtime_error(SDL_GetError());
    }
    return ptr;
}

namespace wee {
    std::string get_resource_path(const std::string& subDir) {
#if !defined(__IOS__)
        static std::string baseRes;
        if (baseRes.empty()){
            //
            //SDL_GetBasePath will return NULL if something went wrong in retrieving the path
            //
            char *basePath = SDL_GetBasePath();
            if (basePath){
                baseRes = basePath;
                SDL_free(basePath);
            }
            else {
                //std::cerr << "Error getting resource path: " << SDL_GetError() << std::endl;
                return DEBUG_LOG(SDL_GetError()), "";
            }
        }
        //If we want a specific subdirectory path in the resource directory
        //append it to the base path. This would be something like Lessons/res/Lesson0
        return subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
#else
        return std::string(iOS_getDataPath());
#endif
    }
}

using namespace wee;

std::istream& operator >> (std::istream& is, SDL_Surface*) {
    return is;
}
std::istream& operator >> (std::istream& is, SDL_Texture*) {
    return is;
}
std::istream& operator >> (std::istream& is, Mix_Chunk*) {
    return is;
}
std::istream& operator >> (std::istream& is, Mix_Music*) {
    return is;
}
