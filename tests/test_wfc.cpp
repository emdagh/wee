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

    typedef int type;

    type in_map[] = { // red maze example 
#if 0
        201, 201, 201, 201,
        201, 202, 203, 202,
        201, 204, 666, 204,
        201, 202, 203, 202
#else   
        
        666,666,666,666,
        666,202,202,666,
        202,205,205,202,
        205,205,205,205,
#endif  
    };
    constexpr size_t kOutputDimension = 4;
    constexpr size_t kOutputSize = kOutputDimension * kOutputDimension;

    type* out_map = new type[kOutputSize];
    
    _wfc::_run<type>(in_map, { 4, 4 }, out_map, { kOutputDimension, kOutputDimension });

    std::vector<type> arr(kOutputSize, -1);;
    std::copy(out_map, out_map + kOutputSize, std::begin(arr));
    for(auto y: range(kOutputDimension)) {
        for(auto x: range(kOutputDimension)) {
            switch(out_map[x + y * kOutputDimension]) {
                case 201: std::cout << " "; break;
                case 202: std::cout << YELLOW << "." << RESET; break;
                case 203: std::cout << "-"; break;
                case 204: std::cout << "|"; break;
                case 205: std::cout << BLUE  << "~" << RESET; break;
                case 666: std::cout << GREEN << "*" << RESET; break;
            }
            //std::cout << std::dec <<  out_map[x + y * kOutputDimension] << ",";
        }
        std::cout << std::endl;
    }

    return 0;
}
