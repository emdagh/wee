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
std::ostream& operator << (std::ostream& os, const collider_t&) {
    return os << "collider_t";
}

std::ostream& operator << (std::ostream& os, const rigidbody_t& rb) {
    return os << rb.body;
}
std::ostream& operator << (std::ostream& os, const transform_t&) {
    return os;
}
std::ostream& operator << (std::ostream& os, const nested_t& ) {
    return os;
}
std::ostream& operator << (std::ostream& os, const visual_t& ) {
    return os;
}
std::ostream& operator << (std::ostream& os, const terrain_t& t) {
    json j = {
        {"last", t.last},
        {"next", t.next}
    };
    return os << j;
}

std::ostream& operator << (std::ostream& os, const input_t& t) {
    return os << t.mouse_is_down;
}
