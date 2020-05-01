#include <core/parallel.hpp>
#include <core/tinytest.hpp>
#include <core/array.hpp>
#include <iostream>
#include <mutex>
#include <unordered_set>

template <typename T, T... Ts>
constexpr std::array<T, sizeof...(Ts)> make_array(std::integer_sequence<T, Ts...> seq) {
    return { Ts...  };
}

int main(int, char**) {
    auto vec = make_array(std::make_index_sequence<12> {});
    //std::vector<int> vec(12);

    std::iota(vec.begin(), vec.end(), 0);
    auto baseline = std::accumulate(vec.begin(), vec.end(), 0);
    
    std::atomic<int> sum = 0;
    std::unordered_set<std::thread::id> ids;
    std::mutex m;
    wee::parallel_foreach(vec.begin(), vec.end(), [&] (int i) {
        {
            std::scoped_lock guard(m);
            ids.insert(std::this_thread::get_id());
        }
        sum.fetch_add(vec[i]);
    });


    suite("testing parallel accumulation") {
        test(baseline == sum);
        test(ids.size() == std::thread::hardware_concurrency());
    }

    //std::cout << "sum=" << sum << std::endl;

    return 0;
}
