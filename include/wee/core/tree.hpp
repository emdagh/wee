#pragma once

namespace wee {
   
    template <typename T, template<typename... Args> typename Container>
    class basic_tree {
        T _value;
        Container<basic_tree> _children;
    public:
        
    };

    typedef basic_tree<SDL_Rect, std::vector > quadtree;
    typedef basic_tree<aabb, std::vector > octree;
}
