/**
 * 2019-01-07
 * TODO:
 *  - add the player entity
 *  - merge gamescreens + manager
 *  - implement A* to estimate difficulty (basic AI agent)
 */

#include <engine/wfc.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <engine/assets.hpp>
#include <engine/ecs.hpp>
#include <engine/camera.hpp>
#include <engine/sprite_font.hpp>
#include <base/application.hpp>
#include <base/applet.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/graphics_device.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <engine/b2DebugDrawImpl.hpp>
#include <engine/b2RayCastImpl.hpp>
#include <engine/b2ContactListenerImpl.hpp>
#include <core/graph.hpp>
#define EMPTY_TILE  0 //0x1337

int g_score = 0;
struct input : wee::singleton<input> {
    bool mouse_down;
    int mouse_x, mouse_y;   
    std::unordered_map<char, bool> keys;
};

#define kCategoryPlayer      (1 << 0)
#define kCategoryEnvironment (1 << 1)
#define kCategoryPickup      (1 << 2)
#define kCategoryDeath       (1 << 3)

using namespace wee;

using entity_type = kult::type;
typedef wee::factory<b2Shape, tmx::Object::Shape, const tmx::Object&> b2ShapeFactory;
typedef wee::factory<b2Body, std::string, b2World*, const tmx::Object&, const kult::type&, void* > b2BodyFactory;

entity_type create_rope(b2World* world, entity_type a, entity_type b, const b2Vec2& bPosWS) {


    b2Body* body = kult::get<physics>(b).body;
    if(!body) {
        throw std::logic_error("entity must have a physics attached");
    }
    entity_type e = kult::entity();
#if 0
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
#else
    b2DistanceJointDef def;
    def.userData = reinterpret_cast<void*>(e);
    def.bodyA = kult::get<physics>(a).body; 
    def.bodyB = kult::get<physics>(b).body; 
    def.localAnchorA = b2Vec2 {0.0f, 0.0f};
    def.localAnchorB = body->GetLocalPoint(bPosWS);
    def.length = (def.bodyA->GetPosition() - bPosWS).Length();
    def.frequencyHz = 5;
    def.dampingRatio = 0.75f;
    def.collideConnected = true;
#endif

    kult::add<joint>(e).joint = world->CreateJoint(&def);
    return e;
}

entity_type create_player(b2World* world, const vec2f& at) {
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
        fd.filter.categoryBits = kCategoryPlayer;//!0;//(uint16_t)collision_filter::player;
        fd.filter.maskBits = kCategoryEnvironment | kCategoryPickup | kCategoryDeath;
        fd.density = 1.0f;
        fd.restitution = 0.0f;
        fd.shape = &shape;
        fd.isSensor = false;
        fd.userData = reinterpret_cast<void*>(self);

        body->CreateFixture(&fd);
    }

    SDL_Texture* texture = nullptr;
    {
        auto is = open_ifstream("assets/img/player.png", std::ios::binary);
        assert(is.is_open());
        texture = assets<SDL_Texture>::instance().load("@player",is);

        kult::add<visual>(self);
        auto& v = kult::get<visual>(self);
        v.texture = texture;
        SDL_QueryTexture(texture, NULL, NULL, &v.src.w, &v.src.h);
        v.src.x = v.src.y = 0;
        v.visible = true;
        /*n.offset.x = -v.src.w / 2;
        n.offset.y = -v.src.h / 2;*/
    }
    kult::add<transform>(self);
    kult::add<physics>(self).body = body;
#if 0
    {
        auto& pp = kult::add<player>(self);
        pp.score = 0;
        pp.hp = 3;
        pp.is_shake = false;
        pp.is_flash = false;
    }
#endif

    kult::get<physics>(self).on_collision_enter = [&] (const collision& col) {
    };
    return self;
}


template <typename T>
struct prototype {
    virtual T* clone() = 0;
};

struct tile : public prototype<tile> {
    kult::type self;
    b2World* _world;
    unsigned int _gid;
    const tmx::Tileset* _tileset;

