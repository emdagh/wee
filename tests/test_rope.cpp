#include "common/common.hpp"
#include "common/components.hpp"
#include "common/Box2DEXT.hpp"
#include <engine/assets.hpp>
#include <util/logstream.hpp>
#include <kult.hpp>
#include <Box2D/Box2D.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <string>

using namespace wee;
typedef int gid;

typedef kult::type entity_type;

struct b2BodyBuilder : public singleton<b2BodyBuilder> {
    typedef std::function<b2Body*(b2World*, const tmx::Object&)> create_function;

    b2Body* build(b2World* world, const tmx::Object& obj) {
        const auto& ix= obj.getShape();
        _mp[ix](world, obj);
    }

    void push(const tmx::Object::Shape& ix, const create_function& fun) {
        _mp[ix] = fun;
    }

    std::map<tmx::Object::Shape, create_function> _mp;
};

static class register_factories {
public:

    static b2Body* create_polygon(b2World* world, const tmx::Object& obj) {
        const auto& pos  = obj.getPosition();
        const auto& aabb = obj.getAABB();

        kult::type self = kult::entity();
        b2Vec2 halfWS;
        halfWS.Set(aabb.width / 2, aabb.height / 2);

        std::vector<b2Vec2> vertices;

        for(const auto& point : obj.getPoints()) {
            b2Vec2 pos;
            pos.Set(SCREEN_TO_WORLD(point.x), SCREEN_TO_WORLD(point.y));
            vertices.push_back(pos);
        }
        b2Body* body = nullptr;
        b2Fixture* fixture = nullptr;
        {
            b2BodyDef bd;
            bd.type = b2_staticBody;
            bd.position.Set(SCREEN_TO_WORLD(pos.x + halfWS.x), SCREEN_TO_WORLD(pos.y + halfWS.y));
            body = world->CreateBody(&bd);
        }
        {
            b2PolygonShape shape;
            shape.Set(&vertices[0], vertices.size());
            b2FixtureDef def;
            def.shape               = &shape;
            def.isSensor            = false;
            def.filter.categoryBits = 0xffff;
            def.filter.maskBits     = 0xffff;
            def.userData            = reinterpret_cast<void*>(self);
            fixture                 = body->CreateFixture(&def);
        }
        kult::add<rigidbody>(self).body = body;
        kult::add<collider>(self).fixture = fixture;
        kult::add<raycast>(self).is_hit = false;

        return body;
    }

    static b2Body* create_polyline(b2World* world, const tmx::Object& obj) {

        DEBUG_METHOD();
        const auto& pos  = obj.getPosition();
        const auto& aabb = obj.getAABB();

        kult::type self = kult::entity();
        b2Vec2 halfWS;
        halfWS.Set(aabb.width / 2, aabb.height / 2);

        std::vector<b2Vec2> vertices;

        for(const auto& point : obj.getPoints()) {
            b2Vec2 pos;
            pos.Set(SCREEN_TO_WORLD(point.x), SCREEN_TO_WORLD(point.y));
            vertices.push_back(pos);
        }
        b2Body* body = nullptr;
        b2Fixture* fixture = nullptr;
        {
            b2BodyDef bd;
            bd.type = b2_staticBody;
            bd.position.Set(SCREEN_TO_WORLD(pos.x + halfWS.x), SCREEN_TO_WORLD(pos.y + halfWS.y));
            body = world->CreateBody(&bd);
        }
        {
            //b2PolygonShape shape;
            b2ChainShape shape;
            shape.CreateChain(&vertices[0], vertices.size());
            b2FixtureDef def;
            def.shape               = &shape;
            def.isSensor            = false;
            def.filter.categoryBits = 0xffff;
            def.filter.maskBits     = 0xffff;
            def.userData            = reinterpret_cast<void*>(self);
            fixture                 = body->CreateFixture(&def);
        }
        kult::add<rigidbody>(self).body = body;
        kult::add<collider>(self).fixture = fixture;
        kult::add<raycast>(self).is_hit = false;

        return body;



    };
    static b2Body* create_rectangle(b2World* world, const tmx::Object& obj) {

        kult::type self = kult::entity();

        const auto& aabb = obj.getAABB();
        const auto& pos  = obj.getPosition();
        b2Vec2 halfWS;
        halfWS.Set(aabb.width / 2, aabb.height / 2);

        b2Body* body = nullptr;
        b2Fixture* fixture = nullptr;
        {
            b2BodyDef bd;
            bd.type = b2_staticBody;
            bd.position.Set(SCREEN_TO_WORLD(pos.x + halfWS.x), SCREEN_TO_WORLD(pos.y + halfWS.y));
            body = world->CreateBody(&bd);
        }
        {
            b2PolygonShape shape;
            shape.SetAsBox(SCREEN_TO_WORLD(halfWS.x), SCREEN_TO_WORLD(halfWS.y));
            b2FixtureDef def;
            def.shape               = &shape;
            def.isSensor            = false;
            def.filter.categoryBits = 0xffff;
            def.filter.maskBits     = 0xffff;
            def.userData            = reinterpret_cast<void*>(self);
            fixture                 = body->CreateFixture(&def);
        }
        
        kult::add<rigidbody>(self).body = body;
        kult::add<collider>(self).fixture = fixture;
        kult::add<raycast>(self).is_hit = false;

        return body;
    };

