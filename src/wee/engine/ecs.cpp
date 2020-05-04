#include <engine/ecs.hpp>

namespace wee {
    std::ostream& operator << (std::ostream& os, const physics_t&)     { return os; }
    std::ostream& operator << (std::ostream& os, const transform_t&)   { return os; }
    std::ostream& operator << (std::ostream& os, const visual_t&)      { return os; }
    std::ostream& operator << (std::ostream& os, const nested_t&)      { return os; }
    std::ostream& operator << (std::ostream& os, const tween_t&)       { return os; }
    std::ostream& operator << (std::ostream& os, const timeout_t&)     { return os; }
    std::ostream& operator << (std::ostream& os, const raycast_t&)     { return os; }
    std::ostream& operator << (std::ostream& os, const joint_t&)       { return os; }
}


//namespace std { //this is probably a Bad Idea, as std injection tends to lead to undefined behavior
//    size_t hash<wee::ecs::entity>::operator () (const wee::ecs::entity& e) const { 
//        return hash<decltype(e._id)>()(e._id); 
//    }
//    bool equal_to<wee::ecs::entity>::operator () (const wee::ecs::entity& a, const wee::ecs::entity& b) const {
//        return a._id == b._id;
//    }
//}