    tile(b2World* world, unsigned int gid, const tmx::Tileset* tileset) 
        : self(kult::entity())
        , _world(world)
        , _gid(gid)
        , _tileset(tileset)
    {
        auto ts_tilesize = tileset->getTileSize();
        auto ts_margin   = tileset->getMargin();
        auto ts_spacing  = tileset->getSpacing();
        auto ts_columns  = tileset->getColumnCount(); 

        [[maybe_unused]] SDL_Rect src;
        src.x = (gid % ts_columns) * (ts_tilesize.x + ts_margin) + ts_spacing;
        src.y  = std::floor(gid / ts_columns) * (ts_tilesize.y + ts_margin) + ts_spacing;
        src.w = ts_tilesize.x;
        src.h = ts_tilesize.y;

        std::string pt = tileset->getImagePath();
        std::string resource_path = wee::get_resource_path("");
        pt.replace(0, resource_path.length() - 1, "");

        auto* texture = assets<SDL_Texture>::instance().load(pt, ::as_lvalue(wee::open_ifstream(pt))); 
        
        kult::add<transform>(self);
        kult::add<visual>(self);
        kult::add<physics>(self);

        kult::get<visual>(self).src     = src;
        kult::get<visual>(self).texture = texture;
        kult::get<visual>(self).visible = false;

        const auto& objects = tileset->getTile(gid + 1)->objectGroup.getObjects(); // +1 for some BS correction in txmlite
        
        for(const auto& object: objects) {

            for(const auto& p: object.getProperties()) {
                if(p.getName() == "class") {
                    std::string classname = p.getStringValue();
                    b2Body* body = b2BodyFactory::instance().create(classname, world, object, self, this);
                    if(body != nullptr) {
                        kult::get<physics>(self).body = body;
                        body->SetActive(false);
                    }
                }
            }

        }

        

    }
    /**
     * make use of delegate constructors
     */
    tile(const tile& other) : tile(other._world, other._gid, other._tileset)
    {

    }

    void destroy() {
        if(kult::has<physics>(self)) {
            b2Body* rb = kult::get<physics>(self).body;
            if(rb != nullptr) {
                for(auto* ptr = rb->GetFixtureList(); ptr; ptr = ptr->GetNext()) {
                    rb->DestroyFixture(ptr);
                }
                _world->DestroyBody(rb);
            }
        }
        kult::purge(self);
    }

    virtual ~tile() {
        destroy();
    }

    tile& operator = (tile other) {
        std::swap(self, other.self);
        return *this;
    }

    virtual tile* clone() {
        return new tile(*this);
    }

    void set_active(bool b) {
        b2Body* rb = kult::get<physics>(self).body;
        if(rb)
            rb->SetActive(b);

    }

    void set_position(float x, float y) {
        kult::get<transform>(self).position = { x, y };
        if(kult::has<physics>(self)) {
            auto* b =kult::get<physics>(self).body;
            if(b) {
                kult::get<physics>(self).body->SetTransform(
                    b2Vec2{SCREEN_TO_WORLD(x), SCREEN_TO_WORLD(y)},
                    0.0f);
            }
        }
    }

    void set_position(const vec2f& a) {
        set_position(a.x, a.y);
    }

    const vec2f& get_position() const {
        return kult::get<transform>(self).position;
    }

    void set_visible(bool b) {
        kult::get<visual>(self).visible = b;
    }
};

struct tile_factory : public wee::singleton<tile_factory> {
    std::unordered_map<unsigned int, tile*> _tiles;

    void register_tile(unsigned int gid, tile* t) {
        _tiles[gid] = t;        
    }

    tile* create(unsigned int gid) {
        return _tiles.find(gid) != _tiles.end() ? _tiles[gid]->clone() : NULL;
    }
};


struct int2 {
    int x, y;
};



