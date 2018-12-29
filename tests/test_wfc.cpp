#include "wfc.hpp"
#include <tmxlite/Map.hpp>
#include <tmxlite/Layer.hpp>
#include <tmxlite/TileLayer.hpp>
#include <engine/assets.hpp>

using wee::range;

typedef tensor<int32_t, 2> vec2i;

void load_tile_layer(const tmx::Map& mp, const tmx::TileLayer* layer) {
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
    
    const auto& tiles = layer->getTiles();

    size_t len = mapdim.x * mapdim.y;
    for(size_t i=0; i < len; i++) {
        if(tiles[i].ID == 0)
            continue;
        unsigned int gid = tiles[i].ID;
        /**
         * here we should acquire the correct tileset for this GID and subtract it's
         * first GID from the one as observed in the tile. This is to `normalize`
         * the tile GID before delinearizing into texture space.
         */
        const tmx::Tileset* tileset = tileset_for_gid(mp, gid);;
        const auto* tile = tileset->getTile(gid);

        gid -= tileset->getFirstGID();

        DEBUG_VALUE_OF(tile->ID);

        for(const auto& object: tile->objectGroup.getObjects()) {

            for(const auto& property: object.getProperties()) {
                DEBUG_VALUE_OF(property.getStringValue());
            }
        }
        
        [[maybe_unused]] int k = 0;
    }
}

void load_tmx() {
    {
        tmx::Map map;
        if(map.load(wee::get_resource_path("assets/levels") + "example.tmx")) {
            for(const auto& tileset: map.getTilesets()) {
                //int firstgid = tileset.getFirstGID();
                for(const auto& tileset_property : tileset.getProperties()) {
                    DEBUG_VALUE_OF(tileset_property.getStringValue());
                }
            }
            for(const auto& layer: map.getLayers()) {
                DEBUG_VALUE_OF(layer->getName());
                switch(layer->getType()) {
                case tmx::Layer::Type::Tile:
                    load_tile_layer(map, reinterpret_cast<tmx::TileLayer*>(layer.get()));
                    break;
                default:
                    throw not_implemented();
                }
            }
        }

    }
}


int main(int, char**) {



    typedef int type;

    type in_map[] = { 
        111, 111, 111, 111,
        111, 111, 111, 111,
        111, 211, 211, 111,
        211, 311, 311, 211,
        311, 311, 311, 311,
        311, 311, 311, 311
    };
    constexpr int2 kOutputDimension = { 64, 16};
    constexpr size_t kOutputSize = kOutputDimension.x * kOutputDimension.y;

    type* out_map = new type[kOutputSize];

    auto t_start = std::chrono::high_resolution_clock::now();
    _wfc::_run<type>(in_map, { 4, 6 }, out_map, kOutputDimension);
    auto t_end = std::chrono::high_resolution_clock::now();

    auto time_passed = std::chrono::duration<double, std::milli>(t_end-t_start).count();

    DEBUG_VALUE_OF(time_passed);

#if 1 
    std::vector<type> arr(kOutputSize, -1);;
    std::copy(out_map, out_map + kOutputSize, std::begin(arr));
    for(auto y: range(kOutputDimension.y)) {
        for(auto x: range(kOutputDimension.x)) {
            switch(out_map[x + y * kOutputDimension.x]) {
                case 201: std::cout << " "; break;
                case 202: std::cout << "+"; break;
                case 203: std::cout << "-"; break;
                case 204: std::cout << "|"; break;
                case 666: std::cout << RED << "*" << RESET; break;
                case 311: std::cout << BLUE  << "~" << RESET; break;
                case 111: std::cout << GREEN << "#" << RESET; break;
                case 211: std::cout << YELLOW << "." << RESET; break;
                default: std::cout << RED << "?" << RESET; break;
            }
            //std::cout << std::dec <<  out_map[x + y * kOutputDimension] << ",";
        }
        std::cout << std::endl;
    }
#endif
    load_tmx();
    return 0;
}
