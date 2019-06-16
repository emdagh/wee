#include <cstdio>
#include <vector>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <cxx-prettyprint/prettyprint.hpp>
#include <core/graph.hpp>

#define inf 0

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


    typedef wee::basic_weighted_graph<int, float> weighted_graph;
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

}