void copy_transform_to_physics() {
    for(auto& e : kult::join<transform, physics>()) {
        const vec2& p = kult::get<transform>(e).position;
        float r = kult::get<transform>(e).rotation;
        b2Body* b = kult::get<physics>(e).body;
        if(b != nullptr) {
            b->SetTransform({ 
                SCREEN_TO_WORLD(p.x), 
                SCREEN_TO_WORLD(p.y) 
                }, r
            );
        }
    }
}
void copy_physics_to_transform() {
    for(auto& e : kult::join<transform, physics>()) {
        b2Body* b = kult::get<physics>(e).body;
        if(b != nullptr) {
            const b2Transform b2t = b->GetTransform();
            const b2Vec2& vec = b2t.p;

            kult::get<transform>(e).position.x = WORLD_TO_SCREEN(vec.x);
            kult::get<transform>(e).position.y = WORLD_TO_SCREEN(vec.y);
            kult::get<transform>(e).rotation   = kult::get<physics>(e).body->GetAngle();
        }
    }
}

void nested_to_transform() {
    for(auto& self : kult::join<nested, transform>()) {
        const auto& n = kult::get<nested>(self);
        if(kult::has<transform>(n.parent)) {
            const auto& pt = kult::get<transform>(n.parent);

            vec2f pos = vec2f::rotate_at(n.offset, pt.position, pt.rotation);

            kult::get<transform>(self).position = pos;
            kult::get<transform>(self).rotation = kult::get<transform>(n.parent).rotation + n.rotation;
        } else {
            kult::get<transform>(self).position = n.offset;
            kult::get<transform>(self).rotation = n.rotation;
        }
    }
}

static class register_factories {
public:

    register_factories() {
    b2BodyFactory::instance().register_class("pickup", [] (b2World* world, const tmx::Object& object, const kult::type& self, void* userData) {
        DEBUG_LOG("created pickup");
        auto shape = std::unique_ptr<b2Shape>(b2ShapeFactory::instance().create(object.getShape(), object));
        
        const auto& pos  = object.getPosition();
        const auto& aabb = object.getAABB();
        b2Vec2 halfWS = { aabb.width / 2, aabb.height / 2 };
        
        float px = pos.x + halfWS.x;
        float py = pos.y + halfWS.y;

        b2BodyDef bd;
        bd.type  = b2_staticBody;
        bd.position.Set(SCREEN_TO_WORLD(px), SCREEN_TO_WORLD(py));
        bd.userData = userData;
        b2Body* body = world->CreateBody(&bd);
        
        int value = 0;

        for(const auto& p: object.getProperties()) {
            if(p.getName() == "value") {
                value = p.getIntValue();
                DEBUG_VALUE_OF(value);
            }
        }

        b2FixtureDef fd;
        fd.isSensor            = true;
        fd.filter.categoryBits = kCategoryPickup;
        fd.filter.maskBits     = kCategoryPlayer;
        fd.userData            = reinterpret_cast<void*>(self);
        fd.shape = shape.get();
        body->CreateFixture(&fd);
        
        kult::get<physics>(self).on_trigger_enter = [value] (const collision& col) {
            tile* t = static_cast<tile*>(kult::get<physics>(col.self).body->GetUserData());
            DEBUG_VALUE_OF(value);
            g_score += value;
            t->destroy();
        };


        return body;

    });
    b2BodyFactory::instance().register_class("death", [] (b2World* world, const tmx::Object& object, const kult::type& self, void* userData) {
        auto shape = std::unique_ptr<b2Shape>(b2ShapeFactory::instance().create(object.getShape(), object));
        
        const auto& pos  = object.getPosition();
        const auto& aabb = object.getAABB();
        b2Vec2 halfWS = { aabb.width / 2, aabb.height / 2 };
        
        float px = pos.x + halfWS.x;
        float py = pos.y + halfWS.y;

        b2BodyDef bd;
        bd.type  = b2_staticBody;
        bd.position.Set(SCREEN_TO_WORLD(px), SCREEN_TO_WORLD(py));
        bd.userData = userData;
        b2Body* body = world->CreateBody(&bd);
        

        b2FixtureDef fd;
        fd.isSensor            = false;
        fd.filter.categoryBits = kCategoryEnvironment;
        fd.filter.maskBits     = kCategoryPlayer;
        fd.restitution         = 0.0f;
        fd.userData            = reinterpret_cast<void*>(self);
        fd.shape = shape.get();
        body->CreateFixture(&fd);

        return body;

    });

    b2BodyFactory::instance().register_class("env", [] (b2World* world, const tmx::Object& object, const kult::type& self, void* userData) {
        auto shape = std::unique_ptr<b2Shape>(b2ShapeFactory::instance().create(object.getShape(), object));
        
        const auto& pos  = object.getPosition();
        const auto& aabb = object.getAABB();
        b2Vec2 halfWS = { aabb.width / 2, aabb.height / 2 };
        
        float px = pos.x + halfWS.x;
        float py = pos.y + halfWS.y;

        b2BodyDef bd;
        bd.type  = b2_staticBody;
        bd.position.Set(SCREEN_TO_WORLD(px), SCREEN_TO_WORLD(py));
        bd.userData = userData;
        b2Body* body = world->CreateBody(&bd);
        

        b2FixtureDef fd;
        fd.isSensor            = false;
        fd.filter.categoryBits = kCategoryEnvironment;
        fd.filter.maskBits     = kCategoryPlayer;
        fd.userData            = reinterpret_cast<void*>(self);
        fd.shape = shape.get();
        body->CreateFixture(&fd);

        //hack!
        kult::add<raycast>(self);
        kult::get<raycast>(self).hit = false;
        //!kcah

        return body;

    });
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
    }
} _gRegisterFactories;
// level > beat > tile > collider  
static std::map<int, entity_type> lookup; // lookup table of prototypes

