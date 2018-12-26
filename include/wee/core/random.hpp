#pragma once

#include <core/singleton.hpp>
#include <random>
#include <type_traits>
#include <chrono>

namespace wee {

    struct random {
        int32_t seed;
        std::mt19937 eng;

        random(int32_t seed) : seed(seed) {
            eng = std::mt19937(seed);//{ seed };//std::random_device{}() };
        }


        template <typename Int>
        Int next_int(Int min, Int max) {
            static_assert(std::is_integral<Int>::value, "an integral value is required");
            using param_type = typename std::uniform_int_distribution<Int>::param_type;
            std::uniform_int_distribution<Int> dist;
            return dist(eng, param_type{min, max});
        }

        template <typename Real>
        Real next(Real min, Real max) {
            static_assert(std::is_floating_point<Real>::value, "a floating point value is required");

            using param_type = typename std::uniform_real_distribution<Real>::param_type;
            std::uniform_real_distribution<Real> dist;
            return dist(eng, param_type{min, max});
        }
    };

    auto randf = [] (float min = 0.0f, float max = 1.0f, int32_t s = 0) -> float {
        using wee::random;
        static auto milliseconds_since_epoch =
            std::chrono::system_clock::now().time_since_epoch() / 
            std::chrono::milliseconds(1);

        int32_t seed = s ? s : milliseconds_since_epoch;
        static random rnd(seed);
        return rnd.next(min, max);
    };
}
