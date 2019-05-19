#pragma once

#include <core/range.hpp>

namespace {
    using wee::range;
    template <typename T>
    using input_iterator = wee::input_iterator<T>;

    template <typename T, size_t N>
    class vectornd { // inheritance of stl containers is considered bad practice...
    public:
        typedef vectornd<T, N-1> sub_t;
        typedef std::array<int, N> shape_t;
    private:
        std::vector<T> _data;
        shape_t _shape; // _shape
    public:
        vectornd(size_t n) 
            : _data(std::vector<T>(0, n))
        { 
            _shape.fill(1); 
            _shape[0] = n;
        }

        template <typename InputIt>
        vectornd(InputIt first, InputIt last)  
        : _data({first, last})
        {
            _shape.fill(1);
            _shape[0] = last - first;
        }

        template <typename S>
        vectornd(const S& other) :
        vectornd(std::begin(other), std::end(other)) 
        {
        }

        void reshape(const shape_t& s) {            
            _shape = s;
        }

        template <typename... Ts>
        void reshape(Ts... args) {
            size_t t = (args * ...);
            assert(t == _data.size());
            reshape(shape_t { args... });
        }

        size_t linearize(const shape_t& pos) const {
            size_t index;
            for(auto i : range(N)) {
                assert(pos[i] >= 0 && pos[i] < _shape[i]);
                auto j =  N - i - 1;
                index = (i == 0) ? pos[j] : (index * _shape[j] + pos[j]);
            }
            return index;
        }

        decltype(auto) operator[] (const shape_t& pos) {
            return _data.at(linearize(pos));
        }

        size_t size(size_t i) const {
            return static_cast<size_t>(_shape[i]);
        }


        template <typename... Ts>
        decltype(auto) at(Ts... args) {
            return (*this)[shape_t { args... }];
        }

        constexpr const shape_t& shape() const {
            return _shape;
        }

        constexpr const std::vector<T>& data() const {
            return _data;
        }

        void iterate(size_t dim) {
            std::vector<int> remaining(N - 1);
            for(size_t i=0, j=0; i < N; i++) 
                if(i != dim) 
                    remaining[j++] = _shape[i];

            int len = std::accumulate(remaining.begin(), remaining.end(), 1, std::multiplies<int>());
            int step = _shape[dim];
            DEBUG_VALUE_OF(remaining);
            for(auto i=0; i < _shape[dim]; i++) {
                DEBUG_VALUE_OF(i);

                for(int j=0; j < len; j++) {
                    //int index = (j + i * _shape[i]);
                    //DEBUG_VALUE_OF(index);
                    DEBUG_VALUE_OF(j * step);
                }
            }

        }

        void slice(int start, const std::vector<int>& sizes , const std::vector<int>& strides) 
        {
            for(auto size: sizes) {
                for(auto j: range(size)) {
                    for(auto stride: strides) {
                        for(auto i: range(stride)) {
                            std::cout << i << "*" << j << "+";
                        }
                        std::cout << std::endl;
                                                
                    }
                }
            }
        }
    };

    template <typename T, size_t N>
    std::ostream& operator << (std::ostream& os, const vectornd<T, N>& v) {
        return os << "data=" << v.data() << ", shape=" << v.shape();
    }

    template <typename T>
    T subscript(size_t ix, const T& shape) {
        size_t index = ix;
        T res;
        auto N = shape.size();
        for(auto j: range(N)) {
            auto i = j;//N - j - 1;
            size_t s = index % shape[i];
            index -= s;
            index /= shape[i];
            res[i] = s;
        }
        return res;
    }

}
