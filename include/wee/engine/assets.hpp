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
#include <core/logstream.hpp>
#include <iostream>
#include <fstream>

SDL_RWops* SDL_RWFromStream(std::istream&);

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

        assets() {
            if(!TTF_WasInit()) {
                TTF_Init();
            }
        }

        virtual ~assets() {
            if(TTF_WasInit()) {
                TTF_Quit();
            }
        }

        TTF_Font* load(const std::string& name, int size, std::istream& is) {
            DEBUG_METHOD();
            DEBUG_VALUE_OF(name);
            DEBUG_VALUE_OF(size);
            if(resources.count(name) == 0) {
                resources[name] = TTF_OpenFontRW(SDL_RWFromStream(is), 0, size);

            }
            return resources.at(name);
        }
    };

    template <>
    struct assets<SDL_Texture> : singleton<assets<SDL_Texture> > {
        typename dictionary<SDL_Texture*>::type resources;

        assets() {
            //IMG_Init(IMG_INIT_PNG | IMG_INIT_GIF | IMG_INIT_JPG)
        }

        SDL_Texture* load(const std::string& name, std::istream& is) {
            DEBUG_METHOD();
            DEBUG_VALUE_AND_TYPE_OF(name);
            if(resources.count(name) == 0) {
                SDL_Surface* surface = IMG_Load_RW(SDL_RWFromStream(is), 0);
                if(!surface) {
                    throw std::runtime_error(IMG_GetError());
                }
                return from_surface(name, surface);
            }
            return resources[name];
        }
        
        SDL_Texture* from_surface(const std::string& name, SDL_Surface* surface) {
            DEBUG_METHOD();
            DEBUG_VALUE_AND_TYPE_OF(name);
            DEBUG_VALUE_AND_TYPE_OF(surface);


            if(!after) {
                throw std::logic_error("no surface -> texture callback defined for asset manager, did you forget to create an application instance?");
            }
            resources[name] = after(surface);
            int w, h;
            SDL_QueryTexture(resources[name], NULL, NULL, &w, &h);
            DEBUG_LOG("{}x{}", w, h);
            return resources[name];
        }

        SDL_Texture* get(const std::string& name) {
            if(resources.count(name) == 0) 
				throw std::out_of_range(name);
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
