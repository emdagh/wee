#include <engine/gui/menuscreen.hpp>
#include <core/logstream.hpp>

using namespace wee;

void menuscreen::from_json(const json& j) {
    try {
        if(j.find("items") != j.end()) {
            for(const auto& i : j.at("items")) {
                DEBUG_VALUE_OF(i);
            }
        }
    } catch(...) {
    }
    
    gamescreen::from_json(j);
}
