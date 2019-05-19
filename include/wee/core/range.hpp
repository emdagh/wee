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
    class range_impl {
        T _begin, _end;
        public:
        class iterator : public input_iterator<T>::type { // < an example why `public` can be important....
            //friend class range_impl;
            T i_;
            public:

            iterator(T i) : i_ (i) { }   	
            T operator *() const { return i_; }
            const iterator &operator ++() { ++i_; return *this; }
            T operator - (const iterator& rhs) {
                return i_ - rhs.i_;
            }
            iterator operator ++(int) { iterator copy(*this); ++i_; return copy; }

            bool operator ==(const iterator &other) const { return i_ == other.i_; }
            bool operator !=(const iterator &other) const { return i_ != other.i_; }
        };

        range_impl(T a, T b) : _begin(a),_end(b) {}
        iterator begin() const { return iterator(_begin); }
        iterator end() const { return iterator(_end); }

    };

    template <typename T>
    range_impl<T> range(T a, T b) {
        return range_impl<T>(a, b);
    }



    template <typename T>
    range_impl<T> range(T a) {
        return range_impl<T>(static_cast<T>(0), a);
    }
}
