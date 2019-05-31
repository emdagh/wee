#include <prettyprint.hpp>
#include <wee.hpp>
#include <sstream>
#include <numeric>
#include <vector>
#include <functional>
#include <core/range.hpp>
#include <core/ndview.hpp>
#include <core/bits.hpp>
#include <core/random.hpp>
#include <core/array.hpp>
#include <core/logstream.hpp>

#include <stack>
#include <unordered_map>

using namespace wee;

template <typename T, size_t N> struct basic_model;
template <typename T> struct tileset;

/** helper functions */
template <typename T, size_t N, size_t... Is>
constexpr size_t linearize_array_impl(const wee::ndindexer<N>& ix, const T& ary, std::index_sequence<Is...>) {
    return ix.linearize((ary[Is])...);
}

template <typename T, size_t N>
constexpr size_t linearize_array(const wee::ndindexer<N>& ix, const T& ary) {
    return linearize_array_impl<T, N>(ix, ary, std::make_index_sequence<N>()); 
}

template <typename T, T First, typename F, T... Is>
constexpr auto static_for_impl(F&& f, std::integer_sequence<T, Is...>) {
    return (std::forward<F>(f)(std::integral_constant<T, First + Is> {}), ...);
}


template <typename T, T First, T Last, typename F>
constexpr auto static_for(F&& f) {
    return static_for_impl<T, First>(std::forward<F>(f), std::make_integer_sequence<T, Last - First>());
}

template <typename T>
constexpr T to_bitmask(T idx) { return 1 << idx; }


template <typename T>
constexpr T to_index(T t) { return ctz(t); }

template <typename T, typename Iter>
void avail(T t, Iter it) {
    T tmp = t;
    size_t i=0, n=popcount(tmp);
    while (i++ < n) { 
        *it++ = to_index(tmp);
        auto lb = tmp & -tmp;
        tmp ^= lb;
    }
}

template <typename T>
constexpr float entropy_of(T t) { return 1.0f - 1.0f / static_cast<float>(popcount(t)); }

template <typename T>
void trace(T* ptr, size_t start, size_t n, size_t stride, T val) {
    for(size_t i=0; i < n; i+=stride)  ptr[start + i] = val; 
}
template <size_t N, const size_t M = N * (N<<1)>
constexpr auto make_direction_index() {
    //static const size_t M = N * (N << 1);
    std::array<ptrdiff_t, M> res;
    trace<ptrdiff_t>(&res[0], 0,     M >> 1, N + 1,  1);
    trace<ptrdiff_t>(&res[0], N * N, M >> 1, N + 1, -1);
    return res;
}


template <size_t N>
using direction_index = std::array<ptrdiff_t, N * (N << 1)>;//= build();

template <size_t N>
struct topology {
    static const int kNumNeighbors = N << 1;
    static std::array<ptrdiff_t, N * (N<<1)> sides;

    typedef std::array<ptrdiff_t, N> value_type;

    value_type _shape { 0 };

    explicit topology(const value_type& t) : _shape(t) {
    }

    size_t length() const { return array_product<value_type, N>(shape()); }

    const value_type& shape() const { return _shape; }

