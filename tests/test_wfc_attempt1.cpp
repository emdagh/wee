#include <list>
#include <vector>
#include <bitset>
#include <array>
#include <cassert>
#include <string.h>
#include <iostream>
#include <core/range.hpp>
#include <random>

#ifdef _WIN32
#define popcount(x) __popcnt(x)
#else
#define popcount(x) __builtin_popcount(x)
#endif

#define PopCount(x) popcount(x)

struct int2 {
    int x, y;

    int2() : int2(0, 0) {}

    int2(int x, int y) : x(x), y(y) {}

	int2 operator + (const int2& other) {
        int2 copy(*this);
        copy += other;
        return copy;
    }

    int2& operator += (const int2& other) {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    int2 operator % (const int2& other) {
        int2 copy(*this);
        copy %= other;
        return copy;
    }

    int2& operator %= (const int2& other) {
        this->x = this->x % other.x;
        this->y = this->y % other.y;
        return *this;
    }
};


using wee::range;

template <typename T>
struct RandomNumberGenerator {
    std::random_device rd;
    int seed;
    RandomNumberGenerator(int seed=0x1337) : seed(seed) {
        srand(seed);
    }

	T operator () (const T& val) {
#if 0
        float fNormalized = (float)rand() / RAND_MAX;
#else
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0f, 1.0f);
        float fNormalized = dis(gen);
#endif
        float fRand = fNormalized * (float)val;
	    return static_cast<T>(fRand);
	}
};

int2 rndivec2(RandomNumberGenerator<int>& rnd, const int2& val) {
    int2 res;
    res.x = rnd(val.x);
    res.y = rnd(val.y);
    return res;
}