void load_tile_layer([[maybe_unused]]b2World* world, const tmx::Map& mp, 
    const tmx::TileLayer* layer, 
    std::vector<int>* res) 
{
    auto tileset_for_gid = [] (const tmx::Map& m, int gid) 
        -> const tmx::Tileset*
    { 
        for(const auto& ts: m.getTilesets()) { 
            if((int)ts.getFirstGID() <= gid) {
                return &ts;
            }
        }
        return NULL;
    };

    auto mapdim = mp.getTileCount();
    //auto map_tilesize = mp.getTileSize();
    
    const auto& tiles = layer->getTiles();

    for(size_t y=0; y < mapdim.y; y++) {
        for(size_t x=0; x < mapdim.x; x++) {
            size_t i = x + y * mapdim.x;

            if(tiles[i].ID == 0) {
                res->push_back(EMPTY_TILE);
                continue;
            }

            unsigned int gid = tiles[i].ID;
            /**
             * here we should acquire the correct tileset for this GID and subtract it's
             * first GID from the one as observed in the tile. This is to `normalize`
             * the tile GID before delinearizing into texture space.
             */
            const tmx::Tileset* tileset = tileset_for_gid(mp, gid);;
            gid -= tileset->getFirstGID();

            res->push_back(gid);
            if(lookup.count(gid) != 0) 
                continue;
            
            tile_factory::instance().register_tile(gid, new tile(world, gid, tileset));
            lookup[gid] = 1;

        }
    }
}

void load_tmx(b2World* world, std::vector<std::vector<int> >* res, int32_t* w, int32_t* h) {
    tmx::Map& map = *(new tmx::Map);
    if(map.load(wee::get_resource_path("assets/levels") + "example.tmx")) {
        auto mapdim = map.getTileCount();
        *w = mapdim.x;
        *h = mapdim.y;

        for(const auto& tileset: map.getTilesets()) {
            //int firstgid = tileset.getFirstGID();
            for(const auto& tileset_property : tileset.getProperties()) {
                DEBUG_VALUE_OF(tileset_property.getStringValue());
            }
        }
        for(const auto& layer: map.getLayers()) {
            DEBUG_VALUE_OF(layer->getName());
            std::vector<int> temp;
            switch(layer->getType()) {
            case tmx::Layer::Type::Tile:
                load_tile_layer(world, map, reinterpret_cast<tmx::TileLayer*>(layer.get()), &temp);
                res->push_back(temp);
                break;
            default:
                break;//throw not_implemented();
            }
        }
    }
}