    constexpr bool is_valid(const value_type& c) const {
#if 0 // fast and ugly
        int count = 0;
        static_for<size_t, 0, N>([&] (auto i) {
            if(c[i] < 0 || _shape[i] <= c[i]) count++;
        });
        return count==0;
#else
        for (auto i : range(N)) {
            if (c[i] < 0 || _shape[i] <= c[i]) {
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

    constexpr size_t to_index(const value_type& at) const {
        typedef wee::ndindexer<N> indexer_type;
        indexer_type ix(_shape);
        return linearize_array(ix, at);
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


template <size_t N>
std::array<ptrdiff_t, N * (N << 1)> topology<N>::sides = make_direction_index<N>();

template <typename T, size_t N>
struct adjacency_list {
    typedef typename topology<N>::value_type shape_type;
    
    static const size_t kNumNeighbors = N << 1;

    std::vector<T> _data;

    adjacency_list(size_t n) {
        _data.resize(n * kNumNeighbors);
    }
    /**
     * add an adjacency in a specified direction
     */

    void add(size_t i_a, size_t i_b, size_t d, bool do_inverse = false) {
        //_data[i_a * N + d] |= to_bitmask(i_b);
        _data[index_for_neighbor(i_a, d)] = to_bitmask(i_b);
    }

    T at(size_t i, size_t d) const  {
        return _data[index_for_neighbor(i, d)];
    }

    size_t index_for_neighbor(size_t i, size_t d) const {
        return i * N + d;
    }

    template<typename InputIt>
    void add_example(InputIt first, const tileset<T>& ts, const topology<N>& topo) {

        ndindexer<N> ix(topo.shape());
        ix.iterate_all([&] (auto... e ) {
            size_t idx = ix.linearize(e...);
            for(auto d: range(kNumNeighbors)) {
                size_t j;
                if(topo.try_move(idx, d, &j)) {
                    add(ts.to_index(first[idx]), ts.to_index(first[j]), d, false);
                }
            }
        });

        //InputIt begin = first;
        //

        /*while(first++ != last) {
            size_t a = std::distance(first, last);
            for(auto i: range(kNumNeighbors)) {
                size_t b;
                if(topo.try_move(a, i, &b)) {
                    DEBUG_VALUE_OF(first[5]);
                    add(static_cast<size_t>(*(begin + a)), static_cast<size_t>(*(begin + b)), i);
                }
            }
        }*/
    }


    const T& operator [] (size_t i) const { return _data.at(i); }
};
template <typename T>
struct wave {
    std::vector<T> _data;
    wee::random _rand;

    wave(size_t n, T t) : _data(n, t)
    {

    }

    auto avail_at(size_t i) const { 
        std::vector<T> res;
        avail(_data[i], std::back_inserter(res)); 
        return res;
    }

    void pop(size_t i, T t) { wee::pop_bits(_data[i], t); }

    void reset(T t) { std::fill(_data.begin(), _data.end(), t); }
    
    size_t length() const { return _data.size(); }

    bool is_collapsed_at(size_t i) const {
        return is_collapsed(_data[i]);
    }

    void collapse(size_t i, T t) { _data.at(i) = t; }

    bool is_collapsed(T t) const { return popcount(t) == 1; }

    size_t min_entropy_index() {
        size_t ret = 0;
        float min_h = std::numeric_limits<float>::infinity();
        for (T i : _data) {
            if (is_collapsed(i)) 
                continue;

            float h = entropy_of(i) - _rand.next<float>(0.0f, 1.0f) / 1000.0f;
            if (h < min_h) {
                min_h = h;
                ret = i;
            }
        }
        return ret;
    }

    bool any_possible(size_t i, T t) const { return _data[i] & t; }
    bool is_same(size_t i, T t) const { return _data[i] == t; }
};
template <typename T>
struct tileset {
    /**
     * TODO: can we replace all this with a single std::unordered_multiset?
     */
    std::vector<T> _data;
    std::unordered_map<T, size_t> _names;
    std::vector<float> _frequency;

    template <typename Iter>
    static tileset make_tileset(Iter first, Iter last) {
        Iter ptr = first;
        tileset res;
        while(ptr != last) {
            res.push(*ptr++);
        }
        return res;
    }

    T tile(size_t i) const { return _data[i]; }
    size_t to_index(T t) const { return _names.at(t); }//_data.at(_names.at(t)); }
    void set_frequency(T t, float f) { _frequency[_names[t]] = f; }
    const std::vector<float> frequencies() const { return _frequency; }
    size_t length() const { return _data.size(); }
    
    template <typename InputIt, typename OutputIt>
    void make_weights(InputIt first, InputIt last, OutputIt d_first) const {
        std::multiset<typename InputIt::value_type> temp(first, last);
        for(auto it = temp.begin(); it != temp.end(); it = temp.upper_bound(*it)) {
            auto ti = to_index(*it);
            *(d_first + ti) = temp.count(*it);
        }
    }
    
    void push(T t) {
        if(_names.count(t) == 0) {
            _names.insert(std::make_pair(t, _data.size()));
            _data.push_back(t);
            _frequency.push_back(1);
        } else {
            size_t idx = to_index(t);
            _frequency[idx]++;
        }
    }

    template <typename OutputIt>
    void weights(OutputIt d_first) const {
        make_weights(_data.begin(), _data.end(), d_first);
    }
};
template <typename T, size_t N>
struct wave_propagator {
    static const size_t kNumNeighbors = N << 1;

    typedef std::function<void(const wave_propagator<T,N>&)> callback_type;
    typedef typename topology<N>::value_type coordinate_type;

    wave<T>* _wave = nullptr;
    topology<N> _topo;
    wee::random _rnd;
    callback_type on_update;

    explicit wave_propagator(wave<T>* w, const topology<N>& topo) : _wave(w), _topo(topo) {
    }

    void propagate(size_t at, const adjacency_list<T, N>& adj) const {
        std::vector<T> open = { at };
        while(!open.empty()) {
            T cell = open.back();
            open.pop_back();
            for(auto d: range(kNumNeighbors)) {
                size_t j;
                if(!_topo.try_move(cell, d, &j)) {
                    continue;
                }
                size_t m = 0;
                for(auto a : _wave->avail_at(cell)) {
                    wee::push_bits(m, adj.at(a, d));
                }
                if(m == 0) {
                    continue;
                }
                if(!_wave->any_possible(j, m)) {
                    return;
                }
                if(!_wave->is_same(j, m)) {
                    open.push_back(j);
                }
                _wave->collapse(j, m);
            }
        }
    }

    bool collapse(size_t i, const std::vector<float>& weights) {
        std::unordered_map<int, float> w;
        float total_weight = 0.f;
        auto options = _wave->avail_at(i);//_wave->_data[i]);
        for(auto t: options) {
            w.insert(std::pair(t, weights[t]));
            total_weight += weights[t];
        }
        float random = _rnd.next<float>(0.f, 1.0f) * total_weight;

        for(const auto& [key, val]: w) {
            random -= val;
            if(random < 0) {
                _wave->collapse(i, to_bitmask<T>(key));
                return true;
            }
        }
        return false;
    }

    bool is_done() const {
        for(auto i: range(_wave->length())) {
            if(!_wave->is_collapsed_at(i)) {
                return false;
            }
        }
        return true;
    }

    /*void run(const basic_model<T,N>* model) {
    }*/
    
};
template <typename T, size_t N, typename R = std::vector<size_t> >
struct basic_constraint {
    virtual ~basic_constraint() = default;
    virtual void init(const wave_propagator<T, N>&, R*) = 0;
    virtual void check(const wave_propagator<T, N>&, R*) = 0;
};
template <typename T, size_t N>
struct basic_model {
    typedef typename topology<N>::value_type shape_type;
    typedef tileset<T> tileset_type;
    typedef adjacency_list<T,N> adjacency_list_type;
   
    std::vector<basic_constraint<T, N>* > _constraints;
    tileset_type&& _tileset;
    adjacency_list_type&& _adjacencies;
    T _banned;

    basic_model(tileset_type&& ts, adjacency_list_type&& a) 
    : _tileset(std::forward<tileset_type>(ts))
    , _adjacencies(std::forward<adjacency_list_type>(a)) 
    {
    }

    void add_constraint(basic_constraint<T,N>* ptr) { _constraints.push_back(ptr); }

    void ban(T t) { _banned |= index_of(_tileset.to_index(t)); }

    T domain() const {
        T res = 0;
        for(auto i : range(_tileset.length())) {
            //res |= to_bitmask(_tileset.to_index(_tileset.tile(i)));
            wee::push_bits(res, to_bitmask(_tileset.to_index(_tileset.tile(i))));
        }
        return res;
    }

    //template <typename OutputIt>
    void solve(const topology<N>& topo) {
        auto len = topo.length();
        wave<T> wv(len, domain());
        /**
         * apply all constraints to the new wave
         */
        wave_propagator<T, N> prop(&wv, topo);
        for(auto* ptr : _constraints) {
            std::vector<size_t> res;
            ptr->init(prop, &res);
            for(auto i: res) {
                prop.propagate(i, _adjacencies);
            }
        }
        /**
         * register a callback lambda that will check all constraints 
         * whenever a collapse cycle has finished.
         */
        prop.on_update = [this] (const wave_propagator<T,N>& p) {
            for(auto* ptr : _constraints) {
                std::vector<size_t> res;
                ptr->check(p, &res);
                for(auto i: res) {
                    p.propagate(i, _adjacencies);
                }
            }
        };
        /**
         * here we run the wave function collapse algorithm
         */
        std::vector<float> weights(_tileset.length());
        _tileset.weights(weights.begin());

        while(!prop.is_done()) {
            size_t i = wv.min_entropy_index();
            prop.collapse(i, _tileset.frequencies());
            prop.propagate(i, _adjacencies);
        }
    }
};
template <typename T, size_t N>
struct border_constraint;
template <typename T, size_t N>
struct corner_constraint {
    static const size_t kNumCorners = 1 << N;
    typedef typename std::array<T, kNumCorners> corners_type;
    typedef typename topology<N>::value_type shape_type;
    corners_type _corners = { 0 };

    corner_constraint(size_t axis, const corners_type& corners) { 
    }

    static corners_type make_corners(const shape_type& shape) {
        ndindexer<N> ix(shape);
        //for(auto dim : range(N)) {
        //    std::array<ptrdiff_t, N-1> aux; 
        //}
    }
        
};

void make_demo() {
    std::unordered_map<int, const char*> tile_colors = {
        { 110, GREEN },
        { 111, YELLOW },
        { 112, BLUE }
    };

    std::unordered_map<int, char> tiles = {
        { 110, '#' },
        { 111, '.' },
        { 112, '~' },
        { 0, ' ' }
    };

    std::vector<int> example = {
        110, 110, 110, 110,
        110, 110, 110, 110,
        110, 110, 110, 110,
        110, 111, 111, 110,
        111, 112, 112, 111,
        112, 112, 112, 112,
        112, 112, 112, 112
    };

    auto ts = tileset<uint64_t>::make_tileset(example.begin(), example.end());
    adjacency_list<uint64_t, 3> a(3);
    /**
     * TODO: this will also require the tileset to function properly
     */
    a.add_example(example.begin(), ts, topology<3> { { 3,3,3} }); 
    basic_model<uint64_t, 3> model(
        std::forward<tileset<uint64_t> >(ts), 
        std::forward<adjacency_list<uint64_t, 3> >(a)
    );
    model.solve(topology<3>({15,15,15}));
}

int main(int argc, char** argv) {
    make_demo();
    return 0;
}
