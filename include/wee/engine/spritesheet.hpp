#pragma once

#include <core/rect.hpp>
#include <nlohmann/json_fwd.hpp>
#include <map>
#include <iosfwd>

namespace wee {
    struct spritesheet {
        std::map<std::string, size_t> index;
        std::vector<rect> frames;

        void add(const std::string& key, const rect& frame) {
            frames.push_back(frame);
            index[key] = frames.size() - 1;
        }
    };

    
    void to_json(json& j, const spritesheet&);
    void from_json(const json& j, spritesheet&);

    std::ostream& operator << (std::ostream&, const spritesheet&);
}
