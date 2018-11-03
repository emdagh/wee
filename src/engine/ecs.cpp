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
