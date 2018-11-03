#include "components.hpp"

using nlohmann::json;

namespace wee {
    void to_json(json& j, const basic_vec2<float>& v) {
        j = {
            { "x" , v.x },
            { "y" , v.y }
        };
    }

    void from_json(const json& j, basic_vec2<float>& v) {
        v.x = j["x"];
        v.y = j["y"];
    }
}
std::ostream& operator << (std::ostream& os, const terrain_t& t) {
    json j = {
        {"last", t.last},
        {"next", t.next}
    };
    return os << j;
}
std::ostream& operator << (std::ostream& os, const player_t&) {
    return os;
}

