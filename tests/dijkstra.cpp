#include <cstdio>
#include <vector>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <cxx-prettyprint/prettyprint.hpp>
#include <core/graph.hpp>

#define inf 0



template <typename H>
int astar(const std::vector<std::vector<int > >& adj, int from, int to, H h) {
    std::unordered_map<int, int> came_from;
    std::unordered_map<int, float> g;
    typedef int priority_t;
    typedef std::pair<priority_t, int> pair_t;
    typedef std::priority_queue<pair_t, std::vector<pair_t>, std::greater<pair_t> > heap_t;
    
    size_t n = adj.size();
   
    heap_t frontier;
    frontier.emplace(0, from);

    came_from[from] = from;
    g[from] = 0;

    while(!frontier.empty()) {
        auto current = frontier.top().second;
        frontier.pop();
        if(current == to) 
            break;

        for(size_t i=0; i < n; i++) {
            if(0 >= adj[current][i]) 
                continue;
            float new_cost = g[current] + adj[current][i];
            if(g.find(i) == g.end() || new_cost < g[i]) {
                g[i] = new_cost;
                priority_t priority = new_cost + h(i, to);
                frontier.emplace(priority, i);
                came_from[i] = current;
            }
        }
    }

    std::vector<int> path;
    int node = to;
    while(node != came_from[node]) {
        std::cout << node << std::endl;
        node = came_from[node];
    }

    std::cout << came_from << std::endl;
    return 0;
}
int main() {
	//int N = 25;
	//scanf("%d", &N);
/**
 * http://2.bp.blogspot.com/-KS2IS_wQ99k/Ux5EYJg2SZI/AAAAAAAACL8/xn2mJDQto8o/s1600/Adjacency+Matrix+Representation+of+Weighted+Graph.JPG
 */
    std::vector<std::vector<int> > adj = { 
        {inf,  2,    3, inf, inf},
        {  2, inf,  15,   2, inf},
        {  3,  15, inf, inf,  13},
        {inf,   2, inf, inf,   9},
        {inf, inf,  13,   9, inf}
    };

    auto dijkstra = [] (int a, int b) {
        return 0;
    };


	int ans = astar(adj, 0, 1, dijkstra);

    typedef wee::basic_weighted_graph<int, float> weighted_graph;
    //typedef wee::basic_graph<int> weighted_graph;

    weighted_graph G = {{{
        { 0, { 1 } },
        { 1, { 2, 3 } },
        { 2, { 1, 4 } }
    }}};

    std::unordered_map<int, int> came_from;
    G.a_star_search(0, 4, came_from, [] (const int& a, const int &b) { return 0.0f; });
    std::vector<int> path;
    G.reconstruct_path(0, 4, came_from, path);
    std::cout << came_from << std::endl;
    std::cout << path << std::endl;

	printf("%d\n", ans);
}