class navigation_mesh {

    void create_columns(const std::vector<int>& tilemap, const int2& size) {
        std::vector<bool> has_node(tilemap.size(), false); 
        for(int y=0; y < size.y - 1; y++) {
            for(int x=0; x < size.x; x++) {
                int tile_0 = tilemap[x + y * size.x];
                int tile_1 = tilemap[x + (y + 1) * size.x];
                if(tile_0 == 0 && tile_1 != 0) { // border 
                    has_node[x + y * size.x] = true;
                }
            }
        }
        for(int y=0; y < size.y - 1; y++) {
            for(int x=0; x < size.x; x++) {
                                
            }
        }
    }
};

typedef wee::basic_weighted_graph<int, float> weighted_graph;
void map_to_graph(const std::vector<int>& tilemap, const int2& size, weighted_graph* g) {
}

/* 
 * requirements:
 * tile gid to entity;
 */

#define TILESIZE    21


template <typename T, typename Allocator = std::allocator<T> >
class object_cache {
    T* _data;
    std::vector<T*> _avail;
public:
    object_cache(size_t size) 
    : _data(new T[size])
    , _avail(&_data, &_data + size)
    {
    }

    ~object_cache() {
        delete[] _data;
    }

    T* next() {
        T* res = _avail.back();
        _avail.pop_back();
        return res;
    }

    void free(T* item) {
        _avail.push_back(item);
    }
};

struct game : public wee::applet {
    static constexpr int2 kOutputDimension = { 16, 10 };
    static constexpr size_t kOutputSize = kOutputDimension.x * kOutputDimension.y;

    sprite_font* _font;
    wee::camera _camera;

    std::vector<int> _example;
    int32_t _example_width;
    int32_t _example_height;

    wfc_model<int>* _model;
        
    std::vector<std::vector<int> > maps;
    //type* out_map = new type[kOutputSize];
    std::vector<int> _out_map;
    std::vector<int> _in_map;

    //object_cache<kult::type> _cache = object_cache<kult::type>(kOutputSize * 64);
    std::vector<tile*> _tiles;

    b2World* _world;
    wee::b2DebugDrawImpl _debugdraw;
    wee::b2ContactListenerImpl _contacts;

    kult::type _player, _rope;

    bool _clicked = false;

    game() {
        _debugdraw.SetFlags(
            b2Draw::e_shapeBit          | //= 0x0001, 
            b2Draw::e_jointBit          | //= 0x0002, 
            b2Draw::e_aabbBit           | //= 0x0004, 
            //b2Draw::e_pairBit           | //= 0x0008, 
            b2Draw::e_centerOfMassBit   | //= 0x0010, 
            //e_particleBit// = 0x0020 
            0
            ) ;
        _world = new b2World({0.0f, 9.8f});
        _world->SetDebugDraw(&_debugdraw);
        _world->SetContactListener(&_contacts);
    }

    void reset() {
    }

