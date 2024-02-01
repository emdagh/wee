#pragma once

#include <iostream>
#include <queue>
#include <stack>
#include <unordered_map>

#include <iostream>
#include <queue>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <optional>
#include <limits>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>


namespace wee {


template <typename T, typename W=float>
using priority_queue_element = std::pair<W, T>;

template <typename T>
using priority_queue = std::priority_queue<
    priority_queue_element<T>,
    std::vector<priority_queue_element<T>>,
    std::greater<priority_queue_element<T>>
>;

template <typename T>
using path_type = std::unordered_map<T,T>;

template <typename T>
struct graph {
    virtual float cost(const T& from, const T& to) = 0;
    virtual std::vector<T> neighbors(const T&) = 0;
    virtual std::vector<T> nodes() = 0;

    bool is_cyclic() {
        for(T current: this->nodes()) {
            if(is_cyclic_from(current)) {
                return true;
            }
        }
        return false;
    }

    bool is_cyclic_from(T start) {
        std::stack<T> frontier;
        frontier.push(start);
        std::unordered_map<T,T> came_from = {{start, start}};

        while (!frontier.empty()) {
            T current = first_of(frontier);//.front();
            frontier.pop();

            for (auto next : this->neighbors(current)) {
                if (!came_from.count(next)) {
                    frontier.push(next);
                    came_from[next] = current;
                } else {
                    return true;
                }
            }
        }

        return false;
    }
};

template <typename T>
struct unweighted_graph : public graph<T> {
    std::unordered_map<T, std::unordered_set<T> > _adj;

    virtual float cost(const T& from, const T& to) { return 1.0f; }
    virtual std::vector<T> neighbors(const T& t) { return _adj[t]; }
    virtual std::vector<T> nodes() { 
        std::vector<T> ret;
        std::transform(_adj.begin(), _adj.end(), std::back_inserter(ret), [] ( const auto& kv) { 
            return kv.first; 
        });
        return ret;
    }
};

template <typename T>
struct weighted_graph : public graph<T> {
    using adj_list_type = std::unordered_map<T, std::unordered_map<T, float> >;
    adj_list_type _adj;

    explicit weighted_graph(const adj_list_type& adj) : _adj(adj) {}

    virtual float cost(const T& from, const T& to) { return _adj[from].count(to) ? _adj[from][to] : std::numeric_limits<float>::infinity(); }
    virtual std::vector<T> neighbors(const T& t) { 
        std::vector<T> ret = {};
        std::transform(_adj[t].begin(), _adj[t].end(), std::back_inserter(ret), [] (const auto& kv) { 
            return kv.first; 
        });
        return ret;
    }

    virtual std::vector<T> nodes() { 
        std::vector<T> ret;
        std::transform(_adj.begin(), _adj.end(), std::back_inserter(ret), [] ( const auto& kv) { 
            return kv.first; 
        });
        return ret;
    }

};

template <typename T>
void depth_first_search(graph<T>* g, const T& start, const T& goal, std::unordered_map<T,T>& came_from) {
    std::stack<T> frontier;
    frontier.push(start);

    while (!frontier.empty()) {
        T current = first_of(frontier);//.front();
        frontier.pop();

        std::cout << "current=" << current << std::endl;

        if (current == goal) break;

        for (auto next : g->neighbors(current)) {
             std::cout << "next=" << next << std::endl;
            if (!came_from.count(next)) {
                frontier.push(next);
                came_from[next] = current;
            }
        }
    }
}

template <typename T, typename H>
void a_star(weighted_graph<T>* g, const T& start, const T& goal, std::unordered_map<T, T>& came_from, H&& heuristic) {
    std::unordered_map<T, float> G;
    priority_queue<T> frontier;
    frontier.emplace((float){}, start); // make use of zero initializer for structs
    
    came_from[start] = start;
    G[start] = (float){};
    while(!frontier.empty()) {
        T current = frontier.top().second;
        frontier.pop();
        if(current == goal) {
            break;
        }
        for(T next: g->neighbors(current)) {
            float new_cost = G[current] + g->cost(current, next);
            if(!G.count(next) || new_cost < G[next]) {
                G[next] = new_cost;
                came_from[next] = current;
                new_cost += heuristic(next, goal);
                frontier.emplace(new_cost, next);
            }
        }
    }
}

template <typename T>
void dijkstra( weighted_graph<T>* g, const T& start, const T& goal, path_type<T>& came_from) {
    std::unordered_map<T, float> running_cost = {{start, 0.0f}};
    priority_queue<T> frontier;// = {{0.0f, start }};
    frontier.emplace(0.0f, start);
    came_from[start] = start;

    while(!frontier.empty()) {
        T current = frontier.top().second;
        frontier.pop();
        if(current == goal) {
            break;
        }

        for(auto next: g->neighbors(current)) {
            
            auto new_cost = running_cost[current] + g->cost(current, next);
            if(!running_cost.count(next) || new_cost < running_cost[next]) {
                running_cost[next] = new_cost;
                came_from[next] = current;
                frontier.emplace(new_cost, next);
            }
        }
    }
}



template<typename T>
std::list<T> reconstruct_path(const T& start, const T& goal, path_type<T>& came_from) {
    std::list<T> path;
    T current = goal;
    if (!came_from.count(goal)) {
        return path; 
    }
    while (current != start) {
        path.push_front(current);
        current = came_from[current];
    }
    path.push_front(start); 

    return path;
}
/*
int main(int, char**) {
    weighted_graph<char> wg {{
    {'A', {{'B', 1.0f}}},
    {'B', {{'C', 1.0f}}},
    {'C', {{'B', 1.0f}, {'D',1.0f}, {'F',1.0f}}},
    {'D', {{'C', 1.0f}, {'E',1.0f}}},
    {'E', {{'F', 1.0f}}},
    {'F', {}},
    }};


    std::cout << "is_cyclic=" << (wg.is_cyclic() ? "true" : "false") << std::endl;
    std::cout << "==========" << std::endl;
    {
        std::cout << "dijkstra path A->F" << std::endl;
        path_type<char> came_from;
        dijkstra(&wg, 'A', 'F', came_from);
        auto pt = reconstruct_path('A', 'F', came_from);
        std::copy(pt.begin(), pt.end(), std::ostream_iterator<char>(std::cout, ","));
        std::cout << std::endl;
    }
    std::cout << "==========" << std::endl;
    {
        std::cout << "A* path A->F" << std::endl;
        path_type<char> came_from;
        a_star(&wg, 'A', 'F', came_from, [] (const char&, const char&) {
            // of course, here you can put some distance heuristic (manhattan, euclidean, etc.)
            // f.eks. `return std::abs(a.x - b.x) + std::abs(a.y - b.y);`
            return 1.0f;
            
        });
        auto pt = reconstruct_path('A', 'F', came_from);
        std::copy(pt.begin(), pt.end(), std::ostream_iterator<char>(std::cout, ","));
    }    
    return 0;
}*/

}
