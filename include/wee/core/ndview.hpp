#pragma once

#include <array>
#include <core/array.hpp>
#include <core/tuple.hpp>
#include <core/range.hpp>
#include <core/static_for.hpp>
#include <algorithm>
#include <numeric>
#include <cassert>

namespace wee {
    template <size_t N>
    class ndindexer {
        typedef std::array<ptrdiff_t, N> shape_t;
        shape_t _shape;
        shape_t _strides;
    protected:
        void compute_strides() {
            ptrdiff_t dsize = 1;
            static_for<0, N>([&] (size_t j) {
                auto i = N - j - 1;
                _strides[i] = _shape[i] != 1 ? dsize : 0;
                dsize *= _shape[i];
            });
        }

        auto constexpr compute_index() const { return ptrdiff_t(0); }

        template <size_t... Is>
        ptrdiff_t constexpr compute_index(const shape_t& s, std::index_sequence<Is...>) {
            return compute_index(s[Is]...);
        }
        
        template <typename R, typename... Rs>
        ptrdiff_t constexpr compute_index(R first, Rs... rest) const {
            /**
             * drop outermost dimension on overflow
             */
            if constexpr (sizeof...(Rs) + 1 > N) {
                return compute_index(rest...);
            } 
            /**
             * append 1 
             */
            else if constexpr (sizeof...(Rs) + 1 < N ) {
                return compute_index(first, rest..., 1);
            } 
            else {
                std::array<ptrdiff_t, sizeof...(Rs) + 1> idx {{
                    static_cast<long>(first), 
                    static_cast<long>(rest)...
                }};
                return wee::inner_product(_strides, idx);//first, rest...);
            }
        }
    public:
        typedef shape_t shape_type;
  
        ndindexer() 
            : _shape({0})
        {
        }

        explicit ndindexer(const std::array<ptrdiff_t, N>& shape) 
        : _shape(shape) 
        {
            compute_strides();
        }

        constexpr const shape_type& strides() const { return _strides; }
        constexpr const shape_type& shape() const { return _shape; }
        constexpr const size_t length() const { return array_product(_shape); }

        template <typename UnaryFunction>
        void iterate_all(UnaryFunction&& fun) const {
            //for(auto axis: range(N)) {
            static_for<0, N>([&] (auto axis) {
                for(auto depth: range(shape()[axis])) {
                    iterate_axis(axis, depth, std::forward<UnaryFunction>(fun));
                }
            });
        }

        
        template <typename UnaryFunction>
        void iterate_axis(size_t d, size_t n, UnaryFunction&& fun) const {
            shape_type idx = { { 0 } };
            idx[d] = n;
#if 1
            while(1) {
                (std::forward<UnaryFunction>(fun)(linearize_array(idx, std::make_index_sequence<N>())));
                size_t j;
                for(j=0; j < N; j++) {
                    auto i = N - j - 1;
                    if(i == d) continue;
                    idx[i]++;
                    if(idx[i] < _shape[i]) break;
                    idx[i] = 0;
                }
                if(j == N) break;
            }
#else
     
            recursive_for<N>([&] (auto a) {
                std::forward<UnaryFunction>(fun)(a);
            });

#endif
        }

        template <typename... Ts>
        size_t linearize(Ts... args) const {
            return compute_index(std::forward<Ts>(args)...);
        }

        template <size_t... Is>
        size_t linearize_array(const shape_type& s, std::index_sequence<Is...>) const {
            return linearize(s[Is]...);
        }
        /**
         * https://stackoverflow.com/questions/46782444/how-to-convert-a-linear-index-to-subscripts-with-support-for-negative-strides
         */
        shape_t delinearize(size_t i) const {
            size_t idx = i;

            shape_t out = { 0 };
            static_for<0, N>([&] (auto j) {
            //for (auto j : range(N)) {
                auto i = N - j - 1;
                auto s = idx % _shape[ i ];
                idx -= s;
                idx /= _shape[ i ];
                out[ i ] = s;
            });
            return out;

        }
        /**
         * returns the indices submatrix 
         * @param - linear starting index 
         * @param - extents of the submatrix
         */
        template <typename UnaryFunction>
        void submatrix(ptrdiff_t start, const shape_type& dims, UnaryFunction&& fun) {
#if 1 
            assert(array_product(dims) != 0);
            std::array<ptrdiff_t, N> idx = { 0 };
            while(1) {
                (std::forward<UnaryFunction>(fun)(start + linearize_array(idx, std::make_index_sequence<N>())));
                size_t j;
                for(j=0; j < N; j++) {
                    size_t i = N - j - 1;
                    idx[i]++;
                    if(idx[i] < dims[i]) break;
                    idx[i] = 0;
                }
                if(j == N) break;
            }
#else
            recursive_for<N>(fun, start, dims);
#endif
        }