    int load_content() {
        DEBUG_METHOD();
        load_tmx(_world, &maps, &_example_width, &_example_height);

        _out_map.resize(kOutputSize);
        std::fill(_out_map.begin(), _out_map.end(), -1);

        [[maybe_unused]] static constexpr int kSpawnPointTile = 434;
        [[maybe_unused]] static constexpr int kWaterTile = 11;

        std::fill(&_out_map[1], &_out_map[kOutputDimension.x], 362);
        _out_map[0] = 361;
        //_out_map[1] = 364;
        _out_map[kOutputDimension.x] = kSpawnPointTile;


        for(auto x: wee::range(kOutputDimension.x)) {
            _out_map[x + (kOutputDimension.y - 1) * kOutputDimension.x] = kWaterTile;
        }
        DEBUG_VALUE_OF(_out_map);

        _in_map = maps[0];
        _model = new wfc_model<int>(&_in_map[0], 
            { _example_height, _example_width },
            &_out_map[0],
            { kOutputDimension.y, kOutputDimension.x }
        );

        _model->ban(kSpawnPointTile);
        _model->run(&_out_map[0]);

        DEBUG_VALUE_OF(_out_map);

        for(int y=0; y < kOutputDimension.y; y++) {
            for(int x=0; x < kOutputDimension.x; x++) {
                int i = x + y * kOutputDimension.x;
                auto it = _out_map[i];
                auto* new_tile = tile_factory::instance().create(it);
                if(new_tile) {
                    new_tile->set_position(x * TILESIZE, y * TILESIZE);
                    new_tile->set_visible(true);
                    new_tile->set_active(true);
                }
                _tiles.push_back(new_tile);
            }
        }

        std::string pt = wee::get_resource_path("assets/fonts") + "Boxy-Bold.ttf";
        _font = new wee::sprite_font("@foofont",
	        wee::assets<TTF_Font>::instance().load(pt, 32, ::as_lvalue(std::ifstream(pt)))
		);
        _camera.set_zoom(1.5f);

        _player = create_player(_world, vec2f{kOutputDimension.x * TILESIZE / 2, kOutputDimension.y * TILESIZE / 2});
        copy_physics_to_transform();

#if 1 
    b2Vec2 pa = kult::get<physics>(_player).body->GetPosition();
    b2Vec2 temp = { 0.0f, -1000.0f };
    b2Vec2 pb = pa + SCREEN_TO_WORLD(temp);
    
    joint_t& a = kult::get<joint>(_rope);
    if(a.joint) {
        DEBUG_LOG("destroy old joint");
        _world->DestroyJoint(a.joint);
        a.joint = NULL;
    }
    
    b2RayCastClosest rc;
    rc.RayCast(_world, pa, pb);
#endif
        return 0;
    }


#undef ANIMATE_COLLAPSE

    int scroll_map(float& camera_x) {
        camera_x += 1;

        for(auto y: range(kOutputDimension.y)) {
            for(auto x: range(kOutputDimension.x)) {
                int i = x + y * kOutputDimension.x;
                if(_tiles[i] != nullptr) {
                    _tiles[i]->set_position(x * TILESIZE - camera_x, y * TILESIZE);

                }
            }
        }

        if(((int)camera_x % TILESIZE) == 0) {
            /**
             * free the column on the left-most edge.
             */
            for(int i=0; i < kOutputDimension.y; i++) {
                int index = i * kOutputDimension.x;
                //_out_map[index] = -1;
                if(_tiles[index] != nullptr) 
                    _tiles[index]->destroy();
            }

            /**
             * move all columns to the left
             */
            for(int y=0; y < kOutputDimension.y; y++) {
                for(int x=0; x < kOutputDimension.x - 1; x++) {
                    int i0 = (x+0) + y * kOutputDimension.x;
                    int i1 = (x+1) + y * kOutputDimension.x;



                    std::swap(_out_map[i0], _out_map[i1]);
                    std::swap(_tiles[i0], _tiles[i1]);
                }
            }
            /**
             * Insert max entropy column.
             */

            for(int i=0; i < kOutputDimension.y; i++) {
                int index = (kOutputDimension.x - 1) + i * kOutputDimension.x;
                _out_map[index] = -1;
            }
            _model->reset(&_out_map[0], _out_map.size(), false);
            _model->run(&_out_map[0]);
            
            camera_x = 0;
            return 0;
        }
        return -1;
    }

    
    int on_click() {
        DEBUG_METHOD();
        _clicked = true;
        return 0;
    }

    void handle_input() {
        static const int clickTimeout = 500;
        static int mouseWasDown = 0;
        static int mouseDownTime = 0;
        if(input::instance().mouse_down) {
            if(mouseWasDown == 0) {
                mouseDownTime = SDL_GetTicks();
            }
            mouseWasDown = 1;
        } else {
            if(mouseWasDown) {
                mouseWasDown = 0;
                auto delta = SDL_GetTicks() - mouseDownTime;
                if((delta) < clickTimeout) {
                    on_click();
                }
            }
        }
    }

