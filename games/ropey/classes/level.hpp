#pragma once

#include <map>
#include <SDL.h>
#include <wee/wee.hpp>
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <core/vec2.hpp>
#include <classes/common.hpp>

class b2World;

struct tileset_info {
    SDL_Texture* texture;
    int margin;
    int spacing;
    int tile_width;
    int tile_height;
};

class level {
    tmx::Map* _map;
    b2World* _world;
    std::map<int, tileset_info> _tilesets;
    int _w, _h;
    int _tw, _th;
    wee::vec2f _offset = wee::vec2{0.f, 0.f};
    entity_type _self;
protected:
    void _parse_tileset(tmx::Tileset&);
    void _parse_layer(tmx::Layer::Ptr);
    void _parse_tile_layer(const tmx::TileLayer*);
    void _parse_object_layer(const tmx::ObjectGroup*);
    void _parse_image_layer(tmx::Layer::Ptr);
public:
    level& source(tmx::Map*);
    level& world(b2World*);
    level& offset(const wee::vec2f&);
    entity_type build();
};