template<typename T> 
bool WaveFunctionCollapse(const int2 &insize, const int **inmap,
                                               const int2 &outsize, 
                                               int **outmap,
                                               RandomNumberGenerator<T> &rnd,
                                               int &num_contradictions) {
    num_contradictions = 0;
    typedef uint64_t bitmask_t;
    const auto nbits = sizeof(bitmask_t) * 8;
    std::array<int, 256> tile_lookup;
    tile_lookup.fill(-1);
    struct Tile { bitmask_t sides[4] = {}; int freq = 0; int tidx = 0; };
    std::vector<Tile> tiles;
    int2 neighbors[] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };
    // Collect unique tiles and their frequency of occurrence.
    for (int iny = 0; iny < insize.y; iny++) {
        for (int inx = 0; inx < insize.x; inx++) {
            auto t = inmap[iny][inx];
            if (tile_lookup[t] < 0) {
                // We use a bitmask_t mask for valid neighbors.
                if (tiles.size() == nbits - 1) return false;
                tile_lookup[t] = (int)tiles.size();
                tiles.push_back(Tile());
            }
            auto &tile = tiles[tile_lookup[t]];
            tile.freq++;
            tile.tidx = t;
        }
    }
    // Construct valid neighbor bitmasks.
    auto to_bitmask = [](size_t idx) { return (bitmask_t)1 << idx; };
    for (int iny = 0; iny < insize.y; iny++) {
        for (int inx = 0; inx < insize.x; inx++) {
            auto t = inmap[iny][inx];
            auto &tile = tiles[tile_lookup[t]];
            int ni = 0;
            for (auto n : neighbors) {
                auto p = (n + int2(inx, iny) + insize) % insize;
                auto tn = inmap[p.y][p.x];
                assert(tile_lookup[tn] >= 0);
                tile.sides[ni] |= to_bitmask(tile_lookup[tn]);
                ni++;
            }
        }
    }
    size_t most_common_tile_id = 0;
    int most_common_tile_freq = 0;
    for (auto &tile : tiles) if (tile.freq > most_common_tile_freq) {
        most_common_tile_freq = tile.freq;
        most_common_tile_id = &tile - &tiles[0];
    }
    // Track an open list (much like A*) of next options, sorted by best candidate at the end.
    std::list<std::pair<int2, int>> open, temp;
    // Store a bitmask per output cell of remaining possible choices.
    auto max_bitmask = (1 << tiles.size()) - 1;
    enum class State : int { NEW, OPEN, CLOSED };
    struct Cell {
        bitmask_t wf;
        int popcnt = 0;
        State state = State::NEW;
        std::list<std::pair<int2, int>>::iterator it;
        Cell(bitmask_t wf, int popcnt) : wf(wf), popcnt(popcnt) {}
    };
    std::vector<std::vector<Cell>> cells(outsize.y, std::vector<Cell>(outsize.x, Cell(max_bitmask,(int)tiles.size())));
    //auto start = rndivec<int, 2>(rnd, outsize);
    auto start = rndivec2(rnd, outsize);
    open.push_back({ start, 0 });  // Start.
    auto &scell = cells[start.y][start.x];
    scell.state = State::OPEN;
    scell.it = open.begin();
    // Pick tiles until no more possible.
    while (!open.empty()) {
        // Simply picking the first list item results in the same chance of conflicts as
        // random picks over equal options, but it is assumed the latter could generate more
        // interesting maps.
        int num_candidates = 1;
        auto numopts_0 = cells[open.back().first.y][open.back().first.x].popcnt;
        for (auto it = ++open.rbegin(); it != open.rend(); ++it)
            if (numopts_0 == cells[it->first.y][it->first.x].popcnt &&
                open.back().second == it->second)
                num_candidates++;
            else
                break;
        auto candidate_i = rnd(num_candidates);
        auto candidate_it = --open.end();
        for (int i = 0; i < candidate_i; i++) --candidate_it;
        auto cur = candidate_it->first;
        temp.splice(temp.end(), open, candidate_it);
        auto &cell = cells[cur.y][cur.x];
        assert(cell.state == State::OPEN);
        cell.state = State::CLOSED;
        bool contradiction = !cell.popcnt;
        if (contradiction) {
            num_contradictions++;
            // Rather than failing right here, fill in the whole map as best as possible just in
            // case a map with bad tile neighbors is still useful to the caller.
            // As a heuristic lets just use the most common tile, as that will likely have the
            // most neighbor options.
            cell.wf = to_bitmask(most_common_tile_id);
            cell.popcnt = 1;
        }
        // From our options, pick one randomly, weighted by frequency of tile occurrence.
        // First find total frequency.
        int total_freq = 0;
        for (size_t i = 0; i < tiles.size(); i++)
            if (cell.wf & to_bitmask(i))
                total_freq += tiles[i].freq;
        auto freqpick = rnd(total_freq);
        // Now pick.
        size_t picked = 0;
        for (size_t i = 0; i < tiles.size(); i++) if (cell.wf & to_bitmask(i)) {
            picked = i;
            if ((freqpick -= tiles[i].freq) <= 0) break;
        }
        assert(freqpick <= 0);
        // Modify the picked tile.
        auto &tile = tiles[picked];
        outmap[cur.y][cur.x] = tile.tidx;
        cell.wf = to_bitmask(picked);  // Exactly one option remains.
        cell.popcnt = 1;
        // Now lets cycle thru neighbors, reduce their options (and maybe their neighbors options),
        // and add them to the open list for next pick.
        int ni = 0;
        for (auto n : neighbors) {
            auto p = (cur + n + outsize) % outsize;
            auto &ncell = cells[p.y][p.x];
            if (ncell.state != State::CLOSED) {
                ncell.wf &= tile.sides[ni]; // Reduce options.
                ncell.popcnt = PopCount(ncell.wf);
                int totalnnumopts = 0;
                if (!contradiction) {
                    // Hardcoded second level of neighbors of neighbors, to reduce chance of
                    // contradiction.
                    // Only do this when our current tile isn't a contradiction, to avoid
                    // artificially shrinking options.
                    int nni = 0;
                    for (auto nn : neighbors) {
                        auto pnn = (p + nn + outsize) % outsize;
                        auto &nncell = cells[pnn.y][pnn.x];
                        if (nncell.state != State::CLOSED) {
                            // Collect the superset of possible options. If we remove anything but
                            // these, we are guaranteed the direct neigbor always has a possible
                            //pick.
                            bitmask_t superopts = 0;
                            for (size_t i = 0; i < tiles.size(); i++)
                                if (ncell.wf & to_bitmask(i))
                                    superopts |= tiles[i].sides[nni];
                            nncell.wf &= superopts;
                            nncell.popcnt = PopCount(nncell.wf);
                        }
                        totalnnumopts += nncell.popcnt;
                        nni++;
                    }
                }
                if (ncell.state == State::OPEN) {
                    // Already in the open list, remove it for it to be re-added just in case
                    // its location is not optimal anymore.
                    totalnnumopts = std::min(totalnnumopts, ncell.it->second);
                    temp.splice(temp.end(), open, ncell.it);  // Avoid alloc.
                }
                // Insert this neighbor, sorted by lowest possibilities.
                // Use total possibilities of neighbors as a tie-breaker to avoid causing
                // contradictions by needless surrounding of tiles.
                std::list<std::pair<int2, int>>::iterator dit = open.begin();
                for (auto it = open.rbegin(); it != open.rend(); ++it) {
                    auto onumopts = cells[it->first.y][it->first.x].popcnt;
                    if (onumopts > ncell.popcnt ||
                        (onumopts == ncell.popcnt && it->second >= totalnnumopts)) {
                        dit = it.base();
                        break;
                    }
                }
                if (temp.empty()) temp.push_back({});
                open.splice(dit, temp, ncell.it = temp.begin());
                *ncell.it = { p, totalnnumopts };
                ncell.state = State::OPEN;
            }
            ni++;
        }
    }
    return true;
}

