


template <typename T>
auto to_bitmask = [] (size_t index) {
    return static_cast<T>(1 << index);
};

template <typename bitmask_t>
auto to_index = [] (const bitmask_t& b) {
    return __builtin_ctzl(b);
};

template <typename It>
auto random_from(It begin, It end) {
    auto len = std::distance(begin, end);
    auto rndi = static_cast<decltype(len)>(wee::randf(0.f, static_cast<float>(len)));
    auto it = begin;
    std::advance(it, rndi);
    return it; 
};

void wfc(const int* in_map, const int2& in_size, int* out_map, const int2& out_size) {
    using wee::range;
    /**
     * get all unique tiles + their frequencies
     */
    typedef uint64_t bitmask_t;
    constexpr int kNumDimensions    = 2;
    constexpr int kNumEdges         = kNumDimensions << 1; 
    /**
     * Step 1: read the map, and for each input tile; register it and count it's frequency.
     */
    int n = in_size.x * in_size.y;
    std::vector<int> tileset(in_map, in_map + n);
    std::sort(tileset.begin(), tileset.end());
    tileset.erase(std::unique(tileset.begin(), tileset.end()), tileset.end());
    /**
     * Step 2: Construct the valid neigbor bitmasks
     */
    constexpr int2 neighbors[kNumEdges] = { 
        { 0,  1}, // top
        { 1,  0}, // right
        { 0, -1}, // bottom
        {-1,  0}, // left
    };
    std::unordered_map<int, int> tile_to_index;
    std::unordered_map<int, int> index_to_tile;
    for(size_t i=0; i < tileset.size(); i++) {
        tile_to_index[tileset[i]] = i;
        index_to_tile[i] = tileset[i];
    }


    DEBUG_VALUE_OF(index_to_tile);

    /**
     * adjacency stores bitmask of possible combinations for each edge / hyperplane
     */
    std::vector<bitmask_t> adjacency(tileset.size() * kNumEdges, 0); 
    for(int y=0; y < in_size.y; y++) {
        for(int x=0; x < in_size.x; x++) {
            int ix0 = x + y * in_size.x;
            int self = in_map[ix0];

            for(int z=0; z < kNumEdges; z++) {
                const int2& n = neighbors[z];
                int2 p = {
                    (x + n.x),// + in_size.x) % in_size.x,
                    (y + n.y) // + in_size.y) % in_size.y
                    //std::min(x + n.x, in_size.x),
                    //std::min(y + n.y, in_size.y)
                };
                //int ix1 = p.x + p.y * in_size.x;
                //if(ix1 != ix0) { // happens for ND-1 cases.
                if( p.x > 0 && p.x < (in_size.x - 1) && 
                    p.y > 0 && p.y < (in_size.y - 1)) {
                    int nt = in_map[p.x + p.y * in_size.x];
                    adjacency[tile_to_index[self] * 4 + z] |= to_bitmask<bitmask_t>(tile_to_index[nt]);
                }
                //} 
                //else {
                //    adjacency[tile_to_index[self] * 4 + z] = -1;//to_bitmask<bitmask_t>(-1);
                //}
            }
        }
    }

    DEBUG_VALUE_OF(adjacency);
    DEBUG_VALUE_OF(to_index<bitmask_t>(16));

    /**
     * construct an initial mask of possibilities by iterating over the tileset.
     */
    bitmask_t initial_mask = 0;
    for(auto it : tile_to_index) {
        initial_mask |= to_bitmask<bitmask_t>(it.second);
    }

    DEBUG_VALUE_OF(initial_mask);

    size_t out_n = out_size.x * out_size.y;
    //
    // cells store the bitmask of the possible indices into the tileset.
    //
    std::vector<bitmask_t> cells(out_n);
    std::generate(std::begin(cells), std::end(cells), [&] () { return initial_mask; }); // everything is possible...
    
    int cix = static_cast<int>(wee::randf(0, out_size.x - 1) + 0.5f); // index of random cell on the N+1D grid.
    int ciy = static_cast<int>(wee::randf(0, out_size.y - 1) + 0.5f); // index of random cell on the N+1D grid.
    
    DEBUG_VALUE_OF(cix);
    DEBUG_VALUE_OF(ciy);

    cells[cix + ciy * out_size.x] = to_bitmask<bitmask_t>(tile_to_index[*random_from(tileset.begin(), tileset.end())]);
    auto pop = [] (std::map<int, int2>& a, int2* res) {
        if(a.empty()) return false;
        auto it = a.begin();
        *res = (*it).second;
        a.erase(it);
        return a.empty();
    };

    std::map<int, int2> open = { { 0, {cix, ciy} } };
    int2 coord;// = open.back(); // 

    do {
        pop(open, &coord);

        DEBUG_VALUE_OF(coord);

        if(__popcount(cells[coord.x + coord.y * out_size.x]) >= 1) {
            std::vector<int> opts(__popcount(cells[coord.x + coord.y * out_size.x]));
            auto tmp = cells[coord.x + coord.y * out_size.x];

            for(auto i: range(opts.size())) {
                opts[i] = to_index<bitmask_t>(tmp);
                auto lb = tmp & -tmp;
                tmp ^= lb;
            }

            DEBUG_VALUE_OF(opts);
            /**
             * select a random tile from the tileset that is *still available* for this cell.
             */
            auto opt = *random_from(opts.begin(), opts.end());
            DEBUG_VALUE_OF(opt);
            cells[coord.x + coord.y * out_size.x] = to_bitmask<bitmask_t>(opt);
        } 

        /**
         * popcount of cell at this point = 1
         */
        auto current_cell_index = to_index<bitmask_t>(cells[coord.x + coord.y * out_size.x]);
        /**
         * next, we reduce the neighbors.
         */
        for(int i=0; i < kNumEdges; i++) {
            int2 p { 
                (coord.x + neighbors[i].x + out_size.x) % out_size.x, 
                (coord.y + neighbors[i].y + out_size.y) % out_size.y
            };
            auto& neighbor = cells[p.x + p.y * out_size.x];
            if(__popcount(neighbor) > 1) { // << should this be here? it feels a tad hacky...
                neighbor &= adjacency[current_cell_index * 4 + i];
                //if(p.x >= 0 && p.x < out_size.x && p.y >= 0 && p.y < out_size.y) {
                    open.insert(std::make_pair(__popcount(neighbor), p));//push_back(p);
                //}
            }
        }
    } while(!open.empty());

    std::vector<int> temp(out_size.x * out_size.y, -1);

    for(int y: range(out_size.y)) {
        for(int x: range(out_size.x)) {
            int i = x + y * out_size.x;
            bitmask_t cb = cells[i];
            int index = to_index<bitmask_t>(cb);
            temp[i] = index_to_tile[index];

        }
    }
    std::copy(std::begin(temp), std::end(temp), out_map);
}
