#pragma once

namespace wee {

    template <typename T>
    struct basic_graph {
        std::unordered_map<T, std::unordered_set<T> > _edges;

        const auto& neighbors(const T& v) {
            return _edges[v];
        }


        void breadth_first_search(const T& start, const T& goal, std::unordered_map<T, T>& came_from) {
            std::queue<T> frontier;
            frontier.push(start);

            while(!frontier.empty()) {
                T current = frontier.front();
                frontier.pop();

                if(current == goal) 
                    break;

                for(T next : this->neighbors(current)) {
                    if(came_from.find(next) == came_from.end()) {
                        frontier.push(next);
                        came_from[next] = current;
                    }
                }
            }
        }

    };

    template <typename T, typename W> 
    struct basic_weighted_graph : public basic_graph<T> {

        std::unordered_map<T, std::unordered_map<T, W> > _weights;

        /**
         * @returns - std::optional containing weight or nullopt_t
         */
        auto cost(const T& from, const T& to) {
            return _weights[from].find(to) != _weights[from].end() 
                ? std::optional<W>{ _weights[from][to] }
                : std::nullopt;
        }
        /**
         * implements the A* search algorithm.
         * @param - IN start: start node 
         * @param - IN goal: finish, or goal, node
         * @param - OUT came_from: list of edges visited from->to 
         * @param - IN heuristic: Heuristic function for guided search
         *
         */
        template <typename H>
        void a_star_search(const T& start, const T& goal, std::unordered_map<T, T>& came_from, H heuristic) {
            std::unordered_map<T, W> g;

            std::priority_queue<std::pair<W, T> > frontier;
            frontier.emplace((W){}, start); // make use of zero initializer for structs
            
            came_from[start] = start;
            g[start] = (W){};

            while(!frontier.empty()) {
                T current = frontier.top().second;
                frontier.pop();

                if(current == goal) 
                    break;

                for(T next: this->neighbors(current)) {
                    W new_cost = g[current] + this->cost(current, next).value_or(std::numeric_limits<W>::infinity());
                    if(!g.count(next) || new_cost < g[next]) {
                        g[next] = new_cost;
                        came_from[next] = current;
                        W priority = new_cost + heuristic(next, goal);
                        frontier.emplace(priority, next);
                    }
                }
            }
        }

        void dijkstra(const T& start, const T& goal, std::unordered_map<T, T>& came_from) {

            std::unordered_map<T, W> g;

            std::priority_queue<std::pair<W, T> > frontier;
            frontier.emplace((W){}, start); // make use of zero initializer for structs
            
            came_from[start] = start;
            g[start] = (W){};

            while(!frontier.empty()) {
                T current = frontier.top().second;
                frontier.pop();

                if(current == goal) 
                    break;

                for(T next: this->neighbors(current)) {
                    W new_cost = g[current] + this->cost(current, next).value_or(std::numeric_limits<W>::infinity());
                    if(!g.count(next) || new_cost < g[next]) {
                        g[next] = new_cost;
                        came_from[next] = current;
                        frontier.emplace(new_cost, next);
                    }
                }
            }
        }

        void reconstruct_path(const T& start, const T& goal, const std::unordered_map<T, T>& came_from, std::vector<T>& res) {
            res.clear();
            T current = goal;
            while(current != start) {
                res.push_back(current);
                current = came_from.at(current);//[current];
            }
            res.push_back(start);
            std::reverse(res.begin(), res.end());
        }
    };
}
