#include <stack>
#include <tuple>
#include <cmath>
#include <map>
#include <core/range.hpp>
#include <core/random.hpp>
#include <prettyprint.hpp>
#include <type_traits>
#include <chrono>
#include <core/logstream.hpp>
#include <sstream>

template<typename T>
constexpr T array_product(T x) { return x; }

template<typename T, typename... Ts>
constexpr T array_product(T x, Ts... xs) { return x * array_product(xs...); }

template <typename Iterator>
constexpr size_t linearize(Iterator first, Iterator last, Iterator first_dim) {
    if(first == last) {
        return *first;
    } else {
        return (*first) + (*first_dim) * linearize(std::next(first), last, std::next(first_dim));
    }
}

#define WEE_DEFAULT_COPY_AND_ASSIGN(T) \
    T(const T&) = default; \
    T& operator = (const T&) noexcept = default; \
    T(T&&) = default; \
    T& operator = (T&&) noexcept = default;
    

template <typename T, size_t Rank>
class tensor {
public:
    using array_type        = std::valarray<T>;
    using index_type        = std::valarray<size_t>;//std::array<size_t, Rank>;
    using value_type        = typename array_type::value_type;      // T
    using reference         = T&;
    using const_reference   = const T&;

    using iterator = T*;
    using const_iterator = const T*;
    
    constexpr iterator       begin()        { return std::begin(_data);  }
    constexpr const_iterator begin()  const { return std::begin(_data);  }
    constexpr const_iterator cbegin() const { return std::cbegin(_data); }
    constexpr iterator       end()          { return std::end(_data);    }
    constexpr const_iterator end()    const { return std::end(_data);    }
    constexpr const_iterator cend()   const { return std::cend(_data);   }

    tensor() { }

    template <typename... Ts>
    tensor(Ts... args) 
    : _shape{static_cast<size_t>(args)...} 
    {
        static_assert(sizeof...(Ts) == Rank);
        
        auto n = array_product(args...);//std::accumulate(std::begin(_shape), std::end(_shape), 1, std::multiplies<int>());
        
        _data.resize(n);
    }

    tensor(const array_type& data, const index_type& shape) 
    : _data(data) 
    , _shape(shape)
    {
    }

    WEE_DEFAULT_COPY_AND_ASSIGN(tensor);
    virtual ~tensor() = default;

    template <typename... Ts>
    constexpr reference at(Ts... ts) {
        std::array<size_t, sizeof...(Ts)> ix = { static_cast<size_t>(ts)... };
        return _data[linearize(std::begin(ix), std::end(ix) - 1, std::begin(_shape))];

    }

    constexpr reference operator [] (const index_type& ix) {
        //return _data[linearize(std::begin(ix), std::end(ix) - 1, std::begin(_shape))];
        return const_cast<T&>(static_cast<const tensor*>(this)->operator [] (ix));

    }
    constexpr const_reference operator [] (const index_type& ix) const {
        return _data[linearize(std::begin(ix), std::end(ix) - 1, std::begin(_shape))];
    }

    array_type slice(size_t s, const index_type& d, const index_type& i) {
        return _data[std::gslice(s, d, i)];
    }

    const index_type& shape() const {
        return _shape;
    }
    
    template <size_t S = 1>
    constexpr std::array<int, 2 * S * Rank> stencil(size_t ix) const {
        auto stencil_indices = std::array<int, 2 * S * Rank> {};
        auto get_neighbor = [this] (size_t ix) {
            return (ix >= 0 && ix < _data.size()) ? static_cast<int>(ix) : -1;
        };
        size_t offset_dim = 1;
        for(size_t d=0; d < Rank; d++) {
            stencil_indices[d * Rank + 0] = get_neighbor(ix - offset_dim); // left
            stencil_indices[d * Rank + 1] = get_neighbor(ix + offset_dim); // right
            offset_dim *= _shape[d];
        }
        return stencil_indices;
    }

    //tensor<T, Rank - 1> 
    //project(std::array<size_t, Rank - 1>& axis) {
    //    
    //}


    array_type& values() {
        //return static_cast<array_type&>(const_cast<tensor*>(this)->values());
        return const_cast<array_type&>(static_cast<const tensor*>(this)->values());
    }
    const array_type& values() const {
        return _data;
    }

    template <typename... Ts>
    tensor& reshape(Ts... args) {
        assert(array_product(static_cast<size_t>(args)...) == _data.size());
        _shape = { static_cast<size_t>(args)... };
        return *this;
    }

private:
    index_type _shape;
    array_type _data;
};


//template <typename T, size_t Rank>
//std::ostream& operator << (std::ostream& os, const tensor<T, Rank>& t) {
//    for(
//}


//#define rnd wee::random::instance()
template<typename T>
auto rnd = [] (T min = zero<T>(), T max = one<T>()) {
    using wee::random;
    static auto milliseconds_since_epoch =
        std::chrono::system_clock::now().time_since_epoch() / 
        std::chrono::milliseconds(1);

    static random _(milliseconds_since_epoch);
    return _.next(min, max);
};




template <typename T, size_t D0>
struct array_view {

    typedef std::array<size_t, D0> index_type;
    
    T* t;
    const index_type shape;

    template <typename... Ts>
    array_view(T* t, Ts... args) 
    : t(t)
    , shape{static_cast<size_t>(args)...} 
    {
        static_assert(sizeof...(Ts) == D0);
    }


    T& operator [] (const index_type& ix) {
        return t[linearize(std::begin(ix), std::end(ix) - 1, std::begin(shape))];
    }
};

using wee::range;

class model {
    bool* wave;
    int* propagator;
    int* compatible;
    int* observed;
    
    std::stack<std::tuple<int, int>> stack;

    size_t FMX, FMY, T;
    bool periodic;

