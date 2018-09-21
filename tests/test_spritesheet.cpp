#include <engine/sprite_sheet.hpp>
#include <util/logstream.hpp>
#include <util/parallel.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using nlohmann::json;
using wee::sprite_sheet;

template <typename T>
void iterate(const json& j, T f) {
    for(auto it=j.begin(); it != j.end(); it++) {
        if(it->is_structured()) {
            iterate(*it, f);
        } else {
            f(it);
        }
    }
}
/*
 * iterate(j, [] (json::const_iterator it) {
 * DEBUG_VALUE_OF(it.key());
 * DEBUG_VALUE_OF(it.value());
 * });
 */

int main(int argc, char* argv[]) {
    DEBUG_METHOD();
    try {
        std::ifstream is("assets/mc.json");
        if(!is.is_open()) {
            throw std::runtime_error("file not found");
        }
        json j;
        is >> j;

        sprite_sheet s = j;

        DEBUG_VALUE_AND_TYPE_OF(s);



    } catch(...) {
        DEBUG_LOG("unhandled exception");
    }

    return 0;
}
