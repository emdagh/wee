#include "wfc.hpp"
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

#define EMPTY_TILE  0 //0x1337

using namespace wee;

using entity_type = kult::type;

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

typedef tensor<int32_t, 2> vec2i;
// level > beat > tile > collider  
entity_type create_object(entity_type parent, const tmx::Object& object) {
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

        kult::add<physics>(self);
        b2BodyDef bd;
        bd.type = b2_staticBody;
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

static std::map<unsigned int, entity_type> lookup;

void load_tile_layer(const tmx::Map& mp, 
    const tmx::TileLayer* layer, 
    std::vector<unsigned int>* res) 
{
    auto tileset_for_gid = [] (const tmx::Map& m, unsigned int gid) 
        -> const tmx::Tileset*
    { 
        for(const auto& ts: m.getTilesets()) { 
            if(ts.getFirstGID() <= gid) {
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


            /*for(const auto& object: tile->objectGroup.getObjects()) {
                const tmx::Object::Shape& shape = object.getShape();
                const tmx::FloatRect& aabb = object.getAABB();
                DEBUG_VALUE_OF((int)shape);
                DEBUG_VALUE_OF(aabb.left);
                DEBUG_VALUE_OF(aabb.top);
                DEBUG_VALUE_OF(aabb.width);
                DEBUG_VALUE_OF(aabb.height);

                [[maybe_unused]] entity_type object_entity = create_object(tile_entity, object);

                for(const auto& property: object.getProperties()) {
                    DEBUG_VALUE_OF(property.getStringValue());
                }
            }*/
        }
    }
}

void load_tmx(std::vector<std::vector<unsigned int> >* res, uint32_t* w, uint32_t* h) {
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
            std::vector<unsigned int> temp;
            switch(layer->getType()) {
            case tmx::Layer::Type::Tile:
                load_tile_layer(map, reinterpret_cast<tmx::TileLayer*>(layer.get()), &temp);
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

struct game : public wee::applet {
    static constexpr int2 kOutputDimension = { 28, 18};
    static constexpr size_t kOutputSize = kOutputDimension.x * kOutputDimension.y;

    wee::camera _camera;

    std::vector<int> _example;
    uint32_t _example_width;
    uint32_t _example_height;

    model<unsigned int>* _model;
        
    std::vector<std::vector<unsigned int> > maps;
    //type* out_map = new type[kOutputSize];
    std::vector<unsigned int> _out_map;
    std::vector<unsigned int> _in_map;

    int load_content() {
        load_tmx(&maps, &_example_width, &_example_height);

        _out_map.resize(kOutputSize);

        for(auto x: wee::range(kOutputDimension.x)) {
            _out_map[x + (kOutputDimension.y - 1) * kOutputDimension.x] = 11;
        }
        DEBUG_VALUE_OF(_out_map);

        _in_map = maps[0];
        //DEBUG_VALUE_OF(_in_map);
        auto t_start = std::chrono::high_resolution_clock::now();

        _model = new model<unsigned int>(&_in_map[0], 
            { _example_height, _example_width },
            &_out_map[0],
            { kOutputDimension.y, kOutputDimension.x }
        );
                

        /*_wfc::_run<type>(&in_map[0], 
            { (int)_example_width, (int)_example_height }, 
            &_out_map[0], 
            kOutputDimension
        );*/
        auto t_end = std::chrono::high_resolution_clock::now();

        auto time_passed = std::chrono::duration<double, std::milli>(t_end-t_start).count();
        DEBUG_VALUE_OF(_out_map);
        DEBUG_VALUE_OF(time_passed);
        return 0;
    }

    int update(int) {
        nested_to_transform();

        _model->step();
        _model->coeff(&_out_map[0]);

        return 0;
    }

    int draw(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColorEXT(renderer, SDL_ColorPresetEXT::CornflowerBlue);
        SDL_RenderClear(renderer);
            //DEBUG_VALUE_OF(self);
            //
            for(auto y: wee::range(kOutputDimension.y)) {
                for(auto x: wee::range(kOutputDimension.x)) {
                    size_t i = x + y * kOutputDimension.x;
                    auto m = _out_map[i];


                    auto avail = _model->avail(m);

                    if(avail.size() == 0) continue;

                    auto j = avail[wee::randgen<size_t>(0, avail.size() - 1)];

                    //for(auto j: avail) { 
                        auto z = _model->tile(j);
                        if(z == EMPTY_TILE)
                            continue;

                        

                        entity_type self = lookup[z];

                        const auto& vis = kult::get<visual>(self);

                        if(!vis.visible)
                            continue;
                        
                        //const auto& tx  = kult::get<transform>(self);

                        int xx = x * 21;//(int)(tx.position.x + 0.5f);
                        int yy = y * 21;//(int)(tx.position.y + 0.5f);
                        int w = vis.src.w;
                        int h = vis.src.h;

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

    void set_callbacks(application*) {
    }
};

#undef main //SDL idiocy
int main(int, char**) {
    applet* let = new game();
    application app(let);
    static_cast<game*>(let)->set_callbacks(&app);
    return app.start();
}
