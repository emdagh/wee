#include <engine/gui/menuscreen.hpp>

using namespace wee;



void menuscreen::from_json(const json& j) {
    try {
        if(j.find("items") != j.end()) {
            for(const auto& i : j.at("items")) {

            }
        }
    } catch(...) {
    }
    
    gamescreen::from_json(j);
}