    register_factories() {
        b2BodyBuilder::instance().push(tmx::Object::Shape::Polygon, create_polygon);
        b2BodyBuilder::instance().push(tmx::Object::Shape::Rectangle, create_rectangle);
        b2BodyBuilder::instance().push(tmx::Object::Shape::Polyline, create_polyline);

        b2BodyBuilder::instance().push(tmx::Object::Shape::Ellipse,
            [&] (b2World* world, const tmx::Object& obj) -> b2Body* {
                const auto& aabb = obj.getAABB();
                const auto& pos  = obj.getPosition();
                
                assert(aabb.width == aabb.height);
                
                float radius = aabb.width / 2.0f;
                kult::type self = kult::entity();
                b2Body* body = nullptr;
                b2Fixture* fixture = nullptr;
                
                {
                    b2BodyDef bd;
                    bd.type = b2_staticBody;
                    bd.position.Set(SCREEN_TO_WORLD(pos.x + radius), SCREEN_TO_WORLD(pos.y + radius));
                    body = world->CreateBody(&bd);
                }
                
                {

                    b2CircleShape shape;
                    shape.m_radius = SCREEN_TO_WORLD(radius);//obj.m_width / 2);
                    b2FixtureDef def;
                    def.shape = &shape;
                    def.isSensor = false;
                    def.filter.categoryBits = 0xffff;
                    def.filter.maskBits = 0xffff;
                    def.userData = reinterpret_cast<void*>(self);
                    fixture = body->CreateFixture(&def);
                }

                kult::add<rigidbody>(self).body = body;
                kult::add<collider>(self).fixture = fixture;
                kult::add<raycast>(self).is_hit = false;

                return body;
            }
        );
    }
}_;

//template <typename T>
//T* build(


void parse_tmx(b2World* world, const std::string& pt) {
    tmx::Map map;

    if(map.load(pt)) {
        DEBUG_LOG("loaded map version: {}.{}", 
                map.getVersion().upper,
                map.getVersion().lower);
        const auto& properties = map.getProperties();
        DEBUG_LOG("map has {} properties", properties.size());
        for(const auto& p : properties) {
            DEBUG_VALUE_OF(p.getName());
            DEBUG_VALUE_OF((int)p.getType());

        }

        const auto& layers = map.getLayers();
        DEBUG_LOG("map has {} layers", layers.size());
        for(const auto& layer : layers) {

            if(layer->getType() == tmx::Layer::Type::Object) {
                DEBUG_LOG("found object layer {}",
                    layer->getName());

                const auto& objects = dynamic_cast<tmx::ObjectGroup*>(layer.get())->getObjects();

                for(const auto& object : objects) {
                    DEBUG_LOG("object {}", object.getName());
                    
                    const auto& position = object.getPosition();
                    const auto& type = object.getType();
                    const auto& shape = object.getShape();

                    b2Body* body = b2BodyBuilder::instance().build(world, object);
                    DEBUG_VALUE_OF(body);


                    DEBUG_LOG("position: {},{}", position.x, position.y);

                    const auto& object_properties = object.getProperties();
                    DEBUG_LOG("object has {} properties", 
                        object_properties.size());
                    for(const auto& object_property : object_properties) {
                        DEBUG_LOG("property: {} ({})", 
                            object_property.getName(),
                            (int)object_property.getType()
                        );
                    }
                }
            }
            const auto& layer_properties = layer->getProperties();
            DEBUG_LOG("layer has {} properties", layer_properties.size());
            for(const auto& p : layer_properties) {
                DEBUG_LOG("property: {} ({})", 
                    p.getName(),
                    (int)p.getType()
                );

            }

        }

    }

}

