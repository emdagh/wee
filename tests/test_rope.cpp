#include "common/common.hpp"
#include "common/components.hpp"
#include "common/Box2DEXT.hpp"
#include <engine/b2Adapters.hpp>
#include <engine/ecs.hpp>
#include <engine/camera.hpp>
#include <engine/assets.hpp>
#include <engine/sprite_sheet.hpp>
#include <core/logstream.hpp>
#include <core/factory.hpp>
#include <core/zip.hpp>
#include <core/map.hpp>
#include <core/lexical_cast.hpp>
#include <core/mat4.hpp>
#include <core/vec3.hpp>
#include <prettyprint.hpp>
#include <kult.hpp>
#include <Box2D/Box2D.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <string>
#include <list>

using namespace wee;
using nlohmann::json;
typedef int gid;

typedef kult::type entity_type;

#define E_CATEGORY_ENVIRONMENT  (1 << 1)
#define E_CATEGORY_PICKUP       (1 << 2)
#define E_CATEGORY_PLAYER       (1 << 3)


using wee::mat4;

auto clean_physics = [] (b2World* world) {
    for(auto& self : kult::join<physics>()) {
        if(!kult::get<physics>(self).do_cleanup) 
            continue;
        b2Body* rb = kult::get<physics>(self).body;

        for(auto* ptr = rb->GetFixtureList(); ptr; ptr = ptr->GetNext()) {
            rb->DestroyFixture(ptr);
        }
        world->DestroyBody(rb);
        kult::purge(self);
    }
};


typedef struct {
    int value;
    int type;
} pickup_t;

std::ostream& operator << (std::ostream& os, const pickup_t& p) {
    json j = {"pickup", 
        { "value", p.value },
        { "type", p.type }
    };
    os << j;
    return os;
}

using pickup = kult::component<1 << 10, pickup_t>;


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
            DEBUG_VALUE_OF(aabb.width);
            DEBUG_VALUE_OF(aabb.height);
            float radius = aabb.width / 2.0f;
            DEBUG_VALUE_OF(radius);
            b2Shape* shape = new b2CircleShape;
            ((b2CircleShape*)shape)->m_radius = SCREEN_TO_WORLD(radius);//obj.m_width / 2);
            return shape;
        });
        /*TODO: Text*/
        
        object_factory::instance().register_class("norope", [&] (b2World*, const tmx::Object&) {
            kult::type self = kult::entity();

            return self;
        });
        
        object_factory::instance().register_class("spawn", [&] (b2World*, const tmx::Object&) {
            kult::type self = kult::entity();

            return self;
        });
        
        object_factory::instance().register_class("environment", [&] (b2World* world, const tmx::Object& obj) {
            const auto& pos  = obj.getPosition();
            const auto& aabb = obj.getAABB();
            b2Vec2 halfWS = { aabb.width / 2, aabb.height / 2 };

            kult::type self = kult::entity();
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


            return self;
        });

        object_factory::instance().register_class("pickup", [&] (b2World* world, const tmx::Object& obj) {
            const auto& pos  = obj.getPosition();
            const auto& aabb = obj.getAABB();
            b2Vec2 halfWS = { aabb.width / 2, aabb.height / 2 };

            /**
             * TODO: based on the pickup value, this entity will have 
             * a visual.
             *
             * TODO: remove visual representation from the tile map
             */


            kult::type self = kult::entity();
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
                std::ifstream is(wee::get_resource_path("assets/img") + "pickups.json");
                if(is.is_open()) {
                    is >> j; 
                    from_json(j, *s);
                    is.close();
                }
            }

            visual_t& v = kult::get<visual>(self);
            v.texture = s->_texture;
        

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

                auto& n = kult::get<nested>(self);
                n.offset.x = -.5f * v.src.w;
                n.offset.y = -.5f * v.src.h;
            }
            /**
             * physics stuff
             */
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
                fd.isSensor            = true;
                fd.filter.categoryBits = E_CATEGORY_PICKUP;
                fd.filter.maskBits     = E_CATEGORY_PLAYER;
                fd.userData            = reinterpret_cast<void*>(self);
                body->CreateFixture(&fd);
            }

            kult::get<physics>(self).on_trigger_enter = [&] (const collision& col) {
                DEBUG_METHOD();
                kult::get<physics>(col.self).do_cleanup = true;
                if(kult::has<player>(col.other)) {
                    kult::get<player>(col.other).score += kult::get<pickup>(col.self).value;
                    DEBUG_VALUE_OF(kult::get<player>(col.other).score);
                }
            };

            return self;
        });
    }

    virtual ~register_factories() {
    }
}_;


