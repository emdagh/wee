#include <tuple>
#include <iterator>
#include <vector>
#include <iostream>

namespace wee {

template <typename It>
using select_access = std::conditional_t<
    std::is_same_v<It, std::vector<bool>::iterator> ||
    std::is_same_v<It, std::vector<bool>::const_iterator>,
    typename It::value_type,
    typename It::reference
>;

template <typename T>
using select_iterator = std::conditional_t<
    std::is_const_v<std::remove_reference_t<T> >,
    typename std::decay_t<T>::const_iterator,
    typename std::decay_t<T>::iterator
>;

template <typename... Args, std::size_t... Ix>
auto any_match_impl(const std::tuple<Args...>& lhs, const std::tuple<Args...>& rhs, std::index_sequence<Ix...>) {
    return (... | (std::get<Ix>(lhs) == std::get<Ix>(rhs)));
}

template <typename... Args>
auto any_match(const std::tuple<Args...>& lhs, const std::tuple<Args...>& rhs) {
    return any_match_impl(lhs, rhs, std::index_sequence_for<Args...>{});
}

template <typename... Iters>
class zip_iterator {
    using value_type = std::tuple<
        select_access<Iters>...
    >;
    std::tuple<Iters...> _iters;
public:
    zip_iterator() = delete;

    zip_iterator(Iters&&... iters) 
    : _iters { std::forward<Iters>(iters)... }
    {
    }

    auto operator ++ () { 
        std::apply([] (auto&... args) {
            ((args+=1), ...);
        }, _iters);
        return *this;
    }

    auto operator ++ (int) {
        auto tmp = *this;
        ++*this;
        return tmp;
    }

    auto operator != (const zip_iterator& other) const {
        return !(*this == other);
    }

    auto operator == (const zip_iterator& other) const {
        return any_match(_iters, other._iters);
    }

    auto operator * () {
        return std::apply([] (auto&&... args) {
            return value_type(*args...);
        }, _iters);
    }
};

template <typename... Ts>
class zipper {
    using zip_type = zip_iterator<select_iterator<Ts>...>;
    std::tuple<Ts...> _args;
public:
    template <typename... Args>
    zipper(Args&&... args) 
    : _args(std::forward<Args>(args)...) 
    {
    }

    auto begin() {
        return std::apply([] (auto&&... args) {
            return zip_type(std::begin(args)...);
        }, _args);
    } 
    auto end() {
        return std::apply([] (auto&&... args) {
            return zip_type(std::end(args)...); 
        }, _args);
    }
};

template <typename... Ts>
auto zip(Ts&&... t) {
    return zipper<Ts...> { std::forward<Ts>(t)... };
}
} // wee


/***
int main(int, char**) {
   

    auto a = std::vector<int> { 0, 1, 2 };
    auto b = std::vector<std::string> { "one", "two", "three" };

    for(auto&& [x, y] : zip(a, b)) {
        std::cout << x << " " << y << std::endl;
    }
    
    return 0;
}*/
