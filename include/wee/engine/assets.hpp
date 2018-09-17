#pragma once

#include <wee/wee.hpp>
#include <core/singleton.hpp>
#include <iosfwd>
#include <map>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <core/delegate.hpp>
#include <core/factory.hpp>
#include <util/logstream.hpp>
#include <iostream>
#include <fstream>

namespace wee {

    template <typename T>
    struct dictionary {
        typedef std::map<std::string, T> type;
    };

    


    std::string get_resource_path(const std::string&);

    template <typename T>
    struct assets : singleton<assets<T> > {
        T* load(const std::string&, std::istream&) { 
            throw not_implemented();
        }
    };

    template <>
    struct assets<SDL_Texture> : singleton<assets<SDL_Texture> > {
        typename dictionary<SDL_Texture*>::type resources;

        SDL_Texture* load(const std::string& name, std::istream& is) {

            DEBUG_METHOD();
            DEBUG_VALUE_OF(name);
            if(resources.count(name) == 0) {


                std::istreambuf_iterator<char> eos;
                std::string contents(std::istreambuf_iterator<char>(is),
                                     (std::istreambuf_iterator<char>()));

                SDL_RWops* ops = SDL_RWFromConstMem(contents.c_str(), (int)contents.length());
                SDL_Surface* surface = IMG_Load_RW(ops, 0);
                resources[name] = after(surface);
            }
            return resources[name];
        }
        
        SDL_Texture* from_surface(const std::string& name,
                SDL_Surface* surface) {
            resources[name] = after(surface);
            return resources[name];
        }

        SDL_Texture* get(const std::string& name) {
            if(resources.count(name) == 0) throw std::out_of_range(name);
            return resources[name];
        }

        delegate<SDL_Texture*(SDL_Surface*)> after;
    };

    struct file_not_found : public std::runtime_error {
        explicit file_not_found(const std::string& what) : std::runtime_error("file not found: " + what) {
        }
    };

    namespace asset_helper {
        template <typename T>
        T* from_file(const std::string& name, const std::string& pt) {
            std::string abs_path = get_resource_path("") + pt;
            DEBUG_VALUE_OF(abs_path);
            std::ifstream is;
            is.open(abs_path);
            if(!is.is_open()) { 
                throw file_not_found(abs_path);
                return NULL;
            }
            return assets<T>::instance().load(name, is);
        }
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