class b2RayCastClosest : public b2RayCastCallback {
    b2Fixture* _fixture;
    b2Vec2 _point, _normal;
    float _fraction= 1.f;
public:
    void RayCast(b2World* world, const b2Vec2& p1, const b2Vec2& p2) {
        _fixture = NULL;
        _fraction = 1.0f;
        world->RayCast(this, p1, p2);
        if(_fixture) {
            kult::type self = reinterpret_cast<kult::type>(_fixture->GetUserData());
            raycast_t& r = kult::get<raycast>(self);
            r.is_hit     = true;
            r.point      = {_point.x, _point.y};
            r.normal     = {_normal.x, _normal.y};
        }

    }
    float32 ReportFixture(b2Fixture* fixture, 
            const b2Vec2& point, 
            const b2Vec2& normal, 
            float32 fraction) {
        auto self = reinterpret_cast<kult::type>(fixture->GetUserData());
        raycast_t& r = kult::get<raycast>(self);
        _fixture    = fixture;
        _fraction   = fraction;
        _point      = point;
        _normal     = normal;
        
        return fraction;
    }
};


struct camera {
    SDL_Rect rect;
};

kult::type create_sensor(b2World* world, kult::type parent, const vec2f& offset, float radius, const collision_callback&) {
    kult::type self = kult::entity();
    b2Body* body = nullptr;
    b2Fixture* fixture = nullptr;
    {

        b2BodyDef def;
        def.type = b2_staticBody;
        body = world->CreateBody(&def);
    }
    {
        b2CircleShape shape;
        shape.m_radius = SCREEN_TO_WORLD(radius);
        b2FixtureDef def;
        def.shape = &shape;
        def.isSensor = true;
        fixture = body->CreateFixture(&def);

    }
    kult::add<collider>(self).fixture = fixture;
    kult::add<rigidbody>(self).body = body;
    kult::add<nested>(parent);
    {
        nested_t& n = kult::get<nested>(parent);
        n.parent = parent;
        n.offset = offset;
        
    }
    return self;
}

entity_type create_rope_between(b2World* world, entity_type a, entity_type b, const b2Vec2& bPosWS) {


    b2Body* body = kult::get<rigidbody>(b).body;
    if(!body) {
        throw std::logic_error("entity must have a rigidbody attached");
    }
    kult::type e = kult::entity();

    b2RopeJointDef def;
    def.userData = reinterpret_cast<void*>(e);
    def.bodyA = kult::get<rigidbody>(a).body; 
    def.bodyB = kult::get<rigidbody>(b).body; 
    def.collideConnected = true;
    // center of player
    def.localAnchorA = b2Vec2 {0.0f, 0.0f};
    // raycast the world here
    def.localAnchorB = body->GetLocalPoint(bPosWS);
    def.maxLength = (def.bodyA->GetPosition() - bPosWS).Length();//SCREEN_TO_WORLD(0.0f);

    auto* joint = world->CreateJoint(&def);
    kult::add<articulation>(e).joint = joint;//world->CreateJoint(&def);
    return e;
}


entity_type create_player(b2World* world, const SDL_Point& at) {
    entity_type self = kult::entity();
    b2Body* body = nullptr;
    b2Fixture* fixture = nullptr;
    {
        b2BodyDef bd;
        bd.type = b2_dynamicBody; 
        bd.position.Set(SCREEN_TO_WORLD(at.x), SCREEN_TO_WORLD(at.y));
        body = world->CreateBody(&bd);
    }

    {
        b2CircleShape shape;
        shape.m_p.Set(0.0f, 0.0f);//
        shape.m_radius = SCREEN_TO_WORLD(10.0f);
        b2FixtureDef fd;
        fd.filter.categoryBits = 0xffff;//!0;//(uint16_t)collision_filter::player;
        fd.filter.maskBits = 0xffff;//!0;//(uint16_t)collision_filter::any;
        fd.density = 1.0f;
        fd.restitution = 0.0f;
        fd.shape = &shape;
        fd.isSensor = false;
        fd.userData = reinterpret_cast<void*>(self);

        fixture = body->CreateFixture(&fd);
    }
    kult::add<rigidbody>(self).body = body;
    kult::add<collider>(self).fixture = fixture;
    return self;
}

