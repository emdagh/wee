#include "common/common.hpp"
#include "common/components.hpp"
#include "common/Box2DEXT.hpp"
#include <engine/assets.hpp>
#include <core/logstream.hpp>
#include <core/factory.hpp>
#include <core/zip.hpp>
#include <core/map.hpp>
#include <prettyprint.hpp>
#include <kult.hpp>
#include <Box2D/Box2D.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <string>
#include <list>

using namespace wee;
typedef int gid;

typedef kult::type entity_type;


static class register_factories final {
public:

    typedef factory<kult::type, std::string,        b2World*, const tmx::Object&> object_factory;
    typedef factory<b2Shape*,   tmx::Object::Shape, const tmx::Object&> b2ShapeFactory;

    register_factories() {



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

        b2ShapeFactory::instance().register_class(tmx::Object::Shape::Rectangle, [] (const tmx::Object& obj) {
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
        
        
        object_factory::instance().register_class("spawnPoint", [&] (b2World*, const tmx::Object&) {
            kult::type self = kult::entity();

            return self;
        });
        
        object_factory::instance().register_class("environment", [&] (b2World* world, const tmx::Object& obj) {
            const auto& pos  = obj.getPosition();
            const auto& aabb = obj.getAABB();
            b2Vec2 halfWS = { aabb.width / 2, aabb.height / 2 };

            kult::type self = kult::entity();
            {
                kult::add<rigidbody>(self);
                kult::add<collider>(self);
                kult::add<raycast>(self);
            }
            b2Body* body = nullptr;
            {
                b2BodyDef bd;
                bd.type = b2_staticBody;
                bd.position.Set(SCREEN_TO_WORLD(pos.x + halfWS.x), SCREEN_TO_WORLD(pos.y + halfWS.y));
                body = world->CreateBody(&bd);
            }
            kult::get<rigidbody>(self).body = body;
            
            b2Fixture* fixture = nullptr;
            {
                b2FixtureDef fd;
                auto shape = std::unique_ptr<b2Shape>(b2ShapeFactory::instance().create(obj.getShape(), obj));
                fd.shape = shape.get();
                fd.isSensor            = false;
                fd.filter.categoryBits = 0xffff;
                fd.filter.maskBits     = 0xffff;
                fd.userData            = reinterpret_cast<void*>(self);
                fixture                 = body->CreateFixture(&fd);
            }
            kult::get<collider>(self).fixture = fixture;


            return self;
        });
    }

    virtual ~register_factories() {
    }
}_;


/**
 * every level chunk has:
 * a spawn point
 * a first-connected collider
 * a pointer to the next chunk
 * a pointer to the previous chunk 
 *
 * The last two (prev, next ptr) can be over-arched by an 
 * std::list 
 */

struct level {
    struct chunk {
        vec2f _spawn;
        kult::type _firstContact;
    };
    std::list<chunk*> _chunks;
};


void parse_tmx(b2World* world, const std::string& pt, SDL_Point* spawnPoint) {


    tmx::Map map;

    if(map.load(pt)) {
        DEBUG_LOG("loaded map version: {}.{}", 
                map.getVersion().upper,
                map.getVersion().lower);
        const auto& properties = map.getProperties();
        DEBUG_LOG("map has {} properties", properties.size());
        for(const auto& p : properties) {
            DEBUG_VALUE_OF(p.getName());
            DEBUG_VALUE_OF(p.getStringValue());
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
                    //const auto& type = object.getType();
                    //const auto& shape = object.getShape();

                    //b2Body* body = b2BodyBuilder::instance().build(world, object);
                    //
                    //
                    //
                    auto in = object.getProperties();
                    
                    /*auto props = wee::zip(
                        wee::map(in, [] (const tmx::Property& p) { return p.getName(); }), 
                        wee::map(in, [] (const tmx::Property& p) { return p.getStringValue(); })
                    );*/

                    //std::vector<std::tuple<std::string, std::string> > props;

                    std::map<std::string, std::string> props;
                    for(const auto& p : object.getProperties()) {
                        props.emplace(p.getName(), p.getStringValue());
                    }
                    DEBUG_VALUE_AND_TYPE_OF(props);

                    if(props.count("class")) {
                        register_factories::object_factory::instance().create(props["class"], world, object);
                    }
                    



                    const auto& object_properties = object.getProperties();
                    DEBUG_LOG("object has {} properties", 
                        object_properties.size());
                    for(const auto& object_property : object_properties) {
                        if(object_property.getName() == "type") {
                            if(object_property.getStringValue() == "spawnPoint") {
                                spawnPoint->x = (int)position.x;
                                spawnPoint->y = (int)position.y;
                            }
                        }
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
        //auto self = reinterpret_cast<kult::type>(fixture->GetUserData());
        //raycast_t& r = kult::get<raycast>(self);
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
    kult::add<visual>(self) = { texture, src, SDL_ColorPresetEXT::White , 0};
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
    b2DebugDrawImpl _debugdraw;
public:
    game() {
        _debugdraw.SetFlags(
            b2Draw::e_shapeBit | //= 0x0001, 
            b2Draw::e_jointBit  //= 0x0002, 
            //b2Draw::e_aabbBit   //= 0x0004, 
            //e_pairBit  | //= 0x0008, 
            //e_centerOfMassBit | //= 0x0010, 
            //e_particleBit// = 0x0020 
        ) ;
        _world = new b2World({0.0f, 9.8f});
        _world->SetDebugDraw(&_debugdraw);
        _camera = { 0, 0, 0, 0 };
    }
    int load_content() {

        assets<SDL_Texture>::instance().load("@BG_mountains", ::as_lvalue(
            std::ifstream(get_resource_path("assets/img") + "mountains.png", std::ios::binary)
        ));
        assets<SDL_Texture>::instance().load("@BG_clouds", ::as_lvalue(
            std::ifstream(get_resource_path("assets/img") + "mountains.png", std::ios::binary)
        ));
        assets<SDL_Texture>::instance().load("@MG_clouds_1", ::as_lvalue(
            std::ifstream(get_resource_path("assets/img") + "mountains.png", std::ios::binary)
        ));
        assets<SDL_Texture>::instance().load("@MG_clouds_2", ::as_lvalue(
            std::ifstream(get_resource_path("assets/img") + "mountains.png", std::ios::binary)
        ));
        assets<SDL_Texture>::instance().load("@MG_clouds_3", ::as_lvalue(
            std::ifstream(get_resource_path("assets/img") + "mountains.png", std::ios::binary)
        ));

        std::string pt = wee::get_resource_path("assets/levels") + "level.tmx";

        SDL_Point spawnPoint;
        parse_tmx(_world, pt, &spawnPoint);
        
        p  = create_player(_world, spawnPoint);
        b1 = create_block(_world, { -16, -300, 32, 32 });
        _rope = create_rope_between(_world, p, b1, kult::get<rigidbody>(b1).body->GetWorldCenter());

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
                    std::ifstream(tset.getImagePath(), std::ios::binary)
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
		return 0;
    }

    int update(int ) {
        _world->Step(1.0f / (float)60, 4, 3);
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
        _debugdraw.SetCameraPosition(_camera.x, _camera.y);
		return 0;

    }

    

    int draw(SDL_Renderer* renderer) {
        _debugdraw.SetRenderer(renderer);
        SDL_RenderGetLogicalSize(renderer, &_camera.w, &_camera.h);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);
        int cx = -_camera.x + (_camera.w >> 1);
        int cy = -_camera.y + (_camera.h >> 1);
        {
            /*std::vector<kult::type> entities;*/

            for(const auto& e : kult::join<transform, visual>()) {
                const visual_t& v = kult::get<visual>(e);
                const transform_t& t = kult::get<transform>(e);
                SDL_Rect dst = {
                    cx + (int)t.p.x, 
                    cy + (int)t.p.y,
                    v.src.w, 
                    v.src.h
                };
                SDL_RenderCopy(renderer, 
                    v.texture,
                    &v.src,
                    &dst
                );
            }

            /*std::sort(entities.begin(), entities.end(), [&] (const kult::type& a, const kult::type& b) {
                int la = kult::get<visual>(a).layer;
                int lb = kult::get<visual>(b).layer;
                return la < lb;
            });*/
        }
        //b2DebugDrawEXT(_world, renderer, _camera);
        _world->DrawDebugData();
        
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);

        b2Vec2 pa = kult::get<rigidbody>(p).body->GetPosition();
        b2Vec2 temp = { _mouse_pos.x, _mouse_pos.y };
        b2Vec2 pb = SCREEN_TO_WORLD(temp);

        pa = WORLD_TO_SCREEN(pa);
        pb = WORLD_TO_SCREEN(pb);



        SDL_RenderDrawLine(renderer, cx + (int)pa.x, cy + (int)pa.y, (int)pb.x, (int)pb.y);
        
        SDL_RenderPresent(renderer);
		return 0;
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

#include <string>


#undef main //SDL idiocy
int main(int, char*[]) {
    applet* let = new game();
    application app(let);
    static_cast<game*>(let)->callbacks(&app);
    return app.start();
}
