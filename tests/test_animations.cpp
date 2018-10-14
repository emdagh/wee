#include <nlohmann/json.hpp>
#include <iostream>
#include <engine/assets.hpp>
#include <prettyprint.hpp>

using wee::get_resource_path;
using nlohmann::json;

void to_json(json& j, const SDL_Rect& r) {
    j = json {
        {"x", r.x},
        {"y", r.y},
        {"w", r.w},
        {"h", r.h}
    };
}

std::ostream& operator << (std::ostream& os, const SDL_Rect& a) {
    json j;
    to_json(j, a); 
    return os << j;
}

struct animation_clip {
    std::string name;
    size_t begin, end;
    bool loop;
};

struct keyframe {
    size_t time;
};

struct animation_info {
    size_t duration;
    std::vector<keyframe> frames;
};

struct animation_controller {
    std::vector<animation_clip> clips;
    std::vector<animation_info> info;

    bool is_finished;
    bool is_playing;

    float current_time;
};

template <typename T>
void iterate(const json& j, T f) {
    for(auto it=j.begin(); it != j.end(); it++) {
        if(it->is_structured()) {
            iterate(*it, f);
        } else {
            f(*it);
        }
    }
}

#include <engine/sprite_sheet.hpp>

using namespace wee;

struct animation {
    sprite_sheet _sheet;
    std::map<std::string, std::vector<size_t> > _animations;



};

void to_json(json& j, const animation& a) {

}

void from_json(const json& j, animation& a) {

    a._sheet = j;
    const auto& frames = j["animations"];
    for(auto it = frames.begin(); it != frames.end(); it++) {
        for(const auto& j : it.value()) {
            a._animations[it.key()].push_back(a._sheet.index_of(j));
        }

    }
}

int main(int argc, char* argv[]) {

    std::ifstream is(get_resource_path("") + "assets/img/skeleton.json");

    json j;
    j << is;

    sprite_sheet s = j;

    std::map<std::string, std::vector<size_t> > _index;

    const auto& frames = j["animations"];
    for(auto it = frames.begin(); it != frames.end(); it++) {
        for(const auto& j : it.value()) {
            _index[it.key()].push_back(s.index_of(j));
        }

    }
    DEBUG_VALUE_OF(_index);


#if 0
    auto frames = j["frames"];
    std::map<std::string, SDL_Rect> _frames;
    for(auto it = frames.begin(); it != frames.end(); it++) {
        const auto& jj = it.value()["frame"];
        SDL_Rect r = (SDL_Rect) {
            jj.at("x").get<int>(),
            jj.at("y").get<int>(),
            jj.at("w").get<int>(),
            jj.at("h").get<int>()
        };
        _frames[it.key()] = r;
    }
    DEBUG_VALUE_OF(_frames);
    auto animations = j["animations"];
    for(const auto& a: animations) {
        DEBUG_VALUE_AND_TYPE_OF(a);
    }
    auto image_path = j["meta"]["image"];
#endif
    return 0;
}
