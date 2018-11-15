#pragma once

#include <classes/common.hpp>

namespace wee {
    class gamescreen;
}

class b2World;
class b2Shape;

typedef wee::factory<entity_type, std::string, b2World*, const tmx::Object&, entity_type&> object_factory;
typedef wee::factory<b2Shape, tmx::Object::Shape, const tmx::Object&> b2ShapeFactory;

typedef wee::factory<wee::gamescreen, std::string> gamescreen_factory;

static class register_factories final {
public:

    register_factories();
    virtual ~register_factories();
}_;
