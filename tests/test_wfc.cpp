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
        201, 202, 203, 202,
        201, 201, 201, 201
#else   
        111, 111, 111, 111,
        111, 111, 111, 111,
        111, 211, 211, 111,
        211, 311, 311, 211,
        311, 311, 311, 311,
        311, 311, 311, 311
#endif  
    };
    constexpr int2 kOutputDimension = { 50, 16};
    constexpr size_t kOutputSize = kOutputDimension.x * kOutputDimension.y;

    type* out_map = new type[kOutputSize];
    
    _wfc::_run<type>(in_map, { 4, 6 }, out_map, kOutputDimension);

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

    return 0;
}
