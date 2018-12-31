#pragma once

#include <core/singleton.hpp>
#include <random>
#include <type_traits>
#include <chrono>

namespace wee {

    struct random {
        uint32_t _seed;
        std::mt19937 eng;

        random(uint32_t s=0) {
            static auto milliseconds_since_epoch =
            std::chrono::system_clock::now().time_since_epoch() / 
            std::chrono::milliseconds(1);

            _seed = s ? s : milliseconds_since_epoch;
            eng = std::mt19937(_seed);//{ seed };//std::random_device{}() };
        }

        uint32_t seed() const { return _seed; }

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

        uint32_t seed = s ? s : milliseconds_since_epoch;
        static random rnd(seed);
        return rnd.next(min, max);
    };

    template <typename T, typename std::enable_if<std::is_integral<T>::value, T>::type* = nullptr>
    T randgen(T min, T max) 
    {
        static random rnd;
        return rnd.next_int<T>(min, max);
    }

    template <typename T, typename std::enable_if<std::is_floating_point<T>::value, T>::type* = nullptr>
    T randgen(T min, T max) 
    {
        static random rnd;
        return rnd.next<T>(min, max);
    };
}
