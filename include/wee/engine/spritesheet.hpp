#pragma once

#include <nlohmann/json_fwd.hpp>
#include <map>
#include <iosfwd>
#include <SDL.h>

using nlohmann::json;

void to_json(json&, const SDL_Rect&);
void from_json(const json&, SDL_Rect&);

namespace wee {
    struct spritesheet {
        std::map<std::string, size_t> index;
        std::vector<SDL_Rect> frames;
        SDL_Texture* texture;

        void add(const std::string& key, const SDL_Rect& frame) {
            frames.push_back(frame);
            index[key] = frames.size() - 1;
        }
        int get(const std::string& key, SDL_Rect* frame) {
            auto it = index.find(key);
            if(it != index.end()) {
                *frame = frames[(*it).second];
                return 0;
            }
            return -1;
        }
    };

    
    void to_json(json& j, const spritesheet&);
    void from_json(const json& j, spritesheet&);

    std::ostream& operator << (std::ostream&, const spritesheet&);
}
