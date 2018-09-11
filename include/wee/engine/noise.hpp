#pragma once

namespace wee {
    class noise_generator {
        public:
            virtual void reseed(int32_t) const = 0;
            virtual float noise(float, float, float) const = 0;
    };

    template <typename T = float,typename R = std::default_random_engine>
        class simplex : public noise_generator {
            typedef T value_type;
            typedef R engine_type;

            int32_t p[512] = { 0 };
            private:
            T _fade(T t) const { 
                return t * t * t * (t * (t * 6 - 15) + 10);
            }
            T _lerp(T t, T a, T b) const {
                return a + t * (b - a);
            }
            T _grad(int32_t hash, T x, T y, T z) const {
                const int32_t h = hash & 15; // mod 16
                const T u = h < 8 ? x : y;
                const T v = h < 4 ? y : h == 12 || h == 14 ? x : z;
                return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
            }
            public:
            simplex(int32_t seed = engine_type::default_seed) {
                reseed(seed);
            }
            virtual float noise(float x, float y, float z) const {
                const int32_t ix = static_cast<int32_t>(std::floor(x)) & 255;
                const int32_t iy = static_cast<int32_t>(std::floor(y)) & 255;
                const int32_t iz = static_cast<int32_t>(std::floor(z)) & 255;

                x -= std::floor(x);
                y -= std::floor(y);
                z -= std::floor(z);

                const float u = _fade(x);
                const float v = _fade(y);
                const float w = _fade(z);

                const int32_t 
                    A  = p[ix   ] + iy,
                       AA = p[A    ] + iz,
                       AB = p[A + 1] + iz,
                       B  = p[ix+ 1] + iy,
                       BA = p[B    ] + iz,
                       BB = p[B + 1] + iz;

                return _lerp(w, _lerp(v, _lerp(u, _grad(p[AA], x, y, z),
                                _grad(p[BA], x - 1, y, z)),
                            _lerp(u, _grad(p[AB], x, y - 1, z),
                                _grad(p[BB], x - 1, y - 1, z))),
                        _lerp(v, _lerp(u, _grad(p[AA + 1], x, y, z - 1),
                                _grad(p[BA + 1], x - 1, y, z - 1)),
                            _lerp(u, _grad(p[AB + 1], x, y - 1, z - 1), 
                                _grad(p[BB + 1], x - 1, y - 1, z - 1))));


            }
            virtual void reseed(int32_t seed) {
                for(size_t i=0; i < 256; i++) {
                    p[i] = i;
                }
                std::shuffle(std::begin(p), std::begin(p) + 256, engine_type(seed));
                for(size_t i=0; i < 256; i++) {
                    p[i + 256] = p[i];
                }
            }

            virtual float octave(float x, float y, float z, int32_t n) const {
                float r = 0.0f;
                float a = 1.0f;

                for(int32_t i=0; i < n; i++) {
                    r += noise(x, y, z) * a;
                    x *= 2.0f;
                    y *= 2.0f;
                    z *= 2.0f;
                    a *= 0.5f;
                }
                return r;
            }

            float normalize(float i) {
                return i * 0.5f + 0.5f;
            }
        };
}
