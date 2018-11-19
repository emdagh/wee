#include <core/factory.hpp>

#include <Box2D/Box2D.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <engine/sprite_sheet.hpp>
#include <nlohmann/json.hpp>
#include <engine/ecs.hpp>
#include <engine/assets.hpp>
#include <core/logstream.hpp>
#include <core/lexical_cast.hpp>
#include <classes/common.hpp>
#include <classes/components.hpp>
#include <classes/factories.hpp>
#include <classes/common.hpp>
#include <classes/entities.hpp>
#include <classes/systems.hpp>


using nlohmann::json;
using namespace wee;
    

register_factories::register_factories() {
    b2ShapeFactory::instance().register_class(tmx::Object::Shape::Polygon, [] (const tmx::Object& obj) {
            std::vector<b2Vec2> vertices;
            for(const auto& point : obj.getPoints()) {
            b2Vec2 pos;
            pos.Set(SCREEN_TO_WORLD(point.x), SCREEN_TO_WORLD(point.y));
            vertices.push_back(pos);
            }
            b2Shape* shape = new b2PolygonShape();
            ((b2PolygonShape*)shape)->Set(&vertices[0], (int32_t)vertices.size());

            return shape;
            });

    b2ShapeFactory::instance().register_class(tmx::Object::Shape::Rectangle, [] (const tmx::Object& obj) {
            const auto& aabb = obj.getAABB();
            b2Vec2 halfWS = { aabb.width / 2, aabb.height / 2 };

            b2Shape* res = new b2PolygonShape;
            {
            ((b2PolygonShape*)res)->SetAsBox(SCREEN_TO_WORLD(halfWS.x), SCREEN_TO_WORLD(halfWS.y));
            }
            return res; 
            });

    b2ShapeFactory::instance().register_class(tmx::Object::Shape::Polyline, [] (const tmx::Object& obj) {
            std::vector<b2Vec2> vertices;
            for(const auto& point : obj.getPoints()) {
            b2Vec2 pos;
            pos.Set(SCREEN_TO_WORLD(point.x), SCREEN_TO_WORLD(point.y));
            vertices.push_back(pos);
            }
            b2Shape* shape = new b2ChainShape;
            ((b2ChainShape*)shape)->CreateChain(&vertices[0], vertices.size());
            return shape;
            });

    b2ShapeFactory::instance().register_class(tmx::Object::Shape::Ellipse, [] (const tmx::Object& obj) {
            const auto& aabb = obj.getAABB();
            assert(aabb.width == aabb.height);
            float radius = aabb.width / 2.0f;
            b2Shape* shape = new b2CircleShape;
            ((b2CircleShape*)shape)->m_radius = SCREEN_TO_WORLD(radius);//obj.m_width / 2);
            return shape;
    });
    /*TODO: Text*/

    object_factory::instance().register_class("norope", [&] (b2World*, const tmx::Object&, entity_type& self) {
            self = kult::entity();

            return &self;
            });

    /*object_factory::instance().register_class("spawn", [&] (b2World*, const tmx::Object&, entity_type& self) {
        self = kult::entity();
        kult::add<transform>(self);
        return &self;
    });*/

    object_factory::instance().register_class("environment", [&] (b2World* world, const tmx::Object& obj, entity_type& self) -> entity_type* {
            const auto& pos  = obj.getPosition();
            const auto& aabb = obj.getAABB();
            b2Vec2 halfWS = { aabb.width / 2, aabb.height / 2 };

            self = kult::entity();
            {
            kult::add<physics>(self);
            kult::add<raycast>(self);
            }
            {
            kult::get<raycast>(self).hit = false;
            }

            b2Body* body = nullptr;
            {
            b2BodyDef bd;
            bd.type = b2_staticBody;
            bd.position.Set(SCREEN_TO_WORLD(pos.x + halfWS.x), SCREEN_TO_WORLD(pos.y + halfWS.y));
            body = world->CreateBody(&bd);
            }
            kult::get<physics>(self).body = body;

            {
                b2FixtureDef fd;
                auto shape = std::unique_ptr<b2Shape>(b2ShapeFactory::instance().create(obj.getShape(), obj));
                fd.shape = shape.get();
                fd.isSensor            = false;
                fd.filter.categoryBits = E_CATEGORY_ENVIRONMENT;
                fd.filter.maskBits     = E_CATEGORY_PLAYER;
                fd.userData            = reinterpret_cast<void*>(self);
                body->CreateFixture(&fd);
            }


            return &self;
    });

    object_factory::instance().register_class("pickup", [&] (b2World* world, const tmx::Object& obj, entity_type& self) {
            const auto& pos  = obj.getPosition();
            const auto& aabb = obj.getAABB();
            b2Vec2 halfWS = { aabb.width / 2, aabb.height / 2 };

            self = kult::entity();
            DEBUG_VALUE_OF(self);
            {
            kult::add<physics>(self);
            kult::add<pickup>(self);
            kult::add<visual>(self);
            kult::add<nested>(self);
            kult::add<transform>(self);
            }
            /** 
             * determing visuals based on
             * pickup value.
             */
            static sprite_sheet* s = nullptr;
            if(!s) {
                s = new sprite_sheet;
                json j;
                std::ifstream is = open_ifstream("assets/pickups.json", std::ios::binary);
                if(is.is_open()) {
                    is >> j; 
                    from_json(j, *s);
                    is.close();
                }
            }

            visual_t& v = kult::get<visual>(self);
            v.texture = s->_texture;
            v.visible = true;


            std::map<std::string, std::string> props;
            for(const auto& p : obj.getProperties()) {
                props.emplace(p.getName(), p.getStringValue());
            }
            if(props.count("value")) {
                pickup_t& p = kult::get<pickup>(self);
                p.value = lexical_cast<int>(props.at("value"));
                if(p.value == 10) {
                    v.src = s->get("blueGem.png");
                }
                if(p.value == 100) {
                    v.src = s->get("redGem.png");
                }

                /*auto& n = kult::get<nested>(self);
                n.offset.x = -.5f * v.src.w;
                n.offset.y = -.5f * v.src.h;*/
            }
            float px = pos.x + halfWS.x;
            float py = pos.y + halfWS.y;

            {
                kult::get<nested>(self).offset= vec2f{px, py};
            }
            /**
             * physics stuff
             */
            b2Body* body = nullptr;
            {
                b2BodyDef bd;
                bd.type = b2_staticBody;
                bd.position.Set(SCREEN_TO_WORLD(px), SCREEN_TO_WORLD(py));
                body = world->CreateBody(&bd);
            }
            kult::get<physics>(self).body = body;

            {
                b2FixtureDef fd;
                auto shape = std::unique_ptr<b2Shape>(b2ShapeFactory::instance().create(obj.getShape(), obj));
                fd.shape = shape.get();
                fd.isSensor            = true;
                fd.filter.categoryBits = E_CATEGORY_PICKUP;
                fd.filter.maskBits     = E_CATEGORY_PLAYER;
                fd.userData            = reinterpret_cast<void*>(self);
                body->CreateFixture(&fd);
            }

            kult::add<synch>(self).cleanup = false;

            kult::get<physics>(self).on_trigger_enter = [&] (const collision& col) {
                DEBUG_METHOD();

                kult::get<synch>(col.self).cleanup = true;
                
                if(kult::has<player>(col.other)) {
                    kult::get<player>(col.other).score += kult::get<pickup>(col.self).value;
                    DEBUG_VALUE_OF(kult::get<player>(col.other).score);
                }
            };

            return &self;
    });
}

register_factories::~register_factories() {
}
