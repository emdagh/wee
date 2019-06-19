#pragma once

namespace wee {


    enum struct texture_filter : uint8_t {
        kLinear,                     //  Use linear filtering.
        kPoint,                        //  Use point filtering.
        kAnisotropic,                  //  Use anisotropic filtering.
        kLinearMipPoint,               //  Use linear filtering to shrink or expand, and point filtering between mipmap levels (mip).
        kPointMipLinear,               //  Use point filtering to shrink (minify) or expand (magnify), and linear filtering between mipmap levels.
        kMinLinearMagPointMipLinear,   //  Use linear filtering to shrink, point filtering to expand, and linear filtering between mipmap levels.
        kMinLinearMagPointMipPoint,    //  Use linear filtering to shrink, point filtering to expand, and point filtering between mipmap levels.
        kMinPointMagLinearMipLinear,   //  Use point filtering to shrink, linear filtering to expand, and linear filtering between mipmap levels.
        kMinPointMagLinearMipPoint     //  Use point filtering to shrink, linear filtering to expand, and point filtering between mipmap levels.
    };

}