entity_type create_block(b2World* world, const SDL_Rect& r) {
    entity_type self = kult::entity();
    b2Body* body = nullptr;
    b2Fixture* fixture=  nullptr;

    float half_w = (float)(r.w >> 1);
    float half_h = (float)(r.h >> 1);

    {
        b2BodyDef def;
        def.type = b2_staticBody;
        def.position.Set(
            SCREEN_TO_WORLD(r.x + half_w), 
            SCREEN_TO_WORLD(r.y + half_h)
        );
        body = world->CreateBody(&def);
    }

    {
        b2PolygonShape shape;
        shape.SetAsBox(
            SCREEN_TO_WORLD(half_w), 
            SCREEN_TO_WORLD(half_h)
        );
        b2FixtureDef def;
        def.shape = &shape;
        def.isSensor = false;
        def.filter.categoryBits = 0xffff;
        def.filter.maskBits = 0xffff;
        def.userData = reinterpret_cast<void*>(self);
        
        fixture = body->CreateFixture(&def);
    }

    kult::add<rigidbody>(self).body = body;
    kult::add<collider>(self).fixture = fixture;
    kult::add<raycast>(self).is_hit = false;

    return self;
}

kult::type tile(SDL_Texture* texture, const SDL_Point& dst, const SDL_Rect& src) {
    kult::type self = kult::entity();
    kult::add<visual>(self) = { texture, src, SDL_ColorPresetEXT::White };
    kult::add<transform>(self) = { {(float)dst.x, (float)dst.y}, 0.0f};
    return self;
}


class game : public applet {
    b2World* _world;
    bool _mousedown = false;
    int _time_down = 0;
    SDL_Rect _camera;
    entity_type b0, p, b1;
    vec2f _mouse_pos;
    kult::type _rope;
public:
    game() {
        _world = new b2World({0.0f, 9.8f});
        _camera = { 0, 0, 0, 0 };
    }
    int load_content() {
        b0 = create_block(_world, {-100, 0, 200, 8 });
        p  = create_player(_world, {0, -150});
        b1 = create_block(_world, { -16, -300, 32, 32 });
        create_block(_world, { -16, -200, 32, 32 });
        create_block(_world, { 200, -100, 32, 32 });
        create_block(_world, { -16, 100, 32, 32 });
        create_block(_world, { -16, 150, 32, 32 });
        _rope = create_rope_between(_world, p, b1, kult::get<rigidbody>(b1).body->GetWorldCenter());

        std::string pt = wee::get_resource_path("") + "assets/levels/level.tmx";
        parse_tmx(_world, pt);

        tmx::Map tiled_map;
        tiled_map.load(pt);
        auto map_dimensions = tiled_map.getTileCount();
        int rows = map_dimensions.y;
        int cols = map_dimensions.x;

        auto tile_size = tiled_map.getTileSize();
        int tile_w = tile_size.x;
        int tile_h = tile_size.y;

        std::map<gid, SDL_Texture*> tilesets;

        auto& map_tilesets = tiled_map.getTilesets();
        for(auto& tset : map_tilesets) {
            auto* tex = wee::assets<SDL_Texture>::instance().load(
                tset.getImagePath(),
                ::as_lvalue(
                    std::ifstream(tset.getImagePath())
                )
            );
            tilesets.insert(std::pair<gid, SDL_Texture*>(tset.getFirstGID(), tex));
        }

        const auto& map_layers = tiled_map.getLayers();
        for(const auto& layer : map_layers) {
            if(layer->getType() != tmx::Layer::Type::Tile) {
                continue;
            }

            auto* tile_layer = dynamic_cast<const tmx::TileLayer*>(layer.get());
            auto& layer_tiles = tile_layer->getTiles();

            for(int y=0; y < rows; y++) {
                for(int x=0; x < cols; x++) {
                    int ix = x + y * cols;
                    gid cur_gid = layer_tiles[ix].ID;
                    if(0 == cur_gid) {
                        continue;
                    }

                    auto tset_gid = -1;
                    for(auto& ts : tilesets) {
                        if(ts.first <= cur_gid) {
                            tset_gid = ts.first;
                            break;
                        }
                    }

                    if(tset_gid == -1) 
                        continue;

                    cur_gid -= tset_gid;

                    int ts_w = 0;
                    int ts_h = 0;
                    SDL_QueryTexture(tilesets[tset_gid],
                            NULL, NULL, &ts_w, &ts_h);

                    int region_x = (cur_gid % (ts_w / tile_w)) * tile_w;
                    int region_y = (cur_gid / (ts_w / tile_h)) * tile_h;

                    int x_pos = x * tile_w;
                    int y_pos = y * tile_h;

                    /*DEBUG_VALUE_OF(region_x);
                    DEBUG_VALUE_OF(region_y);
                    DEBUG_VALUE_OF(x_pos);
                    DEBUG_VALUE_OF(y_pos);*/
                    tile(tilesets[tset_gid], {x_pos, y_pos}, {region_x, region_y, tile_w, tile_h});

                }
            }
        }
    }

