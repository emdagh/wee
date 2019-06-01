#pragma once

#include <array>
#include <core/array.hpp>
#include <core/tuple.hpp>
#include <core/range.hpp>
#include <algorithm>
#include <numeric>

namespace wee {



    template <size_t N, size_t I = 0, typename E, typename... Ts>
    constexpr auto recursive_for(E&& f, const Ts&... args) {
        std::tuple<Ts...> x = std::make_tuple(args...);
        if constexpr(I == N) {
            for(auto i=0; i < std::get<0>(x).shape()[I]; i++) {
                //std::forward<E>(f(args..., i));
                (std::forward<E>(f)(args...));
            }
        } else {
            for(auto i=0; i < std::get<0>(x).shape()[I]; i++) {
                recursive_for<N, I + 1>(std::forward<E>(f), args..., i);
            }
        }
    }


    template <size_t N>
    class ndindexer {
        typedef std::array<ptrdiff_t, N> shape_t;
        shape_t _shape;
        shape_t _strides;
    protected:
        auto compute_strides() {
            ptrdiff_t dsize = 1;
            static_assert(N > 0);
            for(auto j : range(N)) {
                auto i = N - j - 1;
                _strides[i] = _shape[i] != 1 ? dsize : 0;
                dsize *= _shape[i];
            }
            return dsize;
        }

        auto constexpr compute_index() const { return ptrdiff_t(0); }

        /*ptrdiff_t constexpr compute_index_impl(const shape_t& idx) const {
            return std::inner_product(_strides.begin(), _strides.end(), idx.begin(), 0);
        }*/

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
                std::array<ptrdiff_t, sizeof...(Rs) + 1> idx({
                    static_cast<long>(first), 
                    static_cast<long>(rest)...
                });
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
        constexpr const size_t length() const { return array_sum(_shape); }

        template <typename UnaryFunction>
        void iterate_all(UnaryFunction&& fun) const {
            for(auto axis: range(N)) {
                for(auto depth: range(shape()[axis])) {
                    iterate_axis(axis, depth, std::forward<UnaryFunction>(fun));
                }
            }
        }

        template <typename T, size_t... Is>
        void do_operation(T&& f, const shape_type& s, std::index_sequence<Is...>) const {
            (std::forward<T>(f)(s[Is]...));
        }
        
        template <typename UnaryFunction>
        void iterate_axis(size_t d, size_t n, UnaryFunction&& fun) const {
            shape_type idx = { 0 };
            idx[d] = n;
            while(1) {
                //do_operation(std::forward<UnaryOperation>(unary_op), idx, std::make_index_sequence<N>());
                fun(linearize_array(idx, std::make_index_sequence<N>()));
                size_t j;
                for(j=0; j < N; j++) {
                    auto i = N - j - 1;
                    if(i == d) 
                        continue;
                    idx[i]++;
                    if(idx[i] < _shape[i]) 
                        break;
                    idx[i] = 0;
                }
                if(j == N) break;
            }
        }

        template <typename... Ts>
        size_t linearize(Ts... args) const {
            return compute_index(args...);
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
            for (auto j : range(N)) {
                auto i = N - j - 1;
                auto s = idx % _shape[ i ];
                idx -= s;
                idx /= _shape[ i ];
                out[ i ] = s;
            }
            return out;

        }
        /*template <typename InputIt, typename OutputIt>
        void gslice(InputIt it, size_t axis, size_t depth, std::array<ptrdiff_t, N-1>& aux, OutputIt d_iter) const {
            for(size_t i=0, j=0; i < N; i++) if(i != axis) aux[j++] = this->shape()[i];

            iterate(axis, depth, [&](auto... s) {
                *d_iter++ = it[linearize(s...)];
            });
        }*/

        /**
         * returns the indices submatrix 
         * @param - linear starting index 
         * @param - extents of the submatrix
         */
        template <typename UnaryFunction>
        void submatrix(ptrdiff_t start, const shape_type& dims, UnaryFunction&& fun) {
            //DEBUG_VALUE_OF(strides);
            std::array<ptrdiff_t, N> idx = { 0 };
            while(1) {
                //*d_first++ = start + compute_index(idx, std::make_index_sequence<N>());
                //(std::forward<UnaryFunction>(fun)(start + compute_index(idx, std::make_index_sequence<N>())));
                fun(linearize_array(idx, std::make_index_sequence<N>()));
                size_t j;
                for(j=0; j < dims.size(); j++) {
                    size_t i = N - j - 1;
                    idx[i]++;
                    if(idx[i] < dims[i]) break;
                    idx[i] = 0;
                }
                if(j == dims.size()) break;
            }
        }

        /**
         * helper function
         * @param - starting coordinate of submatrix
         * @param - extents of submatrix
         */
        template <typename UnaryFunction>
        void submatrix(const shape_type& a, const shape_type& b, UnaryFunction&& fun) {
            shape_type dims = b;// - a;
            auto start = linearize(a, std::make_index_sequence<N>());
            submatrix(start, dims, std::forward<UnaryFunction>(fun));
        }
    };

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
        template <typename OutputIt>
        void slice(size_t axis, size_t depth, std::array<ptrdiff_t, N-1>& aux, OutputIt d_iter) const {
            for(size_t i=0, j=0; i < N; i++) if(i != axis) aux[j++] = this->shape()[i];

            this->iterate_axis(axis, depth, [&](auto s) {
                *d_iter++ = _data->at(s);//this->linearize(s...));
            });
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
