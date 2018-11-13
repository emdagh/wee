#pragma once

/**
 * I don't like this here... No place else atm though...
 */
#include <core/factory.hpp>
#include <engine/ecs.hpp>
#include <string>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>

class b2Shape;
class b2World;


typedef kult::type entity_type;

typedef wee::factory<entity_type, std::string,        b2World*, const tmx::Object&> object_factory;
typedef wee::factory<b2Shape*,   tmx::Object::Shape, const tmx::Object&> b2ShapeFactory;

#define E_CATEGORY_ENVIRONMENT  (1 << 1)
#define E_CATEGORY_PICKUP       (1 << 2)
#define E_CATEGORY_PLAYER       (1 << 3)
