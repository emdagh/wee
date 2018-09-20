#pragma once

#include <wee/wee.hpp>
#include <core/singleton.hpp>
#include <iosfwd>
#include <map>
#include <string>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <core/delegate.hpp>
#include <core/factory.hpp>
#include <util/logstream.hpp>
#include <iostream>
#include <fstream>

SDL_RWops* SDL_RWFromStream(std::istream& is);

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
    struct assets<TTF_Font> : singleton<assets<TTF_Font> > {
        typename dictionary<TTF_Font*>::type resources;

        TTF_Font* load(const std::string& name, std::istream& is) {
            return NULL;
        }
    };

    template <>
    struct assets<SDL_Texture> : singleton<assets<SDL_Texture> > {
        typename dictionary<SDL_Texture*>::type resources;

        SDL_Texture* load(const std::string& name, std::istream& is) {
            if(resources.count(name) == 0) {
                SDL_Surface* surface = IMG_Load_RW(SDL_RWFromStream(is), 0);
                return from_surface(name, surface);
            }
            return resources[name];
        }
        
        SDL_Texture* from_surface(const std::string& name, SDL_Surface* surface) {
            if(!after) {
                throw std::logic_error("no callback defined for asset manager");
            }
            resources[name] = after(surface);
            return resources[name];
        }

        SDL_Texture* get(const std::string& name) {
            if(resources.count(name) == 0) throw std::out_of_range(name);
            return resources[name];
        }

        delegate<SDL_Texture*(SDL_Surface*)> after;
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


}
