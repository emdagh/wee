#pragma once

#include <nlohmann/json_fwd.hpp>

using nlohmann::json;

namespace wee {

    struct rect {
        int32_t x, y;
        uint32_t w, h;
    };

    void to_json(json& j, const rect& r);
    void from_json(const json& j, rect& r);

    std::ostream& operator << (std::ostream& os, const rect& rc);
}
