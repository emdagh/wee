#pragma once

#include <nlohmann/json_fwd.hpp>
#include <map>
#include <iosfwd>
#include <SDL.h>
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define SDL_RMASK   0xff000000
#define SDL_GMASK   0x00ff0000
#define SDL_BMASK   0x0000ff00
#define SDL_AMASK   0x000000ff
#else
#define SDL_AMASK   0xff000000
#define SDL_BMASK   0x00ff0000
#define SDL_GMASK   0x0000ff00
#define SDL_RMASK   0x000000ff
#endif

using nlohmann::json;

void to_json(json&, const SDL_Rect&);
void from_json(const json&, SDL_Rect&);

namespace wee {
    template <typename T>
    struct basic_sprite_sheet {
        std::map<T, size_t> _ix;
        std::vector<SDL_Rect> _rects;
        SDL_Texture* _texture;

        const SDL_Rect& operator [] (size_t i) const { return _rects[i]; }

        const size_t index_of(const std::string& name) const {
            return _ix.at(name);
        }

        const SDL_Rect& get(const T& key)  {
            return static_cast<const basic_sprite_sheet<T>* >(this)->get(key); 
        }

        const SDL_Rect& get(const T& key) const {
            if(_ix.count(key) == 0) {
                throw std::out_of_range("out of range");
            }

            return _rects.at(_ix.at(key));//_ix[key]];
        }

        void add(const T& key, const SDL_Rect& value) {
            if(_ix.count(key) == 0) {
                _ix[key] = _rects.size();
                _rects.push_back(value);
            }
        }
    };  

    typedef basic_sprite_sheet<std::string> sprite_sheet;

    void to_json(json& j, const sprite_sheet&);
    void from_json(const json& j, sprite_sheet&);

    std::ostream& operator << (std::ostream&, const sprite_sheet&);
}
