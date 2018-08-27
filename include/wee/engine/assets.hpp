#pragma once

#include <core/singleton.hpp>
#include <iosfwd>
#include <map>
#include <string>


namespace wee {


    struct assets : singleton<assets> {
        std::map<std::string, size_t> surface;
        std::map<std::string, size_t> texture;
        std::map<std::string, size_t> sound;
        std::map<std::string, size_t> music;
    
        std::string get_resource_path(const std::string&);
        

    };


    /*uintptr_t load_surface(std::istream&);
    uintptr_t load_texture();
    uintptr_t load_sound();
    uintptr_t load_music();*/


    /*
     * usage::
     * std::ifstream is("foo/path");
     * SDL_Texture* my_image;
     * is >> my_image;
     */

}
