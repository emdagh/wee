#include <nlohmann/json.hpp>
#include <iostream>
#include <engine/assets.hpp>
#include <prettyprint.hpp>

using wee::get_resource_path;
using nlohmann::json;

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

int main(int argc, char* argv[]) {

    std::ifstream is(get_resource_path("") + "assets/img/skeleton.json");

    json j;
    j << is;

    auto frames = j["frames"];
    for(auto it = frames.begin(); it != frames.end(); it++) {
        //SDL_Rect frame = it.value()["frame"];
        DEBUG_VALUE_AND_TYPE_OF(it.value()["frame"]);
    }


    //std::map<std::string, std::vector<std::string> > animation_names = j["animations"];

    //std::cout << animation_names << std::endl;
    
    /*iterate(j["animations"], [&] (const json& js) {
        std::cout << js << std::endl;
    });*/

    return 0;
}
