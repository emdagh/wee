#pragma once

namespace wee {
    template <typename T>
    class octree {
        std::array<octree*, 8> _children;
        octree* _parent;
        aabb _bounds;
        
    public:
        octree(const aabb& bounds);

    };
}
