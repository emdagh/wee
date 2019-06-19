#pragma once

#include <map>

namespace wee {
    template <typename K, typename V>
    std::unordered_map<V, K> inverse_map(std::unordered_map<K, V>& m) {
        std::unordered_map<V,K> inv;
        std::for_each(m.begin(), m.end(), [&inv] (const std::pair<K,V>& p) {
            inv.insert({p.second, p.first});
        });
        return inv;
    }
}
