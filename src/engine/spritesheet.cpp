#include <engine/spritesheet.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <util/logstream.hpp>

namespace wee {
    void to_json(json& j, const spritesheet& s) {
        for(auto it=s.index.begin(); it != s.index.end(); it++) {
            j["frames"][(*it).first] = s.frames[(*it).second];
        }
    }

    void from_json(const json& j, spritesheet& s) {

        auto frames = j["frames"];
        for(auto it = frames.begin(); it != frames.end(); it++) {
            rect frame = it.value()["frame"];
            s.add(it.key(), frame);
        }

    }
    std::ostream& operator << (std::ostream& os, const spritesheet& s) {
        json j;
        to_json(j, s);
        return os << j;
    }
}
