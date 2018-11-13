#pragma once

#include <core/singleton.hpp>
#include <random>

namespace wee {
    class random : public singleton<random> {
        std::random_device _r;
        std::mt19937 _gen;
    public:
        random() { 
            //std::random_device rd;  //Will be used to obtain a seed for the random number engine
                std::mt19937 gen(_r()); //Standard mersenne_twister_engine seeded with rd()
            //_gen(_r());
        }

        template <typename T>
        T next_real(T a, T b) {
            std::uniform_real_distribution<T> distr(a, b);
            return distr(_gen);
        }

        template <typename T>
        T next_int(T a, T b) {
            std::uniform_int_distribution<T> distr(a, b);
            return distr(_gen);
        }

        template <typename T>
        T next_normal(T u, T o) {
            std::normal_distribution<T> d{u, o};
            return d(_gen);
        }
    };

    template <typename T>
    T randi(T a, T b) {
        return random::instance().next_int(a, b);
    }

    float randf(float a = 0.f, float b = 1.0f); 
}
