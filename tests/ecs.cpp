#include <core/tinytest.hpp>
#include <engine/ecs/ecs.hpp>
using namespace wee::ecs;
#if 0


enum {
    kName,
    kMana,
    kPosition,
    kFoo
};

#include <iostream>

struct vec2f {
    float x, y;
};

std::ostream& operator << (std::ostream& os, const vec2f& a) {
    os << "{ x: " << a.x << ", y:" << a.y << " }";
    return os;
}

using mana = component<kMana, float>;
using name = component<kName, std::string>;
using pos  = component<kPosition, vec2f>;
using foo = component<kFoo, int>;

int main(int, char**) {
    
    suite("testing id") {
        test(id() > zero<id_type>());
    }
    
    suite("testing add<component>(id) syntax") {
        entity player = 1, enemy = 2;
        test(!has<name>(player));
        test(!has<mana>(player));
        
        add<name>(player) = "Player";
        add<mana>(player) = 0.33f;
        add<pos>(player) = { 0.0f, 0.0f };
        
        add<name>(enemy) = "Orc";
        add<mana>(enemy) = 0.0f;
        add<pos>(enemy) = { 10.0f, 0.0f };
        
        test(has<name>(player));
        test(has<mana>(player));
        
        test(get<name>(player) == "Player");
        test(get<name>(player) != "Orc");
        
        test(join<name, mana, pos>().size() == 2);
        test(join<name, mana, foo>().size() == 0);
        
        for(auto& id : join<name, mana, pos>()) {
            std::cout << get<name>(id) << " has mana: " << get<mana>(id) << std::endl;
            std::cout << get<pos>(id) << std::endl;
        }
    }
    return 0;
        
}
#else


using name = component<10, std::string>;
using foo = component<1, int>;

int main(int, char**) {
    int player = 1;
    add<name>(player);
    test(has<name>(player));
    test(!has<foo>(player));

    add<foo>(player);

    get<name>(player) = "player one";
    get<foo>(player) = 10;

    test(get<name>(player) == "player one");

    for(auto& it : join<name, foo>()) {
        std::cout << "name=" << get<name>(it) << std::endl;
        std::cout << "foo=" << get<foo>(it) << std::endl;
    }
    
    return 0;
}

#endif