        /**
         * helper function
         * @param - starting coordinate of submatrix
         * @param - extents of submatrix
         */
        template <typename UnaryFunction>
        void submatrix(const shape_type& a, const shape_type& b, UnaryFunction&& fun) {
            shape_type dims = b;// - a;
            auto start = linearize_array(a, std::make_index_sequence<N>());
            submatrix(start, dims, std::forward<UnaryFunction>(fun));
        }

        template <size_t K, size_t I = 0, typename E, typename... Ts>
        constexpr auto recursive_for(E&& f, size_t start, const shape_type& s, const Ts&... args) {
            if constexpr(I == K) {
                //for(auto i=0; i < s[I]; i++) {
                std::forward<E>(f)(start + linearize(args...));//, i);
                //}
            } else {
                for(auto i=0; i < s[I]; i++) {
                    recursive_for<K, I + 1>(f, start, s, args..., i);
                }
            }
        }
        template <typename UnaryFunction>
        void slice(size_t axis, size_t depth, std::array<ptrdiff_t, N-1>& aux, UnaryFunction d_iter) const {
            for(size_t i=0, j=0; i < N; i++) 
                if(i != axis) aux[j++] = this->shape()[i];

            this->iterate_axis(axis, depth, [&](auto s) {
                //*d_iter++ = _data->at(s);//this->linearize(s...));
                std::forward<UnaryFunction>(d_iter)(s);
            });
        }
    };

    template <typename... Ts>
    auto make_ndindexer(Ts... args) {
        return ndindexer<sizeof...(Ts)>(std::array<ptrdiff_t, sizeof...(Ts)> { args... });
    }

    template <typename T, size_t N>
    class ndview : public ndindexer<N> {
        T* _data;
    public:
        typedef typename T::value_type value_type;
        typedef typename ndindexer<N>::shape_type shape_type;
        template <typename S>
        ndview(S* data, const std::array<ptrdiff_t, N>& shape) 
        : ndindexer<N>(shape)
        , _data(data) 
        {
        }
        template <typename... Ts>
        value_type& operator () (Ts... args) {
            return _data->at(linearize(args...));//compute_index(args...)];
        }
    };
    template <typename T, typename... Ts>
    class ndfunction {
        T _fun;
        std::tuple<Ts...> _args;
        size_t _maxdim;


    public:
        auto maxdim(Ts... args) { 
            return std::max(std::initializer_list<typename std::common_type<Ts...>::type >{args...}, [] (auto a, auto b) { return a < b ? a : b; });
        }
        ndfunction(T&& fun, Ts... args) 
            : _fun(fun)
            , _args(args...)
        {
            auto broadcast_shape = [this] (const auto& val) {
                auto offset = this->maxdim() - val.maxdim();
                for(auto i: range(val.maxdim())) {
                    if(this->shape[offset + i] == 1) {
                        this->shape[offset + i] = val.shape[i];
                    } else {
                        if(val.shape[i] != this->shape[offset + i] && val.shape[i] != 1) {
                            throw std::runtime_error("broadcast error");
                        }
                    }
                }
                return true;
            };
            std::apply(broadcast_shape, _args);
        }

        template <size_t... I, typename... Args>
        auto access_impl(std::index_sequence<I...>, Args... idx) const {
            return _fun(std::get<I>(_args)(idx...)...);
        }
        template <typename... Args>
        auto operator () (Args... args) const {
            return access_impl(std::make_index_sequence<sizeof...(Args)>(), args...);
        }
    };
    template <typename T, typename... Ts>
    auto make_ndfunc(T func, Ts&&... ts) {
        return ndfunction<T, Ts...>(func, std::forward<Ts>(ts)...);
    }
}
