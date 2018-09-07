#include <engine/spritesheet.hpp>
#include <engine/assets.hpp>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <util/logstream.hpp>

void to_json(json& j, const SDL_Rect& r) {
    j = json {
            {"x", r.x},
            {"y", r.y},
            {"w", r.w},
            {"h", r.h}
    };
}

void from_json(const json& j, SDL_Rect& r) {
    r = (SDL_Rect) {
            j.at("x").get<int>(),
            j.at("y").get<int>(),
            j.at("w").get<int>(),
            j.at("h").get<int>()
    };
}

namespace wee {
    void to_json(json& j, const spritesheet& s) {
        for(auto it=s.index.begin(); it != s.index.end(); it++) {
            j["frames"][(*it).first] = s.frames[(*it).second];
        }
    }

    void from_json(const json& j, spritesheet& s) {
        auto frames = j["frames"];
        for(auto it = frames.begin(); it != frames.end(); it++) {
            SDL_Rect frame = it.value()["frame"];
            s.add(it.key(), frame);
        }

        std::ifstream is;
        is.open((j["meta"]["image"]));
        if(is.is_open()) {
            s.texture= assets<SDL_Texture>::instance().load(j["meta"]["image"], is);
        }
    }
    std::ostream& operator << (std::ostream& os, const spritesheet& s) {
        json j;
        to_json(j, s);
        return os << j;
    }
}
