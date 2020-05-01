
#pragma once

namespace csv {
    //Read the last element of the tuple without calling recursively
    template <std::size_t idx, class... T>
        typename std::enable_if<idx >= std::tuple_size<std::tuple<T...>>::value - 1>::type
        read_tuple(std::istream &in, std::tuple<T...> &out, const char delimiter) {
            std::string cell;
            std::getline(in, cell, delimiter);
            std::stringstream cell_stream(cell);
            cell_stream >> std::get<idx>(out);
        }

    // Read the @p idx-th element of the tuple and then calls itself with @p idx + 1 to
    /// read the next element of the tuple. Automatically falls in the previous case when
    /// reaches the last element of the tuple thanks to enable_if
    template <std::size_t idx, class... T>
        typename std::enable_if<idx < std::tuple_size<std::tuple<T...>>::value - 1>::type
        read_tuple(std::istream &in, std::tuple<T...> &out, const char delimiter) {
            std::string cell;
            std::getline(in, cell, delimiter);
            std::stringstream cell_stream(cell);
            cell_stream >> std::get<idx>(out);
            read_tuple<idx + 1, T...>(in, out, delimiter);
        }

    // Iterable csv wrapper around a stream. @p T the list of types that form up a row.
    template <typename... T>
        class reader {
            std::istream &_in;
            const char _delim;
            public:
            typedef std::tuple<T...> value_type;
            class iterator;

            // Construct from a stream.
            inline reader(std::istream &in, const char delim=',') : _in(in), _delim(delim) {}

            // Status of the underlying stream
            // @{
            inline bool good() const {
                return _in.good();
            }
            inline const std::istream &underlying_stream() const {
                return _in;
            }
            // @}

            inline iterator begin();
            inline iterator end();
            private:

            // Reads a line into a stringstream, and then reads the line into a tuple, that is returned
            inline value_type read_row() {
                std::string line;
                std::getline(_in, line);
                std::stringstream line_stream(line);
                std::tuple<T...> retval;
                csv::read_tuple<0, T...>(line_stream, retval, _delim);
                return retval;
            }
        };

    // Iterator; just calls recursively @ref csv::read_row and stores the result.
    template <class... T>
        class reader<T...>::iterator {
            reader::value_type _row;
            reader *_parent;
            public:
            typedef std::input_iterator_tag iterator_category;
            typedef reader::value_type      value_type;
            typedef std::size_t             difference_type;
            typedef reader::value_type *    pointer;
            typedef reader::value_type &    reference;

            // Construct an empty/end iterator
            inline iterator() : _parent(nullptr) {}
            // Construct an iterator at the beginning of the @p parent csv object.
            inline iterator(reader &parent) : _parent(parent.good() ? &parent : nullptr) {
                ++(*this);
            }

            // Read one row, if possible. Set to end if parent is not good anymore.
            inline iterator &operator++() {
                if (_parent != nullptr) {
                    _row = _parent->read_row();
                    if (!_parent->good()) {
                        _parent = nullptr;
                    }
                }
                return *this;
            }

            inline iterator operator++(int) {
                iterator copy = *this;
                ++(*this);
                return copy;
            }

            inline reader::value_type const &operator*() const {
                return _row;
            }

            inline reader::value_type const
                *operator->() const {
                    return &_row;
                }

            bool operator==(iterator const &other) {
                return (this == &other) or (_parent == nullptr and other._parent == nullptr);
            }

            bool operator!=(iterator const &other) {
                return not (*this == other);
            }
        };

    template <class... T>
    typename reader<T...>::iterator reader<T...>::begin()
    {
        return iterator(*this);
    }

    template <class... T> typename reader<T...>::iterator reader<T...>::end()
    {
        return iterator();
    }
}
