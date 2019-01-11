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
#include <base/application.hpp>
#include <base/applet.hpp>
#include <gfx/SDL_RendererEXT.hpp>
#include <gfx/SDL_ColorEXT.hpp>
#include <engine/b2DebugDrawImpl.hpp>
struct int2 {
    int x, y;
};

#define EMPTY_TILE  0 //0x1337

using namespace wee;

using entity_type = kult::type;


void copy_transform_to_physics() {
    for(auto& e : kult::join<transform, physics>()) {
        const vec2& p = kult::get<transform>(e).position;
        float r = kult::get<transform>(e).rotation;
        kult::get<physics>(e).body->SetTransform({ 
            SCREEN_TO_WORLD(p.x), 
            SCREEN_TO_WORLD(p.y) 
            }, r
        );
    }
}
void copy_physics_to_transform() {
    for(auto& e : kult::join<transform, physics>()) {
        const b2Transform b2t = kult::get<physics>(e).body->GetTransform();
        const b2Vec2& vec = b2t.p;

        kult::get<transform>(e).position.x = WORLD_TO_SCREEN(vec.x);
        kult::get<transform>(e).position.y = WORLD_TO_SCREEN(vec.y);
        kult::get<transform>(e).rotation   = kult::get<physics>(e).body->GetAngle();
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

typedef wee::factory<b2Shape, tmx::Object::Shape, const tmx::Object&> b2ShapeFactory;
static class register_factories {
public:
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
            float radius = aabb.width / 2.0f;
            b2Shape* shape = new b2CircleShape;
            ((b2CircleShape*)shape)->m_radius = SCREEN_TO_WORLD(radius);//obj.m_width / 2);
            return shape;
    });
    }
} _gRegisterFactories;
// level > beat > tile > collider  
entity_type create_object([[maybe_unused]]b2World* world, entity_type parent, const tmx::Object& object) {
    DEBUG_METHOD();
    const auto& pos  = object.getPosition(); // offset
    const auto& aabb = object.getAABB();
    b2Vec2 halfWS = { aabb.width / 2, aabb.height / 2 };
    
    float px = pos.x + halfWS.x;
    float py = pos.y + halfWS.y;

    entity_type self = kult::entity();
    {
        kult::add<nested>(self) = {
            { px, py }, // offset
            0.0f, // rotation
            parent
        };

        kult::add<transform>(self);
        {
            // auto cleanup pointer :P 
            auto shape = std::unique_ptr<b2Shape>(b2ShapeFactory::instance().create(object.getShape(), object));

            b2BodyDef bd;
            bd.type  = b2_staticBody;
            bd.position.Set(SCREEN_TO_WORLD(0.f), SCREEN_TO_WORLD(0.f));
            b2Body* body = world->CreateBody(&bd);

            b2FixtureDef fd;
            fd.shape = shape.get();
            body->CreateFixture(&fd);

            kult::add<physics>(self) = {
                body
            };
        }

        //bd.position.Set(SCREEN_TO_WORLD(px), SCREEN_TO_WORLD(py));

    }
    return self;
}

entity_type create_tile(entity_type parent, 
        const int2& offset, 
        const SDL_Texture* texture, 
        const SDL_Rect& src) 
{
    entity_type self = kult::entity();
    {
        kult::add<nested>(self) = {
            { (float)offset.x, (float)offset.y },
            0.0f, 
            parent            
        };

        kult::add<transform>(self);

        auto& vis = kult::add<visual>(self);
        vis.texture = const_cast<SDL_Texture*>(texture);
        vis.src  = src;
        vis.color  = SDL_ColorPresetEXT::White;
        
    }
    return self;
}
entity_type create_beat() {
    entity_type self = kult::entity();
    {
        [[maybe_unused]] auto& n = kult::get<nested>(self);
        [[maybe_unused]] auto& t = kult::get<transform>(self);
    }
    return self;
}
entity_type create_level() {
    entity_type self = kult::entity();
    {
        //auto& n = kult::get<nested>(self);
        [[maybe_unused]] auto& t = kult::get<transform>(self);
    }
    return self;
}

