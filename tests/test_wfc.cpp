#include "wfc.hpp"
#include <tmxlite/Map.hpp>
#include <engine/assets.hpp>

using wee::range;

struct tilelayer {
};

// represents a tmx file in memory
struct tilemap {
};

// represents a tsx file in memory
struct tileset { 
};

int main(int, char**) {

    tmx::Map map;
    map.load(wee::get_resource_path("assets") + "wfc.tmx");

    int in_map[] = { // red maze example 
        201, 201, 201, 201,
        201, 202, 203, 202,
        201, 204, 666, 204,
        201, 202, 203, 202
    };
    constexpr size_t kOutputDimension = 8;
    constexpr size_t kOutputSize = kOutputDimension * kOutputDimension;

    int* out_map = new int[kOutputSize];
    wfc(in_map, { 4, 4 }, out_map, { kOutputDimension, kOutputDimension });

    std::vector<int> arr(kOutputSize, -1);;
    std::copy(out_map, out_map + kOutputSize, std::begin(arr));
    for(auto y: range(kOutputDimension)) {
        for(auto x: range(kOutputDimension)) {
            switch(out_map[x + y * kOutputDimension]) {
                case 201: std::cout << " "; break;
                case 202: std::cout << "+"; break;
                case 203: std::cout << "-"; break;
                case 204: std::cout << "|"; break;
                case 666: std::cout << "*"; break;
            }
            //std::cout << std::dec <<  out_map[x + y * kOutputDimension] << ",";
        }
        std::cout << std::endl;
    }

    return 0;
}