template <typename T, int N> char(&dim_helper(T(&)[N])) [N];
#define dim(x) (sizeof(dim_helper(x)))

int main(int, char**) {
    int tilemap[][4] = {
        { 0, 1, 2, 3},
        { 4, 5, 6, 7},
        { 8, 9,10,11},
        {12,13,14,15},
    };

    constexpr int nrows = dim(tilemap);

    int* tilemap_rows[nrows];
    for(auto i: range(nrows)) {
        tilemap_rows[i] = tilemap[i];
    }


    int error = 0;

    constexpr int kOutputSize = 16;
    int** result = new int*[kOutputSize];
    for(int i=0; i < kOutputSize; i++) {
        result[i] = new int[kOutputSize];
    }

    memset(*result, 0, sizeof(int) * kOutputSize * kOutputSize);

    //wfc((int2){4, 4}, tilemap_rows, (int2){kOutputSize, kOutputSize}, result, error) ;
    RandomNumberGenerator<int> gen;
    WaveFunctionCollapse<int>((int2){nrows, nrows}, (const int**)tilemap_rows, (int2){kOutputSize, kOutputSize}, result, gen, error);

    for(auto y : range(kOutputSize)) {
        for(auto x : range(kOutputSize)) {
            std::string repr;
            switch(result[y][x]) {
                case 0:  repr = "/"; break;
                case 1:  repr = "-"; break;
                case 2:  repr = "\\"; break;
                case 3:  repr = " "; break;
                case 4:  repr = "|"; break;
                case 5:  repr = "|"; break;
                case 6:  repr = "|"; break;
                case 7:  repr = "*"; break;
                case 8:  repr = "J"; break;
                case 9:  repr = "_"; break;
                case 10: repr = "L"; break;
                case 11: repr = ":"; break;
                case 12: repr = "_"; break;
                case 13: repr = "_"; break;
                case 14: repr = "_"; break;
                case 15: repr = ";"; break;
            }
            std::cout << repr; //result[x][y];
        }
        std::cout << std::endl;
    }
    std::cout << "conflicts: " << error << std::endl;

    return 0;
}
