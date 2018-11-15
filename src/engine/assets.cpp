#include <engine/assets.hpp>
#include <core/logstream.hpp>
#include <SDL.h>
#include <SDL_mixer.h>
#include <cstring>

#ifdef _WIN32
#define PATH_SEP '\\'
#else
#define PATH_SEP  '/'
#endif

#if defined(__IOS__)
extern "C" const char* iOS_getDataPath();
#endif

Sint64 istream_seek( struct SDL_RWops *context, Sint64 offset, int whence)
{
	std::istream* stream = (std::istream*) context->hidden.unknown.data1;

	if ( whence == SEEK_SET )
		stream->seekg ( offset, std::ios::beg );
	else if ( whence == SEEK_CUR )
		stream->seekg ( offset, std::ios::cur );
	else if ( whence == SEEK_END )
		stream->seekg ( offset, std::ios::end );

	return stream->fail() ? -1 : (int)stream->tellg();
}


size_t istream_read(SDL_RWops *context, void *ptr, size_t size, size_t maxnum)
{
	if ( size == 0 ) return -1;
	std::istream* stream = (std::istream*) context->hidden.unknown.data1;
	stream->read( (char*)ptr, size * maxnum );

	return stream->bad() ? -1 : (size_t)stream->gcount() / size;
}

int istream_close( SDL_RWops *context )
{
	if ( context ) {
		SDL_FreeRW( context );
	}
	return 0;
}

Sint64 istream_size(SDL_RWops* io) {
	std::istream* stream = (std::istream*) io->hidden.unknown.data1;
	std::streamoff offset = stream->tellg();
	stream->seekg(0, std::ios::end);
	std::streamoff ret = stream->tellg();
	stream->seekg(offset, std::ios::beg);
	return ret;
}

SDL_RWops* SDL_RWFromStream(std::istream& is) {
    SDL_RWops *rwops;
    rwops = SDL_AllocRW();

    if ( rwops != NULL ) 
    {
		rwops->size = istream_size;
        rwops->seek = istream_seek;
        rwops->read = istream_read;
        rwops->write = NULL;
        rwops->close = istream_close;
        rwops->hidden.unknown.data1 = &is;
    }
    return rwops;
}


namespace wee {

    std::string dirname(const std::string& in) {
        char* path = const_cast<char*>(in.c_str());
        constexpr static const char* dot = ".";
        char* last_delim = !in.empty() ? strrchr(path, '/') : NULL;
        if(last_delim == path) {
            ++last_delim;
        } else if(last_delim && last_delim[1] == '\0') {
            last_delim = (char*)memchr(path, last_delim - path, '/');
        }

        if(last_delim) {
            last_delim[0] = '\0';

        } else {
            path = (char*)dot;
        }

        return std::string(path);
    }

    std::string basename(const std::string& in) {
        const char* p = strrchr(in.c_str(), PATH_SEP);
        return p ? std::string(p + 1) : in;
    }
    std::ifstream open_ifstream(const std::string& pt) {
        std::string base_name = wee::basename(pt);
        std::string abs_path = get_resource_path(dirname(pt)) + basename(pt);
        DEBUG_LOG("loading: " + abs_path);
        std::ifstream res(abs_path);
        if(!res.is_open()) {
            throw file_not_found(abs_path);
        }
        return res;
    }

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
        std::string res = subDir.empty() ? baseRes : baseRes + subDir + PATH_SEP;
#else
        std::string res = std::string(iOS_getDataPath());
#endif
        std::replace(res.begin(), res.end(), '/', PATH_SEP);
        return res;
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
