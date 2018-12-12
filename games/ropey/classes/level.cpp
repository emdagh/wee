#include <classes/level.hpp>
#include <classes/common.hpp>
#include <classes/components.hpp>
#include <classes/factories.hpp>
#include <core/logstream.hpp>
#include <cmath>
#include <Box2D/Box2D.h>
#include <classes/entities.hpp>
#include <engine/assets.hpp>

using namespace wee;

void level::_parse_tileset(tmx::Tileset&) {
}
void level::_parse_layer(tmx::Layer::Ptr) {
}
void level::_parse_tile_layer(const tmx::TileLayer* layer) {

    /**
     * Get and store the map size
     */
    auto dim = _map->getTileCount();
    int map_w = dim.x;
    int map_h = dim.y;
    /**
     * Get and store tile size (uniform across map)
     */
    auto tile_size = _map->getTileSize();
    int map_tw = tile_size.x;
    int map_th = tile_size.y;

    kult::get<beat>(_self).width = map_tw * map_w;

    auto& tiles = layer->getTiles();

    for(int y=0; y < map_h; y++) {
        for(int x=0; x < map_w; x++) {
            int ix = x + y * map_w;

            int cur_gid = tiles[ix].ID;

            if(0 == cur_gid)
                continue;
            
            int tset_gid = -1;
            
            tileset_info* current_tileset = nullptr;

            for(auto& ts : _tilesets) {
                if(ts.first <= cur_gid) {
                    tset_gid = ts.first;
                    current_tileset = &ts.second;
                    break;
                }
            }
            
            if(tset_gid == -1) 
                continue;

            cur_gid -= tset_gid; // ???

#if 1 
            int ts_w, ts_h;
            SDL_QueryTexture(current_tileset->texture, NULL, NULL, &ts_w, &ts_h);

            int tset_tw = current_tileset->tile_width;
            int tset_th = current_tileset->tile_height;

            int spacing = current_tileset->spacing;
            int margin = current_tileset->margin;

            int set_width = ts_w / (tset_tw + (spacing + margin) / 2); 

            int region_x = (cur_gid % set_width) * (tset_tw + 2) + 2;
            int region_y = std::floor(cur_gid / set_width) * (tset_th + 2) + 2;

            int x_pos = x * map_tw;
            int y_pos = y * map_th;

            auto flipFlags = tiles[ix].flipFlags;
            int flip = SDL_FLIP_NONE;
            flip |= (flipFlags & tmx::TileLayer::Horizontal)  ? static_cast<int>(SDL_FLIP_HORIZONTAL)  : static_cast<int>(SDL_FLIP_NONE);
            flip |= (flipFlags & tmx::TileLayer::Vertical)    ? static_cast<int>(SDL_FLIP_VERTICAL)    : static_cast<int>(SDL_FLIP_NONE);

            float theta = 0.0f;
            if(flipFlags & tmx::TileLayer::Diagonal) {
                theta = 90.0f;
            }

            const auto& ts_info = _tilesets[tset_gid];

            auto tile = 
            create_tile(ts_info.texture, 
                {x_pos, y_pos}, 
                {
                    region_x , 
                    region_y, 
                    tset_tw, 
                    tset_th    
                }, 
                static_cast<SDL_RendererFlip>(flip), 
                theta
            );
            {
                kult::get<nested>(tile).parent = _self;
            }
#endif
        }
    }
}
void level::_parse_object_layer(const tmx::ObjectGroup* ptr) {
    for(const auto& object : ptr->getObjects()) {
        for(const auto& prop : object.getProperties()) {
            if(prop.getName() == "class") {
                if(prop.getStringValue() == "spawn") {
                    const auto& pos = object.getPosition();
                    kult::get<beat>(_self).spawn = { pos.x, pos.y };
                } else {
                    entity_type ent;
                    object_factory::instance().create(prop.getStringValue(), 
                        _world,
                        object,
                        ent
                    );
                    kult::add<nested>(ent).parent = _self;
                }
            }
        }
    }
}

void level::_parse_image_layer(tmx::Layer::Ptr) {
}

level& level::source(tmx::Map* ref) {
    return _map = ref, *this ;
}

level& level::world(b2World* world) {
    return _world = world, *this;
}

level& level::offset(const wee::vec2f& val) {
    return _offset = val, *this;
}

entity_type level::build() {
    if(!_map || !_world) {
        throw std::logic_error("builder incomplete!");
        assert(false);
    }

    _self = kult::entity();
    kult::add<beat>(_self).respawn = 0;
    kult::add<transform>(_self) = {  _offset, 0.0f };


    for(const auto& i : _map->getProperties()) {

        if(i.getName() == "difficulty")
            DEBUG_VALUE_OF(i.getName());
            DEBUG_VALUE_OF(i.getStringValue());
            kult::get<beat>(_self).difficulty = std::atoi(i.getStringValue().c_str());
    }
    DEBUG_LOG("Loading tilesets...");
    for(auto& tset : _map->getTilesets()) {
        auto first_gid = tset.getFirstGID();

        const auto& ts_properties = tset.getProperties();
        DEBUG_VALUE_OF(ts_properties.size());
        
        tileset_info info;

        for(const auto& i : ts_properties) {

            if(i.getName() == "uri") {
                info.texture = wee::assets<SDL_Texture>::instance().get(i.getStringValue()
                    /*tset.getImagePath(),
                    ::as_lvalue(
                        std::ifstream(tset.getImagePath(), std::ios::binary)
                    )*/
                );
            }
        }
        const auto& size = tset.getTileSize();
        info.tile_width = size.x;
        info.tile_height = size.y;
        info.margin  = tset.getMargin();
        info.spacing = tset.getSpacing();

        _tilesets.insert(std::pair<int, tileset_info>(first_gid, info));
    }

    const auto& layers = _map->getLayers();
    for(const auto& layer : layers) {
        switch(layer->getType()) {
            case tmx::Layer::Type::Object:
                _parse_object_layer(dynamic_cast<tmx::ObjectGroup*>(layer.get()));
                break;
            case tmx::Layer::Type::Tile:
                _parse_tile_layer(dynamic_cast<const tmx::TileLayer*>(layer.get()));
                break;
            case tmx::Layer::Type::Image:
            default:
                throw not_implemented();
                break;
        }
    }
    return _self;
}
