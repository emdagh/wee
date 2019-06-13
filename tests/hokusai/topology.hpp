#pragma once

template <size_t N>
struct topology {
    static const int kNumNeighbors = N << 1;
    static const int kNumDirections = N * (N << 1);
    static const int kNumCorners = 2 << N;

    static std::array<ptrdiff_t, N * (N<<1)> sides;

    typedef std::array<ptrdiff_t, N> value_type;

    value_type _shape { 0 };
    std::array<bool, N> _is_periodic;

    explicit topology(const value_type& t, const std::array<bool, N>& is_periodic = {false}) 
    : _shape(t) 
    , _is_periodic(is_periodic)
    {
    }

    size_t length() const { 
        return array_product(shape());
    }

    const value_type& shape() const { 
        return _shape; 
    }

    constexpr bool is_valid(const value_type& c) const {
#if 0 // fast and ugly
        int count = 0;
        static_for<size_t, 0, N>([&] (auto i) {
            if(c[i] < 0 || _shape[i] <= c[i]) count++;
        });
        return count==0;
#else
        for (auto i : range(N)) {
            if (!_is_periodic[i] && (c[i] < 0 || _shape[i] <= c[i])) {
                return false;
            }
        }
        return true;
#endif
    }

    auto to_coordinate(size_t i) const { 
        ndindexer<N> ix(_shape);
        return ix.delinearize(i);
    }

    constexpr size_t to_index(const value_type& at_) const {
        typedef wee::ndindexer<N> indexer_type;
        auto at = at_ % _shape;
        indexer_type ix(_shape);
        return ix.linearize_array(at, std::make_index_sequence<N>{});
    }

    constexpr bool try_move(const value_type& from, const value_type& d, size_t* d_index) const {
        auto to = from + d;
        if (is_valid(to)) {
            return *d_index = to_index(to), true;
        }
        return false;
    }

    constexpr bool try_move(const value_type& from, size_t i, size_t* d_index) const {
        return try_move(from, neighbor(i), d_index);
    }
    constexpr bool try_move(size_t i, size_t j, size_t* d_index) const {
        return try_move(to_coordinate(i), j, d_index);
    }

    template <typename OutputIt>
    void neighbor_copy(size_t i, OutputIt d_first) const {
        auto first = sides.begin() + i * N;
        auto last = first + N;
        std::copy(first, last, d_first);
    }

    value_type neighbor(size_t i) const {
        value_type res;
        neighbor_copy(i, res.begin());
        return res;
    }
};