    float camera_x = 0;

    int update(int ) {
        handle_input();
        /*
         * scroll the map and insert new tile row when necessary
         */

#if 1 
        bool map_did_scroll = scroll_map(camera_x) == 0;

        if(map_did_scroll) {
            /*for(auto* tile: _tiles) {
                if(tile) {
                    tile->set_position(tile->get_position() - vec2f{(float)TILESIZE, 0.0f});
                }
            }*/
            for(auto y: range(kOutputDimension.y)) {
                auto x = kOutputDimension.x - 1;
                auto i = x + y * kOutputDimension.x;

                auto it = _out_map[i];
                auto* new_tile = tile_factory::instance().create(it);
                if(new_tile != nullptr) {
                    new_tile->set_position(x * TILESIZE, y * TILESIZE);
                    new_tile->set_visible(true);
                    new_tile->set_active(true);
                }
                _tiles[i] = new_tile;
            }
        }
#endif
        //_model->run(&_out_map[0]);
        //_camera.set_position((kOutputDimension.x * TILESIZE / 2) + camera_x, (kOutputDimension.y * TILESIZE) / 2, 0.0f);
        _camera.set_position((kOutputDimension.x * TILESIZE / 2), (kOutputDimension.y * TILESIZE) / 2, 0.0f);
        //kult::get<transform>(_player).position.x = camera_x;



        for(auto& e : kult::join<raycast>()) {
            auto& r = kult::get<raycast>(e);

            if(r.hit) {
                r.hit = false;
                _rope = create_rope(_world, _player, e, b2Vec2{r.point.x, r.point.y});
                //if(_current_beat != n.parent) {
                //    _spawnedNextBeat = false;
                //    DEBUG_LOG("new beat entered");
                //}
                break;
            }
        }

        //if(input::instance().keys.count('w')) {
        //    if(input::instance().keys['w'] == true) {
                joint_t& a = kult::get<joint>(_rope);
                if(a.joint) {
                    //b2RopeJoint* j = static_cast<b2RopeJoint*>(a.joint);
                    //j->SetMaxLength(j->GetMaxLength() - 0.0251f);
                    b2DistanceJoint* j = static_cast<b2DistanceJoint*>(a.joint);
                    j->SetLength(std::max(SCREEN_TO_WORLD(16.f), j->GetLength() - 0.0125f));
                }
            //}
        //}

        static int is_even = 1;
        if(_clicked) {

            joint_t& a = kult::get<joint>(_rope);
            if(a.joint) {
                DEBUG_LOG("destroy old joint");
                _world->DestroyJoint(a.joint);
                a.joint = NULL;
            }
            for(auto& e : kult::join<raycast>()) {
                kult::get<raycast>(e).hit = false;
            }

            //if(is_even & 1) {
                vec2 playerPosition = kult::get<transform>(_player).position;
                vec3 mousePosition = {
                    (float)input::instance().mouse_x,
                    (float)input::instance().mouse_y,// - _camera.h / 2, 
                    0.0f
                };
                mousePosition = vec3::transform(mousePosition, mat4::inverted(_camera.get_transform()));
                b2Vec2 pa = SCREEN_TO_WORLD(b2Vec2(playerPosition.x, playerPosition.y));
                b2Vec2 pb = SCREEN_TO_WORLD(b2Vec2(mousePosition.x, mousePosition.y));
                b2RayCastClosest rc;
                rc.RayCast(_world, pa, pb);
            //}
            is_even++;
            _clicked = false;
        }
        
        copy_transform_to_physics();
        _world->Step(1.0f / (float)60, 4, 3);
        copy_physics_to_transform();

        return 0;
    }
	void draw_string(SDL_Renderer* renderer, 
            wee::sprite_font* font, 
            const std::string& what, 
            const SDL_Point& where, 
            const SDL_Color& color) {
        SDL_SetTextureColorMod(font->_texture, color.r, color.g, color.b);;
        int x = where.x;
        int y = where.y;
        const wee::font_info& finfo = font->_info;
        int yy = y - finfo.height - finfo.descent;// + (finfo.ascent + finfo.descent);
        for(const auto& c : what) {
            const wee::glyph_info& info = font->_ginfo[c];
            const SDL_Rect& src = font->get(c);
            SDL_Rect dst = {
                x + info.minx, 
                yy,// - info.maxy,
                src.w, src.h
            };
            SDL_RenderCopy(renderer, font->_texture, &src, &dst);
            x += info.advance;
        }
	}