    int update(int dt) {
        _world->Step(1.0f / 60.0f, 4, 3);
        articulation_t& a = kult::get<articulation>(_rope);
        for(auto& e : kult::join<raycast>()) {
            raycast_t& r = kult::get<raycast>(e);
            if(r.is_hit) {
                r.is_hit = false;
                if(a.joint) {
                    _world->DestroyJoint(a.joint);
                }
                _rope = create_rope_between(_world, p, e, b2Vec2{r.point.x, r.point.y});
            }
        }

        b2Vec2 pos = WORLD_TO_SCREEN(kult::get<rigidbody>(p).body->GetPosition());
        _camera.x = pos.x;
        _camera.y = pos.y;
    }

    

    int draw(SDL_Renderer* renderer) {
        SDL_RenderGetLogicalSize(renderer, &_camera.w, &_camera.h);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);
        int cx = -_camera.x + (_camera.w >> 1);
        int cy = -_camera.y + (_camera.h >> 1);
        {
            for(const auto& e : kult::join<transform, visual>()) {
                const visual_t& v = kult::get<visual>(e);
                const transform_t& t = kult::get<transform>(e);

                SDL_Rect dst = {
                    cx + t.p.x, 
                    cy + t.p.y,
                    v.src.w, 
                    v.src.h
                };
                /*SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::White);
                SDL_RenderDrawRect(renderer, &dst);*/
                SDL_RenderCopy(renderer, 
                    v.texture,
                    &v.src,
                    &dst
                );


            }
        }
        b2DebugDrawEXT(_world, renderer, _camera);
        
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);

        

        b2Vec2 pa = kult::get<rigidbody>(p).body->GetPosition();
        b2Vec2 temp = { _mouse_pos.x, _mouse_pos.y };
        b2Vec2 pb = SCREEN_TO_WORLD(temp);


        pa = WORLD_TO_SCREEN(pa);
        pb = WORLD_TO_SCREEN(pb);



        SDL_RenderDrawLine(renderer, cx + pa.x, cy + pa.y, pb.x, pb.y);
        
        SDL_RenderPresent(renderer);
    }

    int on_click() {
        for(auto& e : kult::join<raycast>()) {
            kult::get<raycast>(e).is_hit = false;
        }
        int cx = -_camera.x + (_camera.w >> 1);
        int cy = -_camera.y + (_camera.h >> 1);

        b2Vec2 pa = kult::get<rigidbody>(p).body->GetPosition();
        b2Vec2 temp = { _mouse_pos.x - cx, _mouse_pos.y - cy };
        b2Vec2 pb = SCREEN_TO_WORLD(temp);

        //_world->RayCast(&_raycast, pa, pb);
        b2RayCastClosest rc;
        rc.RayCast(_world, pa, pb);
        return 0;
    }

    void callbacks(application* app) {
        app->on_mousedown += [&] (char) {
            this->_mousedown = true;
            this->_time_down = SDL_GetTicks();
            return 0;
        };

        app->on_mouseup += [&] (char) {
            this->_mousedown = false;
            int dt = (SDL_GetTicks() - this->_time_down);
            static const int TIMEOUT = 500;
            DEBUG_VALUE_OF(dt);
            if(dt < TIMEOUT) {
                return this->on_click();
            }
            return 0;
        };

        app->on_mousemove += [&] (int x, int y) {
            this->_mouse_pos.x = (float)x;
            this->_mouse_pos.y = (float)y;
            return 0;
        };
    }

};

int main(int, char*[]) {
    applet* let = new game();
    application app(let);
    static_cast<game*>(let)->callbacks(&app);
    return app.start();
}
