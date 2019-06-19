#include <core/ndarray.hpp>
#include <core/range.hpp>
#include <tuple>
#include <vector>
/* 
 * v0.1: a 1:1 port of the original C# algorithm.
 */


namespace wee {


        /**
         * nd + Nd * (nd-1 + Nd-1 * (nd-2 + Nd-2 * (nd-3 ... )))
         * see also: https://eli.thegreenplace.net/2015/memory-layout-of-multi-dimensional-arrays/
         */
	struct model{
        virtual bool progagate() const = 0;
        virtual bool on_boundary(int, int) const = 0;
    };

    struct basic_model : model {
        typedef double real;
        template <typename T, size_t D0 = 1>
        struct array {
            typedef std::vector<T> type;
            typedef std::array<size_t, D0> index_type;
        };
        size_t              _dim[2]; 
        size_t              _num_patterns;
        bool                _periodic_out;
        size_t              _foundation = -1;
        array<real>::type   _pattern_weight;
    };

    struct overlapping_model : public basic_model {
    };

    struct basic_tiled_model : public basic_model {
        array<bool, 3> _propagator;
        array<color, 2> _tiles;
        size_t _tile_size;

        virtual bool propagate() const;
        virtual bool on_boundary(int, int) const { return false; }
    };

    template <typename T>
    T::value_type _sum(const T& a) {
        return std::accumulate(std::begin(a), std::end(a), T::value_type());
    }

    template <typename T>
    size_t random_index(const std::vector<T>& a, const T& val) {
        auto sum = _sum(a);
        if(sum == 0.0) {
            return std::floor(val * a.size());
        }

        auto zero_to_sum = val * sum;
        T acc = T();
        for(auto i : range(0, a.size())) {
            acc += a[i];
            if(zero_to_sum <= acc) 
                return i;
        }
    }
    return 0;
}
