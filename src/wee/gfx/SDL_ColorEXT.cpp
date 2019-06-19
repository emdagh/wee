#include <gfx/SDL_ColorEXT.hpp>
#include <SDL.h>

void SDL_CreateColorRGB(uint8_t r, uint8_t g, uint8_t b, SDL_Color* ptr) {
	ptr->r = r;
	ptr->g = g;
	ptr->b = b;
	ptr->a = 255;
}

void SDL_CreateColorHSV(uint8_t h, uint8_t s, uint8_t v, SDL_Color* ptr) {
    if(s == 0)
        return SDL_CreateColorRGB(v, v, v, ptr);
    uint8_t region  = h / 43;
    uint8_t fpart   = (h - (region * 43)) * 6;
    uint8_t p       = (v * (255 - s)) >> 8;
    uint8_t q       = (v * (255 - ((s * fpart) >> 8))) >> 8;
    uint8_t t       = (v * (255 - ((s * (255 - fpart)) >> 8))) >> 8;
    
    switch(region) {
        case 0: return  SDL_CreateColorRGB(v, t, p, ptr);
        case 1: return  SDL_CreateColorRGB(q, v, p, ptr);
        case 2: return  SDL_CreateColorRGB(p, v, t, ptr);
        case 3: return  SDL_CreateColorRGB(p, q, v, ptr);
        case 4: return  SDL_CreateColorRGB(t, p, v, ptr);
        default: return SDL_CreateColorRGB(v, p, q, ptr);
    }
    return SDL_CreateColorRGB(0, 0, 0, ptr);
}

C_API void SDL_CreateColorEXT(uint32_t i, struct SDL_Color* res) {
    res->a = 0xff;//(i & 0xff000000) >> 24;
    res->r = (i & 0x00ff0000) >> 16;
    res->g = (i & 0x0000ff00) >> 8;
    res->b = (i & 0x000000ff) >> 0;
}

SDL_Color SDL_CreateColorEXT_(uint32_t i) {
    SDL_Color ref;
    SDL_CreateColorEXT(i, &ref);
    return ref;
}

