#pragma once

#include <array>

namespace {

    template <typename T, size_t N>
    class ndview {
        typedef std::array<ptrdiff_t, N> shape_t;
        T _data;
        shape_t _shape;
        shape_t _strides;
    protected:
        auto compute_strides() {
            ptrdiff_t dsize = 1;
            static_assert(N > 0);
            for(auto j : range(N)) {
                auto i = j;//N - j - 1;
                _strides[i] = _shape[i] != 1 ? dsize : 0;
                dsize *= _shape[i];
            }
            return dsize;
        }

        auto constexpr compute_index() const { return ptrdiff_t(0); }
        
        template <typename R, typename... Rs>
        ptrdiff_t constexpr compute_index(R first, Rs... rest) const {
            /**
             * drop outermost dimension on overflow
             */
            if constexpr (sizeof...(Rs) + 1 > N) {
                return compute_index(rest...);
            } 
            /**
             * append 0 index for missing outer dimensions
             */
            else if constexpr (sizeof...(Rs) + 1 < N ) {
                return compute_index(first, rest..., 0);
            } 
            else {
                std::array<ptrdiff_t, sizeof...(Rs) + 1> idx({
                    static_cast<long>(first), 
                    static_cast<long>(rest)...
                });
                ptrdiff_t offset = std::inner_product(_strides.begin(), _strides.end(), idx.begin(), 0);
                return offset;
            }
        }
    public:
        typedef typename T::value_type value_type;
        typedef shape_t shape_type;

        template <typename S>
        ndview(S&& data, const std::array<ptrdiff_t, N>& shape) 
        : _data(std::forward<S>(data))
        , _shape(shape) {
            compute_strides();
        }

        const shape_type& strides() const { return _strides; }
        const shape_type& shape() const { return _shape; }

        template <typename OutputIt>
        void slice(size_t axis, size_t depth, std::array<ptrdiff_t, N-1>& aux, OutputIt d_iter) const {
            //std::array<ptrdiff_t, N-1> aux;
            for(size_t i=0, j=0; i < N; i++) if(i != axis) aux[j++] = _shape[i];
            DEBUG_VALUE_OF(aux);

            iterate(axis, depth, [&](const shape_type& s) {
                size_t idx = std::inner_product(s.begin(), s.end(), this->strides().begin(), 0);
                *d_iter++ = _data[idx];
            });
        }
        
        template <typename UnaryOperation>
        void iterate(size_t d, size_t n, UnaryOperation unary_op) const {
            shape_type idx = { 0 };
            idx[d] = n;
            while(1) {
                unary_op(idx);
                size_t j;
                for(j=0; j < N; j++) {
                    if(j == d) 
                        continue;
                    auto i = j;//N - j - 1;
                    idx[i]++;
                    if(idx[i] < _shape[i]) 
                        break;
                    idx[i] = 0;
                }
                if(j == N) break;
            }
        }

        template <typename... Ts>
        value_type& operator () (Ts... args) {
            return _data[linearize(args...)];//compute_index(args...)];
        }

        template <typename... Ts>
        size_t linearize(Ts... args) {
            return compute_index(args...);
        }

        std::string to_string() const {
            std::stringstream ss;
            ss << "data=";//
            int dim = 0;
            for(auto len: shape()) {
                ss << "[";
                for(auto i: range(len)) {
                    std::vector<value_type> subm;
                    slice(dim, i, std::back_inserter(subm));
                    ss << subm;
                }
                dim++;
                ss << "]";
            }
            //<< _data 
            ss << ", shape=" << _shape << ", strides=" << _strides;
            return ss.str();
        }
    };

    template <typename T, size_t N>
    std::ostream& operator << (std::ostream& os, const ndview<T, N>& n) {
        return os << n.to_string() << std::endl;
    }

}