static std::map<int, entity_type> lookup;

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
    auto map_tilesize = mp.getTileSize();
    
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
            

            auto ts_tilesize = tileset->getTileSize();
            auto ts_margin   = tileset->getMargin();
            auto ts_spacing  = tileset->getSpacing();
            auto ts_columns  = tileset->getColumnCount(); 

            SDL_Rect src;
            src.x = (gid % ts_columns) * (ts_tilesize.x + ts_margin) + ts_spacing;
            src.y  = std::floor(gid / ts_columns) * (ts_tilesize.y + ts_margin) + ts_spacing;
            src.w = ts_tilesize.x;
            src.h = ts_tilesize.y;

            std::string pt = tileset->getImagePath();
            std::string resource_path = wee::get_resource_path("");
            pt.replace(0, resource_path.length() - 1, "");

            auto* texture = assets<SDL_Texture>::instance().load(pt, ::as_lvalue(wee::open_ifstream(pt))); 

            int2 offset = { 
                (int)(x * map_tilesize.x),
                (int)(y * map_tilesize.y)
            };

            auto tile = create_tile(kult::none(), offset, texture, src);


            lookup[gid] = tile;

            for(const auto& object: tileset->getTile(gid)->objectGroup.getObjects()) {
                const tmx::Object::Shape& shape = object.getShape();
                const tmx::FloatRect& aabb = object.getAABB();
                DEBUG_VALUE_OF((int)shape);
                DEBUG_VALUE_OF(aabb.left);
                DEBUG_VALUE_OF(aabb.top);
                DEBUG_VALUE_OF(aabb.width);
                DEBUG_VALUE_OF(aabb.height);

                [[maybe_unused]] entity_type object_entity = create_object(world, tile, object);

                for(const auto& property: object.getProperties()) {
                    DEBUG_VALUE_OF(property.getStringValue());
                }
            }
        }
    }
}

