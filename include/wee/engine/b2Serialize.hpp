#pragma once

#include <nlohmann/json.hpp>

namespace wee {
    void to_json(json& j, const b2Shape* s) {
    }
    void to_json(json& j, const b2Fixture* f) {
    }
    void to_json(json& j, const b2Body* b) {

        /*for(auto* f=b->GetFixtureList(); f; f = f->GetNext()) {
            json _;
            to_json(_, f);
            j["fixtures"].push_back("fixture", _);
        }*/
    }


}
