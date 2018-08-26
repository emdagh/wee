#include <core/rect.hpp>
#include <nlohmann/json.hpp>

namespace wee {

    void to_json(json& j, const rect& r) {
        j = json {
                {"x", r.x},
                {"y", r.y},
                {"w", r.w},
                {"h", r.h}
        };
    }
    void from_json(const json& j, rect& r) {
        r = rect {
                j.at("x").get<int32_t>(),
                j.at("y").get<int32_t>(),
                j.at("w").get<uint32_t>(),
                j.at("h").get<uint32_t>()
        };
    }

    std::ostream& operator << (std::ostream& os, const rect& rc) {
        json j;
        to_json(j, rc);
        return os << j;
    }
}
