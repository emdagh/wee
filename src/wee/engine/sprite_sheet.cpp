#include <engine/sprite_sheet.hpp>
#include <engine/assets.hpp>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include <core/logstream.hpp>

void to_json(json& j, const SDL_Rect& r) {
    j = json {
            {"x", r.x},
            {"y", r.y},
            {"w", r.w},
            {"h", r.h}
    };
}

void from_json(const json& j, SDL_Rect& r) {
    r = SDL_Rect {
            j.at("x").get<int>(),
            j.at("y").get<int>(),
            j.at("w").get<int>(),
            j.at("h").get<int>()
    };
}

namespace wee {
    void to_json(json& j, const sprite_sheet& s) {
        for(auto it=s._ix.begin(); it != s._ix.end(); it++) {
            j["frames"][(*it).first] = s.get((*it).first);
        }
    }

    void from_json(const json& j, sprite_sheet& s) {
        auto frames = j["frames"];
        for(auto it = frames.begin(); it != frames.end(); it++) {
            SDL_Rect frame = it.value()["frame"];
            s.add(it.key(), frame);
        }

        std::ifstream is;

        std::string image_path = get_resource_path("") + j["meta"]["image"].get<std::string>();

        is.open(image_path, std::ios::binary);
        if(is.is_open()) {
            s._texture= assets<SDL_Texture>::instance().load(j["meta"]["image"], is);
        } else {
            throw file_not_found(image_path);
        }
    }
    std::ostream& operator << (std::ostream& os, const sprite_sheet& s) {
        json j;
        to_json(j, s);
        return os << j;
    }
}
