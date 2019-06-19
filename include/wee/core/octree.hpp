#pragma once

namespace wee {
    template <typename T>
    class octree {
        std::array<octree*, 8> _children;
        octree* _parent;
        aabb _bounds;
        T _data;
        
    public:
        octree(const aabb& bounds);

    };
}
