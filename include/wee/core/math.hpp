#pragma once

#include <cmath>
#include <cstdint>
#include <string.h> // < memcpy

namespace wee {

    struct math {
        constexpr static int log2(int n) {
            return ( (n<2) ? 0 : 1 + log2(n>>1));
        }

        template <typename T>
        static T npot(const T& n) {
            return 1 << (log2(n - 1) + 1);
        }

        float rsqrt(float number) {
            uint32_t i;
            float x2, y;
            const float threehalfs = 1.5F;

            x2 = number * 0.5F;
            y  = number;
            //i  = * ( long * ) &y;                     // evil floating point bit level hacking
            memcpy(&i, &y, sizeof(uint32_t));
            //i  = 0x5f3759df - ( i >> 1 );               // what the fuck?  
            i = 0x5F375A86 - (i >> 1); 
            //y  = * ( float * ) &i;
            memcpy(&y, &i, sizeof(float));
            y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
            //  y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed
            return y;
        }
    };
}