const SDL_Color SDL_ColorPresetEXT::AliceBlue       =SDL_CreateColorEXT_(0xF0F8FF);
const SDL_Color SDL_ColorPresetEXT::AntiqueWhite    =SDL_CreateColorEXT_(0xFAEBD7);
const SDL_Color SDL_ColorPresetEXT::Aqua            =SDL_CreateColorEXT_(0x00FFFF);
const SDL_Color SDL_ColorPresetEXT::Aquamarine      =SDL_CreateColorEXT_(0x7FFFD4);
const SDL_Color SDL_ColorPresetEXT::Azure           =SDL_CreateColorEXT_(0xF0FFFF);
const SDL_Color SDL_ColorPresetEXT::Beige           =SDL_CreateColorEXT_(0xF5F5DC);
const SDL_Color SDL_ColorPresetEXT::Bisque          =SDL_CreateColorEXT_(0xFFE4C4);
const SDL_Color SDL_ColorPresetEXT::Black           =SDL_CreateColorEXT_(0x000000);
const SDL_Color SDL_ColorPresetEXT::BlanchedAlmond  =SDL_CreateColorEXT_(0xFFEBCD);
const SDL_Color SDL_ColorPresetEXT::Blue            =SDL_CreateColorEXT_(0x0000FF);
const SDL_Color SDL_ColorPresetEXT::BlueViolet      =SDL_CreateColorEXT_(0x8A2BE2);
const SDL_Color SDL_ColorPresetEXT::Brown           =SDL_CreateColorEXT_(0xA52A2A);
const SDL_Color SDL_ColorPresetEXT::BurlyWood       =SDL_CreateColorEXT_(0xDEB887);
const SDL_Color SDL_ColorPresetEXT::CadetBlue       =SDL_CreateColorEXT_(0x5F9EA0);
const SDL_Color SDL_ColorPresetEXT::Chartreuse      =SDL_CreateColorEXT_(0x7FFF00);
const SDL_Color SDL_ColorPresetEXT::Chocolate       =SDL_CreateColorEXT_(0xD2691E);
const SDL_Color SDL_ColorPresetEXT::Coral           =SDL_CreateColorEXT_(0xFF7F50);
const SDL_Color SDL_ColorPresetEXT::CornflowerBlue  =SDL_CreateColorEXT_(0x6495ED);
const SDL_Color SDL_ColorPresetEXT::Cornsilk        =SDL_CreateColorEXT_(0xFFF8DC);
const SDL_Color SDL_ColorPresetEXT::Crimson         =SDL_CreateColorEXT_(0xDC143C);
const SDL_Color SDL_ColorPresetEXT::Cyan            =SDL_CreateColorEXT_(0x00FFFF);
const SDL_Color SDL_ColorPresetEXT::DarkBlue        =SDL_CreateColorEXT_(0x00008B);
const SDL_Color SDL_ColorPresetEXT::DarkCyan        =SDL_CreateColorEXT_(0x008B8B);
const SDL_Color SDL_ColorPresetEXT::DarkGoldenRod   =SDL_CreateColorEXT_(0xB8860B);
const SDL_Color SDL_ColorPresetEXT::DarkGray        =SDL_CreateColorEXT_(0xA9A9A9);
const SDL_Color SDL_ColorPresetEXT::DarkGrey        =SDL_CreateColorEXT_(0xA9A9A9);
const SDL_Color SDL_ColorPresetEXT::DarkGreen       =SDL_CreateColorEXT_(0x006400);
const SDL_Color SDL_ColorPresetEXT::DarkKhaki       =SDL_CreateColorEXT_(0xBDB76B);
const SDL_Color SDL_ColorPresetEXT::DarkMagenta     =SDL_CreateColorEXT_(0x8B008B);
const SDL_Color SDL_ColorPresetEXT::DarkOliveGreen  =SDL_CreateColorEXT_(0x556B2F);
const SDL_Color SDL_ColorPresetEXT::DarkOrange      =SDL_CreateColorEXT_(0xFF8C00);
const SDL_Color SDL_ColorPresetEXT::DarkOrchid      =SDL_CreateColorEXT_(0x9932CC);
const SDL_Color SDL_ColorPresetEXT::DarkRed         =SDL_CreateColorEXT_(0x8B0000);
const SDL_Color SDL_ColorPresetEXT::DarkSalmon      =SDL_CreateColorEXT_(0xE9967A);
const SDL_Color SDL_ColorPresetEXT::DarkSeaGreen    =SDL_CreateColorEXT_(0x8FBC8F);
const SDL_Color SDL_ColorPresetEXT::DarkSlateBlue   =SDL_CreateColorEXT_(0x483D8B);
const SDL_Color SDL_ColorPresetEXT::DarkSlateGray   =SDL_CreateColorEXT_(0x2F4F4F);
const SDL_Color SDL_ColorPresetEXT::DarkSlateGrey   =SDL_CreateColorEXT_(0x2F4F4F);
const SDL_Color SDL_ColorPresetEXT::DarkTurquoise   =SDL_CreateColorEXT_(0x00CED1);
const SDL_Color SDL_ColorPresetEXT::DarkViolet      =SDL_CreateColorEXT_(0x9400D3);
const SDL_Color SDL_ColorPresetEXT::DeepPink        =SDL_CreateColorEXT_(0xFF1493);
const SDL_Color SDL_ColorPresetEXT::DeepSkyBlue     =SDL_CreateColorEXT_(0x00BFFF);
const SDL_Color SDL_ColorPresetEXT::DimGray         =SDL_CreateColorEXT_(0x696969);
const SDL_Color SDL_ColorPresetEXT::DimGrey         =SDL_CreateColorEXT_(0x696969);
const SDL_Color SDL_ColorPresetEXT::DodgerBlue      =SDL_CreateColorEXT_(0x1E90FF);
const SDL_Color SDL_ColorPresetEXT::FireBrick       =SDL_CreateColorEXT_(0xB22222);
const SDL_Color SDL_ColorPresetEXT::FloralWhite     =SDL_CreateColorEXT_(0xFFFAF0);
const SDL_Color SDL_ColorPresetEXT::ForestGreen     =SDL_CreateColorEXT_(0x228B22);
const SDL_Color SDL_ColorPresetEXT::Fuchsia         =SDL_CreateColorEXT_(0xFF00FF);
const SDL_Color SDL_ColorPresetEXT::Gainsboro       =SDL_CreateColorEXT_(0xDCDCDC);
const SDL_Color SDL_ColorPresetEXT::GhostWhite      =SDL_CreateColorEXT_(0xF8F8FF);
const SDL_Color SDL_ColorPresetEXT::Gold            =SDL_CreateColorEXT_(0xFFD700);
const SDL_Color SDL_ColorPresetEXT::GoldenRod       =SDL_CreateColorEXT_(0xDAA520);
const SDL_Color SDL_ColorPresetEXT::Gray            =SDL_CreateColorEXT_(0x808080);
const SDL_Color SDL_ColorPresetEXT::Grey            =SDL_CreateColorEXT_(0x808080);
const SDL_Color SDL_ColorPresetEXT::Green           =SDL_CreateColorEXT_(0x008000);
const SDL_Color SDL_ColorPresetEXT::GreenYellow     =SDL_CreateColorEXT_(0xADFF2F);
const SDL_Color SDL_ColorPresetEXT::HoneyDew        =SDL_CreateColorEXT_(0xF0FFF0);
const SDL_Color SDL_ColorPresetEXT::HotPink         =SDL_CreateColorEXT_(0xFF69B4);
const SDL_Color SDL_ColorPresetEXT::IndianRed       =SDL_CreateColorEXT_(0xCD5C5C);
const SDL_Color SDL_ColorPresetEXT::Indigo          =SDL_CreateColorEXT_(0x4B0082);
const SDL_Color SDL_ColorPresetEXT::Ivory           =SDL_CreateColorEXT_(0xFFFFF0);
const SDL_Color SDL_ColorPresetEXT::Khaki           =SDL_CreateColorEXT_(0xF0E68C);
const SDL_Color SDL_ColorPresetEXT::Lavender        =SDL_CreateColorEXT_(0xE6E6FA);
const SDL_Color SDL_ColorPresetEXT::LavenderBlush   =SDL_CreateColorEXT_(0xFFF0F5);
const SDL_Color SDL_ColorPresetEXT::LawnGreen       =SDL_CreateColorEXT_(0x7CFC00);
const SDL_Color SDL_ColorPresetEXT::LemonChiffon    =SDL_CreateColorEXT_(0xFFFACD);
const SDL_Color SDL_ColorPresetEXT::LightBlue       =SDL_CreateColorEXT_(0xADD8E6);
const SDL_Color SDL_ColorPresetEXT::LightCoral      =SDL_CreateColorEXT_(0xF08080);
const SDL_Color SDL_ColorPresetEXT::LightCyan       =SDL_CreateColorEXT_(0xE0FFFF);
const SDL_Color SDL_ColorPresetEXT::LightGoldenRodYellow    =SDL_CreateColorEXT_(0xFAFAD2);
const SDL_Color SDL_ColorPresetEXT::LightGray         =SDL_CreateColorEXT_(0xD3D3D3);
const SDL_Color SDL_ColorPresetEXT::LightGrey         =SDL_CreateColorEXT_(0xD3D3D3);
const SDL_Color SDL_ColorPresetEXT::LightGreen        =SDL_CreateColorEXT_(0x90EE90);
const SDL_Color SDL_ColorPresetEXT::LightPink         =SDL_CreateColorEXT_(0xFFB6C1);
const SDL_Color SDL_ColorPresetEXT::LightSalmon       =SDL_CreateColorEXT_(0xFFA07A);
const SDL_Color SDL_ColorPresetEXT::LightSeaGreen     =SDL_CreateColorEXT_(0x20B2AA);
const SDL_Color SDL_ColorPresetEXT::LightSkyBlue      =SDL_CreateColorEXT_(0x87CEFA);
const SDL_Color SDL_ColorPresetEXT::LightSlateGray    =SDL_CreateColorEXT_(0x778899);
const SDL_Color SDL_ColorPresetEXT::LightSlateGrey    =SDL_CreateColorEXT_(0x778899);
const SDL_Color SDL_ColorPresetEXT::LightSteelBlue    =SDL_CreateColorEXT_(0xB0C4DE);
const SDL_Color SDL_ColorPresetEXT::LightYellow       =SDL_CreateColorEXT_(0xFFFFE0);
const SDL_Color SDL_ColorPresetEXT::Lime              =SDL_CreateColorEXT_(0x00FF00);
const SDL_Color SDL_ColorPresetEXT::LimeGreen         =SDL_CreateColorEXT_(0x32CD32);
const SDL_Color SDL_ColorPresetEXT::Linen             =SDL_CreateColorEXT_(0xFAF0E6);
const SDL_Color SDL_ColorPresetEXT::Magenta           =SDL_CreateColorEXT_(0xFF00FF);
const SDL_Color SDL_ColorPresetEXT::Maroon            =SDL_CreateColorEXT_(0x800000);
const SDL_Color SDL_ColorPresetEXT::MediumAquaMarine  =SDL_CreateColorEXT_(0x66CDAA);
const SDL_Color SDL_ColorPresetEXT::MediumBlue        =SDL_CreateColorEXT_(0x0000CD);
const SDL_Color SDL_ColorPresetEXT::MediumOrchid      =SDL_CreateColorEXT_(0xBA55D3);
const SDL_Color SDL_ColorPresetEXT::MediumPurple      =SDL_CreateColorEXT_(0x9370DB);
const SDL_Color SDL_ColorPresetEXT::MediumSeaGreen    =SDL_CreateColorEXT_(0x3CB371);
const SDL_Color SDL_ColorPresetEXT::MediumSlateBlue   =SDL_CreateColorEXT_(0x7B68EE);
const SDL_Color SDL_ColorPresetEXT::MediumSpringGreen =SDL_CreateColorEXT_(0x00FA9A);
const SDL_Color SDL_ColorPresetEXT::MediumTurquoise   =SDL_CreateColorEXT_(0x48D1CC);
const SDL_Color SDL_ColorPresetEXT::MediumVioletRed   =SDL_CreateColorEXT_(0xC71585);
const SDL_Color SDL_ColorPresetEXT::MidnightBlue      =SDL_CreateColorEXT_(0x191970);
const SDL_Color SDL_ColorPresetEXT::MintCream         =SDL_CreateColorEXT_(0xF5FFFA);
const SDL_Color SDL_ColorPresetEXT::MistyRose         =SDL_CreateColorEXT_(0xFFE4E1);
const SDL_Color SDL_ColorPresetEXT::Moccasin          =SDL_CreateColorEXT_(0xFFE4B5);
const SDL_Color SDL_ColorPresetEXT::NavajoWhite       =SDL_CreateColorEXT_(0xFFDEAD);
const SDL_Color SDL_ColorPresetEXT::Navy              =SDL_CreateColorEXT_(0x000080);
const SDL_Color SDL_ColorPresetEXT::OldLace           =SDL_CreateColorEXT_(0xFDF5E6);
const SDL_Color SDL_ColorPresetEXT::Olive             =SDL_CreateColorEXT_(0x808000);
const SDL_Color SDL_ColorPresetEXT::OliveDrab         =SDL_CreateColorEXT_(0x6B8E23);
const SDL_Color SDL_ColorPresetEXT::Orange            =SDL_CreateColorEXT_(0xFFA500);
const SDL_Color SDL_ColorPresetEXT::OrangeRed         =SDL_CreateColorEXT_(0xFF4500);
const SDL_Color SDL_ColorPresetEXT::Orchid            =SDL_CreateColorEXT_(0xDA70D6);
const SDL_Color SDL_ColorPresetEXT::PaleGoldenRod     =SDL_CreateColorEXT_(0xEEE8AA);
const SDL_Color SDL_ColorPresetEXT::PaleGreen         =SDL_CreateColorEXT_(0x98FB98);
const SDL_Color SDL_ColorPresetEXT::PaleTurquoise     =SDL_CreateColorEXT_(0xAFEEEE);
const SDL_Color SDL_ColorPresetEXT::PaleVioletRed     =SDL_CreateColorEXT_(0xDB7093);
const SDL_Color SDL_ColorPresetEXT::PapayaWhip        =SDL_CreateColorEXT_(0xFFEFD5);
const SDL_Color SDL_ColorPresetEXT::PeachPuff         =SDL_CreateColorEXT_(0xFFDAB9);
const SDL_Color SDL_ColorPresetEXT::Peru              =SDL_CreateColorEXT_(0xCD853F);
const SDL_Color SDL_ColorPresetEXT::Pink              =SDL_CreateColorEXT_(0xFFC0CB);
const SDL_Color SDL_ColorPresetEXT::Plum              =SDL_CreateColorEXT_(0xDDA0DD);
const SDL_Color SDL_ColorPresetEXT::PowderBlue        =SDL_CreateColorEXT_(0xB0E0E6);
const SDL_Color SDL_ColorPresetEXT::Purple            =SDL_CreateColorEXT_(0x800080);
const SDL_Color SDL_ColorPresetEXT::RebeccaPurple     =SDL_CreateColorEXT_(0x663399);
const SDL_Color SDL_ColorPresetEXT::Red               =SDL_CreateColorEXT_(0xFF0000);
const SDL_Color SDL_ColorPresetEXT::RosyBrown         =SDL_CreateColorEXT_(0xBC8F8F);
const SDL_Color SDL_ColorPresetEXT::RoyalBlue         =SDL_CreateColorEXT_(0x4169E1);
const SDL_Color SDL_ColorPresetEXT::SaddleBrown       =SDL_CreateColorEXT_(0x8B4513);
const SDL_Color SDL_ColorPresetEXT::Salmon            =SDL_CreateColorEXT_(0xFA8072);
const SDL_Color SDL_ColorPresetEXT::SandyBrown        =SDL_CreateColorEXT_(0xF4A460);
const SDL_Color SDL_ColorPresetEXT::SeaGreen          =SDL_CreateColorEXT_(0x2E8B57);
const SDL_Color SDL_ColorPresetEXT::SeaShell          =SDL_CreateColorEXT_(0xFFF5EE);
const SDL_Color SDL_ColorPresetEXT::Sienna            =SDL_CreateColorEXT_(0xA0522D);
const SDL_Color SDL_ColorPresetEXT::Silver            =SDL_CreateColorEXT_(0xC0C0C0);
const SDL_Color SDL_ColorPresetEXT::SkyBlue           =SDL_CreateColorEXT_(0x87CEEB);
const SDL_Color SDL_ColorPresetEXT::SlateBlue         =SDL_CreateColorEXT_(0x6A5ACD);
const SDL_Color SDL_ColorPresetEXT::SlateGray         =SDL_CreateColorEXT_(0x708090);
const SDL_Color SDL_ColorPresetEXT::SlateGrey         =SDL_CreateColorEXT_(0x708090);
const SDL_Color SDL_ColorPresetEXT::Snow              =SDL_CreateColorEXT_(0xFFFAFA);
const SDL_Color SDL_ColorPresetEXT::SpringGreen       =SDL_CreateColorEXT_(0x00FF7F);
const SDL_Color SDL_ColorPresetEXT::SteelBlue         =SDL_CreateColorEXT_(0x4682B4);
const SDL_Color SDL_ColorPresetEXT::Tan               =SDL_CreateColorEXT_(0xD2B48C);
const SDL_Color SDL_ColorPresetEXT::Teal              =SDL_CreateColorEXT_(0x008080);
const SDL_Color SDL_ColorPresetEXT::Thistle           =SDL_CreateColorEXT_(0xD8BFD8);
const SDL_Color SDL_ColorPresetEXT::Tomato            =SDL_CreateColorEXT_(0xFF6347);
const SDL_Color SDL_ColorPresetEXT::Turquoise         =SDL_CreateColorEXT_(0x40E0D0);
const SDL_Color SDL_ColorPresetEXT::Violet            =SDL_CreateColorEXT_(0xEE82EE);
const SDL_Color SDL_ColorPresetEXT::Wheat             =SDL_CreateColorEXT_(0xF5DEB3);
const SDL_Color SDL_ColorPresetEXT::White             =SDL_CreateColorEXT_(0xFFFFFF);
const SDL_Color SDL_ColorPresetEXT::WhiteSmoke        =SDL_CreateColorEXT_(0xF5F5F5);
const SDL_Color SDL_ColorPresetEXT::Yellow            =SDL_CreateColorEXT_(0xFFFF00);
const SDL_Color SDL_ColorPresetEXT::YellowGreen       =SDL_CreateColorEXT_(0x9ACD32);