/**
 * every level chunk has:
 * a spawn point
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
                        DEBUG_LOG("creating object at {}, {}", object.getPosition().x, object.getPosition().y);
                        register_factories::object_factory::instance().create(props["class"], world, object);
                    }
                    const auto& object_properties = object.getProperties();
                    DEBUG_LOG("object has {} properties", 
                        object_properties.size());
                    for(const auto& object_property : object_properties) {
                        if(object_property.getName() == "class") {
                            if(object_property.getStringValue() == "spawn") {
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



kult::type create_sensor(b2World* world, kult::type parent, const vec2f& offset, float radius, const collision_callback&) {
    kult::type self = kult::entity();
    b2Body* body = nullptr;
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
        body->CreateFixture(&def);

    }
    kult::add<physics>(self).body = body;
    kult::add<nested>(parent);
    {
        nested_t& n = kult::get<nested>(parent);
        n.parent = parent;
        n.offset = offset;
        
    }
    return self;
}

entity_type create_rope_between(b2World* world, entity_type a, entity_type b, const b2Vec2& bPosWS) {


    b2Body* body = kult::get<physics>(b).body;
    if(!body) {
        throw std::logic_error("entity must have a physics attached");
    }
    kult::type e = kult::entity();

    b2RopeJointDef def;
    def.userData = reinterpret_cast<void*>(e);
    def.bodyA = kult::get<physics>(a).body; 
    def.bodyB = kult::get<physics>(b).body; 
    def.collideConnected = true;
    // center of player
    def.localAnchorA = b2Vec2 {0.0f, 0.0f};
    // raycast the world here
    def.localAnchorB = body->GetLocalPoint(bPosWS);
    def.maxLength = (def.bodyA->GetPosition() - bPosWS).Length();//SCREEN_TO_WORLD(0.0f);

    kult::add<joint>(e).joint = world->CreateJoint(&def);
    return e;
}


entity_type create_player(b2World* world, const SDL_Point& at) {
    entity_type self = kult::entity();
    b2Body* body = nullptr;
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
        fd.filter.categoryBits = E_CATEGORY_PLAYER;//!0;//(uint16_t)collision_filter::player;
        fd.filter.maskBits = E_CATEGORY_ENVIRONMENT | E_CATEGORY_PICKUP;//!0;//(uint16_t)collision_filter::any;
        fd.density = 1.0f;
        fd.restitution = 0.0f;
        fd.shape = &shape;
        fd.isSensor = false;
        fd.userData = reinterpret_cast<void*>(self);

        body->CreateFixture(&fd);
    }

    SDL_Texture* texture = nullptr;
    {
        auto is = std::ifstream(wee::get_resource_path("assets/img/Assets/PNG/Players/Player Red") + "playerRed_roll.png", std::ios::binary);
        assert(is.is_open());
        texture = assets<SDL_Texture>::instance().load("@player",is);

        kult::add<visual>(self);
        auto& v = kult::get<visual>(self);
        v.texture = texture;
        SDL_QueryTexture(texture, NULL, NULL, &v.src.w, &v.src.h);
        v.src.x = v.src.y = 0;
        auto& n = kult::add<nested>(self);
        n.offset.x = -v.src.w / 2;
        n.offset.y = -v.src.h / 2;

    }
    kult::add<transform>(self);
    kult::add<physics>(self).body = body;
    kult::add<player>(self);

    kult::get<physics>(self).on_collision_enter = [&] (const collision& col) {
        DEBUG_METHOD();
        DEBUG_VALUE_OF(col.self);
    };
    return self;
}

entity_type create_block(b2World* world, const SDL_Rect& r) {
    entity_type self = kult::entity();
    b2Body* body = nullptr;

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
        
        body->CreateFixture(&def);
    }

    kult::add<physics>(self).body = body;
    kult::add<raycast>(self).hit = false;

    return self;
}

kult::type tile(SDL_Texture* texture, const SDL_Point& dst, const SDL_Rect& src, const SDL_RendererFlip& flip, float radians) {
    kult::type self = kult::entity();
    kult::add<visual>(self) = { texture, src, SDL_ColorPresetEXT::White, flip};
    kult::add<transform>(self) = { {(float)dst.x, (float)dst.y}, radians};
    kult::add<nested>(self) = { { 0.f, 0.f }, 0.f, self };

    return self;
}

        auto copy_transform_to_physics = [] () {
            for(auto& e : kult::join<transform, physics>()) {
                const vec2& p = kult::get<transform>(e).position;
                float r = kult::get<transform>(e).rotation;
                kult::get<physics>(e).body->SetTransform({ 
                        SCREEN_TO_WORLD(p.x), 
                        SCREEN_TO_WORLD(p.y) 
                    }, r
                );
            }
        };

        auto copy_physics_to_transform = [] () {
            for(auto& e : kult::join<transform, physics>()) {
                const b2Transform b2t = kult::get<physics>(e).body->GetTransform();
                const b2Vec2& vec = b2t.p;
                
                kult::get<transform>(e).position.x = WORLD_TO_SCREEN(vec.x);
                kult::get<transform>(e).position.y = WORLD_TO_SCREEN(vec.y);
                kult::get<transform>(e).rotation   = kult::get<physics>(e).body->GetAngle();
            }
        };
#include <engine/gui/gamescreen.hpp>
#include <engine/sprite_font.hpp>

struct splash : gamescreen {
    wee::sprite_font* _font;

    virtual void load_content() {
        std::string pt = wee::get_resource_path("assets/ttf") + "BlackCastleMF.ttf";//Boxy-Bold.ttf";
        _font = new wee::sprite_font("@foofont",
	        wee::assets<TTF_Font>::instance().load("@foofont", 32, ::as_lvalue(std::ifstream(pt)))
		);
    }
    virtual void update(int, bool, bool) {}
    virtual void draw(SDL_Renderer* renderer) {
        int w, h;
        SDL_RenderGetLogicalSize(renderer, &w, &h);

    } 
};

class game : public applet {
    b2World* _world;
    bool _mousedown = false;
    int _time_down = 0;
    SDL_Rect _camera;
    entity_type b0, p, b1;
    vec2f _mouse_pos;
    kult::type _rope;
    b2DebugDrawImpl _debugdraw;
    camera _cam;
    b2ContactListenerImpl _contacts;
public:
    game() {
        _debugdraw.SetFlags(
            b2Draw::e_shapeBit          | //= 0x0001, 
            b2Draw::e_jointBit          | //= 0x0002, 
            //b2Draw::e_aabbBit           | //= 0x0004, 
            //b2Draw::e_pairBit           | //= 0x0008, 
            b2Draw::e_centerOfMassBit   | //= 0x0010, 
            //e_particleBit// = 0x0020 
            0
        ) ;
        _world = new b2World({0.0f, 9.8f});
        _world->SetDebugDraw(&_debugdraw);
        _world->SetContactListener(&_contacts);
        _camera = { 0, 0, 0, 0 };
    }
    int load_content() {
        std::string pt = wee::get_resource_path("assets/levels") + "level.tmx";

        SDL_Point spawnPoint;
        parse_tmx(_world, pt, &spawnPoint);
        
        p  = create_player(_world, spawnPoint);
        b1 = create_block(_world, { -16, -300, 32, 32 });
        
        b2Vec2 pa = kult::get<physics>(p).body->GetPosition();
        b2Vec2 temp = { 0.0f, -1000.0f };
        b2Vec2 pb = pa + SCREEN_TO_WORLD(temp);

        b2RayCastClosest rc;
        rc.RayCast(_world, pa, pb);


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
            auto first_gid = tset.getFirstGID();

            auto* tex = wee::assets<SDL_Texture>::instance().load(
                tset.getImagePath(),
                ::as_lvalue(
                    std::ifstream(tset.getImagePath(), std::ios::binary)
                )
            );
            tilesets.insert(std::pair<gid, SDL_Texture*>(first_gid, tex));
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

                    //DEBUG_VALUE_OF(flipFlags);
                    //



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

                    int set_width = ts_w / tile_w;

                    int region_x = (cur_gid % set_width) * tile_w;
                    int region_y = std::floor(cur_gid / set_width) * tile_h;

                    int x_pos = x * tile_w;
                    int y_pos = y * tile_h;

                    auto flipFlags = layer_tiles[ix].flipFlags;
                    int flip = SDL_FLIP_NONE;
                    flip |= (flipFlags & tmx::TileLayer::Horizontal)  ? static_cast<int>(SDL_FLIP_HORIZONTAL)  : static_cast<int>(SDL_FLIP_NONE);
                    flip |= (flipFlags & tmx::TileLayer::Vertical)    ? static_cast<int>(SDL_FLIP_VERTICAL)    : static_cast<int>(SDL_FLIP_NONE);

                    float theta = 0.0f;
                    if(flipFlags & tmx::TileLayer::Diagonal) {
                        theta = 90.0f;
                    }

                    tile(tilesets[tset_gid], 
                        {x_pos, y_pos}, 
                        {region_x, region_y, tile_w, tile_h}, 
                        static_cast<SDL_RendererFlip>(flip), 
                        theta
                    );

                }
            }
        }

        copy_physics_to_transform();
		return 0;
    }

    int update(int dt) {
        copy_transform_to_physics();
        _world->Step(1.0f / (float)60, 4, 3);
        copy_physics_to_transform();

        for(auto& e : kult::join<raycast>()) {
            raycast_t& r = kult::get<raycast>(e);
            if(r.hit) {
                r.hit = false;
                DEBUG_LOG("create new joint");
                _rope = create_rope_between(_world, p, e, b2Vec2{r.point.x, r.point.y});
                break;
            }
        }

        

        b2Vec2 pos = WORLD_TO_SCREEN(kult::get<physics>(p).body->GetPosition());
        _cam.set_position(pos.x, pos.y);
        _cam.update(dt);
        _debugdraw.SetCameraTransform(_cam.get_transform());
        clean_physics(_world);
		return 0;
    }

    

    int draw(SDL_Renderer* renderer) {
        _debugdraw.SetRenderer(renderer);
        SDL_RenderGetLogicalSize(renderer, &_camera.w, &_camera.h);
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);

        _cam.set_viewport(_camera.w, _camera.h);
        {
            /*std::vector<kult::type> entities;*/

            for(const auto& e : kult::join<transform, visual, nested>()) {
                const visual_t& v = kult::get<visual>(e);
                const transform_t& t = kult::get<transform>(e);
                const nested_t& n = kult::get<nested>(e);

                vec3 position = { 
                    t.position.x + n.offset.x,
                    t.position.y + n.offset.y,
                    0.0f
                };

                vec3 positionCS = vec3::transform(position, _cam.get_transform());


                SDL_Rect dst = {
                    (int)(positionCS.x + 0.5f), //cx + (int)(t.p.x + v.offset.x + .5f), 
                    (int)(positionCS.y + 0.5f), //cy + (int)(t.p.y + v.offset.y + .5f),
                    v.src.w, 
                    v.src.h
                };
                SDL_RenderCopyEx(renderer, 
                    v.texture,
                    &v.src,
                    &dst,
                    t.rotation,
                    NULL,
                    v.flip
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

        vec2 pa = kult::get<transform>(p).position;
        vec3 playerPos = vec3::transform ({pa.x, pa.y, 0.0f}, _cam.get_transform());
        pa.x = playerPos.x;
        pa.y = playerPos.y;

        //vec3 mousePositionWS = vec3::transform({_mouse_pos.x, _mouse_pos.y, 0.0f }, _cam.get_transform());
        //vec2 pb = { mousePositionWS.x, mousePositionWS.y };
        //
        //vec3 pbb;
        //_cam.to_screen((vec3){_mouse_pos.x, _mouse_pos.y, 0.0f}, &pbb);
        //vec2 pb = { pa.x + pbb.x, pa.y + pbb.y };
        vec2 pb = { _mouse_pos.x, _mouse_pos.y };
        //pa = WORLD_TO_SCREEN(pa);
        //pb = WORLD_TO_SCREEN(pb);



        SDL_RenderDrawLine(renderer, 
            (int)pa.x, 
            (int)pa.y, 
            (int)pb.x, 
            (int)pb.y
        );
        
        SDL_RenderPresent(renderer);
		return 0;
    }

    int on_click() {

        joint_t& a = kult::get<joint>(_rope);
        if(a.joint) {
            DEBUG_LOG("destroy old joint");
            _world->DestroyJoint(a.joint);
            a.joint = NULL;
        }

        for(auto& e : kult::join<raycast>()) {
            kult::get<raycast>(e).hit = false;
        }
        //int cx = -_camera.x + (_camera.w >> 1);
        //int cy = -_camera.y + (_camera.h >> 1);

        vec2 playerPosition = kult::get<transform>(p).position;

        //b2Vec2 temp = { _mouse_pos.x - cx, _mouse_pos.y - cy };
        vec3 mousePosition = {
            _mouse_pos.x,
            _mouse_pos.y,// - _camera.h / 2, 
            0.0f
        };

        mousePosition = vec3::transform(mousePosition, mat4::inverted(_cam.get_transform()));
        
        b2Vec2 pa = SCREEN_TO_WORLD(b2Vec2(playerPosition.x, playerPosition.y));
        b2Vec2 pb = SCREEN_TO_WORLD(b2Vec2(mousePosition.x, mousePosition.y));


        b2RayCastClosest rc;
        rc.RayCast(_world, pa, pb);
        _cam.shake(1000, false);
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