void load_tmx(b2World* world, std::vector<std::vector<int> >* res, int32_t* w, int32_t* h) {
    tmx::Map map;
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

/* 
 * requirements:
 * tile gid to entity;
 */

#define TILESIZE    42

struct game : public wee::applet {
    static constexpr int2 kOutputDimension = { 8, 10 };
    static constexpr size_t kOutputSize = kOutputDimension.x * kOutputDimension.y;

    wee::camera _camera;

    std::vector<int> _example;
    int32_t _example_width;
    int32_t _example_height;

    model<int>* _model;
        
    std::vector<std::vector<int> > maps;
    //type* out_map = new type[kOutputSize];
    std::vector<int> _out_map;
    std::vector<int> _in_map;

    b2World* _world;
    wee::b2DebugDrawImpl _debugdraw;

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
    }

    int load_content() {
        load_tmx(_world, &maps, &_example_width, &_example_height);

        _out_map.resize(kOutputSize);
        std::fill(_out_map.begin(), _out_map.end(), -1);

        [[maybe_unused]] static constexpr int kSpawnPointTile = 434;
        [[maybe_unused]] static constexpr int kWaterTile = 11;

        _out_map[0] = 361;
        _out_map[1] = 364;
        _out_map[kOutputDimension.x] = kSpawnPointTile;


        for(auto x: wee::range(kOutputDimension.x)) {
            _out_map[x + (kOutputDimension.y - 1) * kOutputDimension.x] = kWaterTile;
        }
        DEBUG_VALUE_OF(_out_map);

        _in_map = maps[0];
        //DEBUG_VALUE_OF(_in_map);
        _model = new model<int>(&_in_map[0], 
            { _example_height, _example_width },
            &_out_map[0],
            { kOutputDimension.y, kOutputDimension.x }
        );

        _model->ban(kSpawnPointTile);
        //_model->ban(kWaterTile);
        _model->run(&_out_map[0]);

#if  0 //TIMING
        for([[maybe_unused]]auto k: wee::range(1,11)) {
            size_t NRUNS = 1000 * k;
            std::valarray<float> timings(0.0f, NRUNS);
            for(auto i: wee::range(NRUNS)) {              
                auto t_start = std::chrono::high_resolution_clock::now();
                _model->run(&_out_map[0]);
                auto t_end = std::chrono::high_resolution_clock::now();

                auto time_passed = std::chrono::duration<double, std::milli>(t_end-t_start).count();
                timings[i] = time_passed;
            }
            DEBUG_VALUE_OF(NRUNS);
            DEBUG_VALUE_OF(timings.sum());
            DEBUG_VALUE_OF(timings.max());
            DEBUG_VALUE_OF(timings.sum() / NRUNS);
        }

        //DEBUG_VALUE_OF(_out_map);
#endif
        return 0;
    }

#undef ANIMATE_COLLAPSE

    int update(int ) {
        static float camera_x = 0.f;
        camera_x += 1;
        if(camera_x >= TILESIZE) {
            /**
             * move all columns to the left
             */
            for(int y=0; y < kOutputDimension.y; y++) {
                for(int x=0; x < kOutputDimension.x - 1; x++) {
                    std::swap(_out_map[x + y * kOutputDimension.x], _out_map[(x+1) + y * kOutputDimension.x]);
                }
            }
            /**
             * Insert max entropy column.
             */

            for(int i=0; i < kOutputDimension.y; i++) {
                int index = (kOutputDimension.x - 1) + i * kOutputDimension.x;
                _out_map[index] = -1;
                //_model->reset_coeff(index);

            }
            _model->reset(&_out_map[0], _out_map.size(), false);
            _model->run(&_out_map[0]);
            camera_x = 0;
        }

        //nested_to_transform();
#ifdef ANIMATE_COLLAPSE
        _model->step();
        _model->coeff(&_out_map[0]);
#else
        //copy_transform_to_physics();
        //_world->Step(1.0f / (float)60, 4, 3);
        //copy_physics_to_transform();
        //_model->run(&_out_map[0]);
#endif
        _camera.set_position(200.0f + camera_x, 240.f, 0.0f);
        return 0;
    }

    int draw(SDL_Renderer* renderer) {
        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        _camera.set_viewport(w, h);
        _debugdraw.SetRenderer(renderer);
        _debugdraw.SetCameraTransform(_camera.get_transform());
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);
        _world->DrawDebugData();
            for(auto y: wee::range(kOutputDimension.y)) {
                for(auto x: wee::range(kOutputDimension.x)) {
                    size_t i = x + y * kOutputDimension.x;
                    auto m = _out_map[i]; // out_map is *not* the same as coefficients, it is being treated like that now for visual effect

#ifdef ANIMATE_COLLAPSE
                    auto avail = _model->avail(m);
                    if(avail.size() == 0) 
                        continue;

                    auto j = avail[wee::randgen<size_t>(0, avail.size() - 1)];

                        auto z = _model->tile(j);
#else
                        auto z = m;
#endif
                        if(z == EMPTY_TILE)
                            continue;

                        

                        entity_type self = lookup[z];

                        const auto& vis = kult::get<visual>(self);

                        if(!vis.visible)
                            continue;
                        
                        //const auto& tx  = kult::get<transform>(self);
                        //
                        vec3 pos = { (float)TILESIZE * x, (float)TILESIZE * y, 0.0f };
                        pos = vec3::transform(pos, _camera.get_transform());

                        int xx = (int)pos.x;//x * 42;//(int)(tx.position.x + 0.5f); // x * 21
                        int yy = (int)pos.y;//y * 42;//(int)(tx.position.y + 0.5f); // y * 21
                        int w = vis.src.w * 2;
                        int h = vis.src.h * 2;

                        SDL_Rect dst = {
                            xx, yy, w, h
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
                }
            //}
        return SDL_RenderPresent(renderer), 0;
    }

    void set_callbacks(application* app) {
        app->on_mousedown += [this] (char) {
            std::fill(_out_map.begin(), _out_map.end(), 0);
            this->_model->reset(&_out_map[0], _out_map.size());
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