    int draw(graphics_device* dev) {

        SDL_Renderer* renderer = dev->get_renderer();
        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        _camera.set_viewport(w, h);
        _debugdraw.SetRenderer(renderer);
        _debugdraw.SetCameraTransform(_camera.get_transform());
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);
#if 0
        joint_t& a = kult::get<joint>(_rope);
        if(a.joint) {

            const b2Vec2& pa = a.joint->GetAnchorA();
            const b2Vec2& pb = a.joint->GetAnchorB();

            vec3 pa_ws { WORLD_TO_SCREEN(pa.x), WORLD_TO_SCREEN(pa.y), 0.0f };
            vec3 pb_ws { WORLD_TO_SCREEN(pb.x), WORLD_TO_SCREEN(pb.y), 0.0f };

            pa_ws = vec3::transform(pa_ws, _camera.get_transform());
            pb_ws = vec3::transform(pb_ws, _camera.get_transform());

            int x0 = (int)(pa_ws.x);
            int x1 = (int)(pb_ws.x);
            int y0 = (int)(pa_ws.y);
            int y1 = (int)(pb_ws.y);

            SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::Black);
            SDL_RenderDrawLine(renderer, x0, y0, x1, y1); 
        }

#endif
        for(auto self: kult::join<transform, visual>()) {

            const auto& vis = kult::get<visual>(self);
            const auto& tr  = kult::get<transform>(self);

            if(!vis.visible)
                continue;
           
            vec3 pos = { tr.position.x, tr.position.y, 0.0f };
            pos = vec3::transform(pos, _camera.get_transform());
            vec3 size = vec3{
                (float)vis.src.w, 
                (float)vis.src.h, 
                0.0f
            };
            size = size * _camera.get_zoom(); 

            
            SDL_Rect dst = {
                (int)((pos.x - size.x * 0.5f) + 0.5f), 
                (int)((pos.y - size.y * 0.5f) + 0.5f),
                (int)(size.x + 0.5f), 
                (int)(size.y + 0.5f)
            };

            SDL_RenderCopyEx(renderer, 
                vis.texture,
                &vis.src,
                &dst,
                0.0f,
                NULL,
                SDL_FLIP_NONE
            );

        }
        draw_string(renderer, _font, "score: " + std::to_string(g_score) , { 10, 64 }, SDL_ColorPresetEXT::Black);
        //_world->DrawDebugData();
        return SDL_RenderPresent(renderer), 0;
    }

    void set_callbacks(application* app) {

        app->on_keypress += [&] (char key) {
            //DEBUG_VALUE_OF(key);
            input::instance().keys[key] = true;
            return 0;
        };
        app->on_keyrelease += [&] (char key) {
            //DEBUG_VALUE_OF(key);
            input::instance().keys[key] = false;
            return 0;
        };

        app->on_mousedown += [&] (char) {
            input::instance().mouse_down = true;
            return 0;
        };
        app->on_mouseup += [] (char) {
            input::instance().mouse_down = false;
            return 0;
        };
        app->on_mousemove += [] (int x, int y) {
            input::instance().mouse_x = x;
            input::instance().mouse_y = y;
            
            return 0;
        };
    }
};

#undef main //SDL idiocy
int main(int, char**) {
    applet* let = new game();
    application app(let);
    static_cast<game*>(let)->set_callbacks(&app);
    return app.start();
}