    double* weights;
    double* weightLogWeights;

    int* sumsOfOnes;
    double sumOfWeights, sumOfWeightLogWeights, startingEntropy;
    double* sumsOfWeights, *sumsOfWeightLogWeights, *entropies;

    static constexpr int DX[] = { -1, 0, 1, 0 };
    static constexpr int DY[] = {  0, 1, 0,-1 };

    wee::random random{0};

public:
    model(int w, int h) : FMX(w), FMY(h) {}

    void init() {
        size_t len = FMX * FMY;
        wave = new bool[len * T];
        compatible = new int[len * T * 4];
        weightLogWeights = new double[T];
        sumOfWeights = 0;
        sumOfWeightLogWeights = 0;

        for(auto t : range(T)) {
            weightLogWeights[t] = weights[t] * std::log(weights[t]);
            sumOfWeights += weights[t]; // << uninitialized??
            sumOfWeightLogWeights += weightLogWeights[t];
        }
        startingEntropy = std::log(sumOfWeights) - sumOfWeightLogWeights / sumOfWeights;

        sumsOfOnes = new int[len];
        sumsOfWeights = new double[len];
        sumsOfWeightLogWeights = new double[len];
        entropies = new double[len];

        //stack = new std::stack<std::tuple<int, int> >;
        //stack.reserve(len * T);
    }

    // n-dimensional = on_boundary(const std::array<size_t, D0>&) = 0;
    virtual bool on_boundary(int x, int y) = 0;
    virtual void ban(size_t i, size_t t) {
        size_t len = FMX * FMY;
        array_view<bool, 2> vw(wave, len, T);
        vw[{i, t}] = false;
        int* comp = &array_view<int, 3>(compatible, len, T, 4)[{i, t}];
        for(auto d : range(4)) {
            comp[d] = 0;
        }
        stack.push({i ,t});

        auto sum = sumsOfWeights[i];
        entropies[i] += sumsOfWeightLogWeights[i] / sum - std::log(sum);

        sumsOfOnes[i]               -= 1;
        sumsOfWeights[i]            -= weights[t];
        sumsOfWeightLogWeights[i]   -= weightLogWeights[t];

        sum = sumsOfWeights[i];
        entropies[i] -= sumsOfWeightLogWeights[i] / sum - std::log(sum);
    }

    int observe() {
        double minH = 1E+3;
        int argmin = -1;
        size_t len = FMX * FMY;
        int amount;
        double H;

        for(auto i : range(len)) {
            if(on_boundary(i % FMX, i / FMX))
                continue;
            if(amount = sumsOfOnes[i]; amount == 0)
                return -1;
            if(H = entropies[i]; amount > 1 && H <= minH) {
                if(auto noise = 1E-6 * rnd<decltype(H)>(); H + noise < minH) {
                    minH = H + noise;
                    argmin = i;
                }
            }
        }
        if(argmin == -1) {
            observed = new int[len]; // <<< TODO: hoist!
            for(auto i : range(len)) {
                for(auto t : range(T)) {
                    if(array_view<bool, 2> vw(wave, len, T); vw[{i, t}]) {
                        observed[i] = t;
                    }
                    //if(wave[i + t * len]) {
                    //    observed[i] = t;
                    //}
                }
            }
            return 0;
        }

        double* distribution = new double[T];
        for(auto t : range(T)) {
            distribution[t] = array_view<bool, 2>(wave, len, T)[{
                (size_t)argmin, t
            }] ? weights[t] : 0.0;
        }
        size_t r = rnd<double>();

        bool* w = &wave[argmin]; // <<< correct?
        for(auto t : range(T)) {
            if(w[t] != (t==r)) {
                ban(argmin, t);
            }
        }
        
        return 1;
    }

    void propagate() {
        while(stack.size() > 0) {
            auto e = stack.top();
            stack.pop();

            int i1 = std::get<0>(e);
            int x1 = i1 % FMX;
            int y1 = i1 / FMX;
            
            for(size_t d : range(4)) {
                int dx = DX[d];
                int dy = DY[d];
                int x2 = x1 + dx;
                int y2 = y1 + dy;
                if(on_boundary(x2, y2)) {
                    continue;
                }

                x2 += x2 < 0 ? (int)FMX : x2 >= (int)FMX ? -(int)FMX : 0;
                y2 += y2 < 0 ? (int)FMY : y2 >= (int)FMY ? -(int)FMY : 0;

                int i2 = x2 + y2 * FMX;
                int* p = &array_view<int, 3>(propagator, 4, T, T)[{ 
                    d, 
                    (size_t)std::get<1>(e) 
                }];
                int* compat = &compatible[i2];

                for(auto l : range(T)){ // p.Length == T? <<< correct?
                    int t2 = p[l];
                    int* comp = &compat[t2];
                    comp[d]--;
                    if(comp[d] == 0) {
                        ban(i2, t2);
                    }
                } 
            }
        }
    }

    bool run(int seed, int ) {
        if(wave == nullptr) 
            init();
        clear();
        random = wee::random(seed);

        return false;
    }

    void clear() {
    }

};

template <typename T>
tensor<T, 1> arange(T t) {
    tensor<T, 1> res(t);
    std::iota(std::begin(res), std::end(res), 0);
    return res;
}
int main(int, char**) {
    //[[maybe_unused]] tensor<float, 3> t(3, 3, 3);
    auto t = arange(3 * 3 * 3).reshape(3, 3, 3);

    auto va = t.values();
    DEBUG_VALUE_OF(va);
    auto ar = t.slice(1, { 3 }, {3 * 3});

    // iterate through z-dimension at [1, 0]
    for(auto it = std::begin(ar); it != std::end(ar); it++) {
        DEBUG_VALUE_OF(*it);
    }

    return 0;
}
