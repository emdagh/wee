#include <classes/components.hpp>
#include <nlohmann/json.hpp>

using nlohmann::json;

std::ostream& operator << (std::ostream& os, const player_t&) {
    return os;
}
std::ostream& operator << (std::ostream& os, const pickup_t& p) {
    json j = {"pickup", 
        { "value", p.value },
        { "type", p.type }
    };
    os << j;
    return os;
}
std::ostream& operator << (std::ostream& os, const beat_t&) {
    return os;
}
std::ostream& operator << (std::ostream& os, const synch_t&) {
    return os;
}
