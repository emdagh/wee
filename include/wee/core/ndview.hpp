#pragma once

#include <array>

namespace wee {


    template <typename T, size_t N>
    class ndview {
        typedef std::array<ptrdiff_t, N> shape_t;
        T* _data;
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

        ptrdiff_t constexpr compute_index_impl(const shape_t& idx) const {
            return std::inner_product(_strides.begin(), _strides.end(), idx.begin(), 0);
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
                return compute_index_impl(idx);
            }
        }
    public:
        typedef typename T::value_type value_type;
        typedef shape_t shape_type;

        template <typename S>
        ndview(S* data, const std::array<ptrdiff_t, N>& shape) 
        : _data(data) //std::forward<S>(data))
        , _shape(shape) {
            compute_strides();
        }

        constexpr const shape_type& strides() const { return _strides; }
        constexpr const shape_type& shape() const { return _shape; }

        template <typename OutputIt>
        void slice(size_t axis, size_t depth, std::array<ptrdiff_t, N-1>& aux, OutputIt d_iter) const {
            for(size_t i=0, j=0; i < N; i++) if(i != axis) aux[j++] = _shape[i];

            iterate(axis, depth, [&](const shape_type& s) {
                *d_iter++ = _data->at(linearize(s));
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
        
        template <size_t S, size_t I=0, typename E, typename... Args>
        void recursive_for(E&& e, Args... args) {
            if constexpr (I == S) {
                for(auto i=0; i < shape()[I]; i++) {
                    e(std::array<ptrdiff_t, N>({args..., i}));
                }
            } else {
                for(auto i=0; i < shape()[I]; i++) {
                    recursive_for<S, I + 1>(e, args..., i);
                }
            }
        }

        template <typename... Ts>
        value_type& operator () (Ts... args) {
            return _data->at(linearize(args...));//compute_index(args...)];
        }

        size_t linearize(const shape_t& idx) const {
            return compute_index_impl(idx);
        }

        template <typename... Ts>
        size_t linearize(Ts... args) const {
            return compute_index(args...);
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
    /**
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

            [[maybe_unused]] auto broadcast_shape = [this] (const auto& val) {
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
            //std::for_each(broadcast_shape, args...);
        }

        template <size_t... I, typename... Args>
        auto access_impl(std::index_sequence<I...>, Args... idx) const {
            return _fun(std::get<I>(_args)(idx...)...);
        }

        template <typename... Args>
        auto operator () (Args... args) const {
            return access_impl(std::make_index_sequence<sizeof...(Args)>(), args...);
        }
    };*/

    template <typename T, size_t N>
    std::ostream& operator << (std::ostream& os, const ndview<T, N>& n) {
        return os << n.to_string() << std::endl;
    }

}
