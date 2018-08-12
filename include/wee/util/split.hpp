#pragma once

#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>

namespace wee { 

    template <typename Iterator>
    std::vector<std::pair<Iterator, Iterator>>
    split(Iterator begin, Iterator end, std::size_t n) {
        std::vector<std::pair<Iterator, Iterator> > ranges;
        ranges.reserve(n);
        std::generate_n(std::back_inserter(ranges), n, [&, size = std::distance(begin, end)] {
                const auto cur = begin;
                std::advance(begin, size / n + (ranges.size() < size % n));
                return std::make_pair(cur, begin);
                });
        return ranges;
    }
}
