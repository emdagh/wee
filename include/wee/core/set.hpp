#include <algorithm>

namespace wee {
    template <typename T>
    T set_intersect(const T& a, const T& b) {
        using key_type = typename T::value_type;
        if(a.size() <= b.size()) {
            T res;
            std::copy_if(a.begin(), a.end(), std::inserter(res, res.end()), [&b] (const key_type& k) {
                return b.count(k) != 0;
            });
            return res;
        }
        return set_intersect(b, a);
    }
    template <typename T>
    T set_difference(const T& a, const T& b) {
        using key_type = typename T::value_type;
        if(a.size() <= b.size()) {
            T res;
            std::copy_if(a.begin(), a.end(), std::inserter(res, res.end()), [&b] (const key_type& k) {
                return b.count(k) == 0;
            });
            return res;
        }
        return set_difference(b, a);
    }



    template <typename T>
    T set_union(const T& a, const T& b) {
        T res { a };
        res.insert(b.begin(), b.end());
        return res;
    
    }
}