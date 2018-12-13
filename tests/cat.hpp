

template <typename T>
struct ndarray {
    typedef std::vector<T> value_type;
    value_type data;
    typedef std::array<size_t, D0> index_type;
    index_type shape;
};

/*
 * The individual units the generation algorithm works with. 
 * Tiles wrap a value of any type, but they are usually an integer 
 * index into a tileset, or a Color when working with bitmaps. 
 * The value isn't important, all relevant information about a 
 * tile is stored externally.
 */
struct tile {
};

struct wave {
};

struct propagator {
    virtual int clear() = 0;
    virtual int ban() = 0;
    virtual int select() = 0;
    virtual int step() = 0;
    virtual int run() = 0;
};

struct basic_tile_constraint {
    virtual void init(propagator*) = 0;
    virtual void check(propagator*) = 0;
};

/*
 * Specifies an area or volume of space and how to navigate it.
 */
template <size_t D0>
struct topology {
    typedef std::array<size_t, D0> index_type;
    typedef std::array<bool, D0> periodicity_type;
    // The amount of dimensions
    index_type          _shape;
    // The periodicity in N dimensional space
    periodicity_type    _periodic; 
    std::vector<bool>   _mask;

    topology(const index_type& sizes) {
        size_t size = std::accumulate(std::begin(sizes),
            std::end(sizes),
            1,
            std::multiplies<size_t>()
        );
        _mask.resize(size, true);
    }

    size_t index_of(const index_type& ix) {
        std::vector<size_t> m = { 1 };
        std::partial_sum(std::begin(_shape),
            std::end(_shape) - 1,
            std::back_inserter(m),
            std::multiplies<size_t>()
        );
        return std::inner_product(std::begin(n), 
            std::end(n),
            std::begin(m),
            0
        );
    }

    bool try_move(const index_type& ix, index_type* out) {
        assert(sizeof(ix) == sizeof(_shape));
        auto coord = ix;
        for(size_t i=0; i < ix.size(); i++) {
            coord[i] += directions[i];
        }

        for(size_t i=0; i < ix.size(); i++) {
            if(_periodic[i]) {
                if(coord[i] < 0) coord[i]           += _shape[i];
                if(coord[i] >= _shape[i]) coord[i]  -= _shape[i]
            } else {
                if(coord[i] < 0 || coord[i] > _shape[i]) {
                    //invalidate_index(out);
                    ::memset(out, -1, sizeof(index_type));
                    return false;
                }
            }
        }
        //memcpy(out, &coord, sizeof(index_type));

        return _mask[index_of(coord)];
        
        //return true;
    }
};



struct model {
};

struct adjacent_model : public model {
};

struct overlapping_model : public model {
};

struct tile_model : public model {
};

int main(int, char**) {
    return 0;
}
