#pragma once

#include <algorithm>
#include <iterator>
#include <iostream>

namespace wee {

    template <typename T>
    struct input_iterator {
        typedef std::iterator<std::input_iterator_tag, T, T, T*, T&> type;

    };

    template <typename T>
    class range_class {
        T _begin, _end;
        public:
        class iterator : input_iterator<T>::type {
            //friend class range_class;
            T i_;
            public:
            iterator(T i) : i_ (i) { }   	
            T operator *() const { return i_; }
            const iterator &operator ++() { ++i_; return *this; }
            iterator operator ++(int) { iterator copy(*this); ++i_; return copy; }

            bool operator ==(const iterator &other) const { return i_ == other.i_; }
            bool operator !=(const iterator &other) const { return i_ != other.i_; }
        };



        range_class(T a, T b) : _begin(a),_end(b) {}

        iterator begin() const { return iterator(_begin); }
        iterator end() const { return iterator(_end); }
    };

    template <typename T>
    range_class<T> range(T a, T b) {
        return range_class<T>(a, b);
    }




    template <typename T>
    range_class<T> range(T a) {
        return range_class<T>(static_cast<T>(0), a);
    }
}
