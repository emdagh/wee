#pragma once

template <typename T, typename H, size_t kNumDimensions = 2>
void astar(T a, T b, const std::vector<T>& adjacency, H heuristic) {
    constexpr const size_t kNumNeighbors = std::pow(3, kNumDimensions) - 1;
    /**
     * G = the distance between the _current_ node and the _start_ node
     */

    /**
     * H = estimated distance from the _current_ node to the _end_ node
     */

    /**
     * F = the total cost of the node. F = G + H
     */
}
