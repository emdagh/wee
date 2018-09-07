/**
 * shamelessly copied (and modified) from 
 * http://www.goodliffe.net/circularbuffers
 *
 * my additions:
 *  + erase()
 *
 */



/******************************************************************************
 * $Id:  $
 * $Name: $
 *
 * Author: Pete Goodliffe
 *
 * ----------------------------------------------------------------------------
 * Copyright 2002 Pete Goodliffe All rights reserved.
 *
 * ----------------------------------------------------------------------------
 * Purpose: STL-style circular buffer
 *
 * ----------------------------------------------------------------------------
 * History: See source control system log.
 *
 *****************************************************************************/

#ifndef circular_array_H
#define circular_array_H

#include <exception>
#include <iterator>
#include <memory>
#include <iostream>

namespace wee {

    /******************************************************************************
     * Iterators
     *****************************************************************************/

    /**
     * Iterator type for the circular_array class.
     *
     * This one template class provides all variants of forward/reverse
     * const/non const iterators through plentiful template magic.
     *
     * You don't need to instantiate it directly, use the good public functions
     * availble in circular_array.
     */
    template <typename T,                                  //circular_array type
                                                           //(incl const)
              typename T_nonconst,                         //with any consts
              typename elem_type = typename T::value_type> //+ const for const iter
    class circular_array_iterator
    {
        public:

            typedef circular_array_iterator<T,T_nonconst,elem_type> self_type;

            typedef T                                   cbuf_type;
            typedef std::random_access_iterator_tag     iterator_category;
            typedef typename cbuf_type::value_type      value_type;
            typedef typename cbuf_type::size_type       size_type;
            typedef typename cbuf_type::pointer         pointer;
            typedef typename cbuf_type::const_pointer   const_pointer;
            typedef typename cbuf_type::reference       reference;
            typedef typename cbuf_type::const_reference const_reference;
            typedef typename cbuf_type::difference_type difference_type;

            circular_array_iterator(cbuf_type *b, size_type p)
                : buf_(b), pos_(p) {}

            // Converting a non-const iterator to a const iterator
            circular_array_iterator
               (const circular_array_iterator<T_nonconst, T_nonconst,
                                               typename T_nonconst::value_type>
                &other)
                : buf_(other.buf_), pos_(other.pos_) {}
            friend class circular_array_iterator<const T, T, const elem_type>;

            // Use compiler generated copy ctor, copy assignment operator and dtor

            elem_type &operator*()  { return (*buf_)[pos_]; }
            elem_type *operator->() { return &(operator*()); }

            self_type &operator++()
            {
                pos_ += 1;
                return *this;
            }
            self_type operator++(int)
            {
                self_type tmp(*this);
                ++(*this);
                return tmp;
            }

            self_type &operator--()
            {
                pos_ -= 1;
                return *this;
            }
            self_type operator--(int)
            {
                self_type tmp(*this);
                --(*this);
                return tmp;
            }

            self_type operator+(difference_type n) const
            {
                self_type tmp(*this);
                tmp.pos_ += n;
                return tmp;
            }
            self_type &operator+=(difference_type n)
            {
                pos_ += n;
                return *this;
            }

            self_type operator-(difference_type n) const
            {
                self_type tmp(*this);
                tmp.pos_ -= n;
                return tmp;
            }
            self_type &operator-=(difference_type n)
            {
                pos_ -= n;
                return *this;
            }

            difference_type operator-(const self_type &c) const
            {
                return pos_ - c.pos_;
            }

            bool operator==(const self_type &other) const
            {
                return pos_ == other.pos_ && buf_ == other.buf_;
            }
            bool operator!=(const self_type &other) const
            {
                return pos_ != other.pos_ && buf_ == other.buf_;
            }
            bool operator>(const self_type &other) const
            {
                return pos_ > other.pos_;
            }
            bool operator>=(const self_type &other) const
            {
                return pos_ >= other.pos_;
            }
            bool operator<(const self_type &other) const
            {
                return pos_ < other.pos_;
            }
            bool operator<=(const self_type &other) const
            {
                return pos_ <= other.pos_;
            }

        private:

            cbuf_type *buf_;
            size_type  pos_;
    };

    template <typename circular_array_iterator_t>
    circular_array_iterator_t operator+
        (const typename circular_array_iterator_t::difference_type &a,
         const circular_array_iterator_t                           &b)
    {
        return circular_array_iterator_t(a) + b;
    }

    template <typename circular_array_iterator_t>
    circular_array_iterator_t operator-
        (const typename circular_array_iterator_t::difference_type &a,
         const circular_array_iterator_t                           &b)
    {
        return circular_array_iterator_t(a) - b;
    }


    /******************************************************************************
     * circular_array
     *****************************************************************************/

    /**
     * This class provides a circular buffer in the STL style.
     *
     * You can add data to the end using the @ref push_back function, read data
     * using @ref front() and remove data using @ref pop_front().
     *
     * The class also provides random access through the @ref operator[]()
     * function and its random access iterator. Subscripting the array with
     * an invalid (out of range) index number leads to undefined results, both
     * for reading and writing.
     *
     * This class template accepts three template parameters:
     *   <li> T                            The type of object contained
     *   <li> Alloc                        Allocator type to use (in line with other
     *                                     STL containers).
     *
     * @short   STL style circule buffer
     * @author  Pete Goodliffe
     * @version 1.00
     */
    template <typename T,
              typename Alloc                        = std::allocator<T> >
    class circular_array
    {
        public:

            enum
            {
                version_major = 1,
                version_minor = 0
            };

            // Typedefs
            typedef circular_array<T, Alloc>         self_type;
            typedef Alloc                             allocator_type;
            typedef typename Alloc::value_type        value_type;
            typedef typename Alloc::pointer           pointer;
            typedef typename Alloc::const_pointer     const_pointer;
            typedef typename Alloc::reference         reference;
            typedef typename Alloc::const_reference   const_reference;
            typedef typename Alloc::size_type         size_type;
            typedef typename Alloc::difference_type   difference_type;

            typedef circular_array_iterator<self_type, self_type> iterator;
            typedef circular_array_iterator<const self_type, self_type, const value_type> const_iterator;
            typedef std::reverse_iterator<iterator>       reverse_iterator;
            typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

            // Lifetime
            enum { default_capacity = 128 };
            explicit circular_array(size_type capacity = default_capacity)
            : array_(alloc_.allocate(capacity))
            , capacity_(capacity)
            , head_(1)
            , tail_(0)
            , size_(0)
            {
            }
            circular_array(const circular_array &other)
            : array_(alloc_.allocate(other.capacity_)),
              capacity_(other.capacity_),
              head_(other.head_), tail_(other.tail_),
              size_(other.size_)
            {
                try
                {
                    assign_into(other.begin(), other.end());
                }
                catch (...)
                {
                    destroy_all_elements();
                    alloc_.deallocate(array_, capacity_);
                    throw;
                }
            }
            template <class InputIterator>
            circular_array(InputIterator from, InputIterator to)
            : array_(alloc_.allocate(1)), capacity_(1),
              head_(1), tail_(0), size_(0)
            {
                circular_array tmp;
                tmp.assign_into_reserving(from, to);
                swap(tmp);
            }
            ~circular_array()
            {
                destroy_all_elements();
                alloc_.deallocate(array_, capacity_);
            }
            circular_array &operator=(const self_type &other)
            {
                circular_array tmp(other);
                swap(tmp);
                return *this;
            }
            void swap(circular_array &other)
            {
                std::swap(array_,         other.array_);
                std::swap(capacity_,      other.capacity_);
                std::swap(head_,          other.head_);
                std::swap(tail_,          other.tail_);
                std::swap(size_, other.size_);
            }
            allocator_type get_allocator() const { return alloc_; }

            // Iterators
            iterator         begin()       { return iterator(this, 0); }
            iterator         end()         { return iterator(this, size()); }

            const_iterator   begin() const { return const_iterator(this, 0); }
            const_iterator   end() const   { return const_iterator(this, size()); }

            reverse_iterator rbegin()      { return reverse_iterator(end()); }
            reverse_iterator rend()        { return reverse_iterator(begin()); }

            const_reverse_iterator rbegin() const
            {
                return const_reverse_iterator(end());
            }
            const_reverse_iterator rend() const
            {
                return const_reverse_iterator(begin());
            }

            // Size
            size_type size() const        { return size_; }
            size_type capacity() const    { return capacity_; }
            bool      empty() const       { return !size_; }
            size_type max_size() const
            {
                return alloc_.max_size();
            }
            void reserve(size_type new_size)
            {
                if (capacity() < new_size)
                {
                    circular_array tmp(new_size);
                    tmp.assign_into(begin(), end());
                    swap(tmp);
                }
            }

            // Accessing
            reference       front()       {return array_[head_];}
            reference       back()        {return array_[tail_];}
            const_reference front() const {return array_[head_];}
            const_reference back() const  {return array_[tail_];}

            /**
             * my own addition
             */
            iterator erase(iterator start, iterator end) {
                for(auto it=start; it != end; ++it) {
                    std::swap(front(), *it);
                    pop_front();
                    //increment_head();
                }
                return size() > 0 ? iterator(this, head_) : iterator(this, size());
            }

            void push_back(const value_type &item)
            {
                size_type next = next_tail();
                if (size_ == capacity_)
                {
                    array_[next] = item;
                    increment_head();
                }
                else
                {
                    alloc_.construct(array_ + next, item);
                }
                increment_tail();
            }
            void pop_front()
            {
                size_type destroy_pos = head_;
                increment_head();
                alloc_.destroy(array_ + destroy_pos);
            }
            void clear()
            {
                for (size_type n = 0; n < size_; ++n)
                {
                    alloc_.destroy(array_ + index_to_subscript(n));
                }
                head_ = 1;
                tail_ = size_ = 0;
            }

            reference       operator[](size_type n)       {return at_unchecked(n);}
            const_reference operator[](size_type n) const {return at_unchecked(n);}

            reference       at(size_type n)               {return at_checked(n);}
            const_reference at(size_type n) const         {return at_checked(n);}

        private:

            reference at_unchecked(size_type index) const
            {
                return array_[index_to_subscript(index)];
            }

            reference at_checked(size_type index) const
            {
                if (size >= size_)
                {
                    throw std::out_of_range();
                }
                return at_unchecked(index);
            }

            // Rounds an unbounded to an index into array_
            size_type normalize(size_type n) const { return n % capacity_; }

            // Converts external index to an array subscript
            size_type index_to_subscript(size_type index) const
            {
                return normalize(index + head_);
            }

            void increment_tail()
            {
                ++size_;
                tail_ = next_tail();
            }

            size_type next_tail()
            {
                return (tail_+1 == capacity_) ? 0 : tail_+1;
            }

            void increment_head()
            {
                // precondition: !empty()
                ++head_;
                --size_;
                if (head_ == capacity_) head_ = 0;
            }

            template <typename f_iter>
            void assign_into(f_iter from, f_iter to)
            {
                if (size_) clear();
                while (from != to)
                {
                    push_back(*from);
                    ++from;
                }
            }

            template <typename f_iter>
            void assign_into_reserving(f_iter from, f_iter to)
            {
                if (size_) clear();
                while (from != to)
                {
                    if (size_ == capacity_)
                    {
                        reserve(static_cast<size_type>(capacity_ * 1.5));
                    }
                    push_back(*from);
                    ++from;
                }
            }

            void destroy_all_elements()
            {
                for (size_type n = 0; n < size_; ++n)
                {
                    alloc_.destroy(array_ + index_to_subscript(n));
                }
            }

            allocator_type  alloc_;
            value_type     *array_;
            size_type       capacity_;
            size_type       head_;
            size_type       tail_;
            size_type       size_;
    };

    template <typename T,
              typename Alloc>
    bool operator==(const circular_array<T, Alloc> &a,
                    const circular_array<T, Alloc> &b)
    {
        return a.size() == b.size() && std::equal(a.begin(), a.end(), b.begin());
    }

    template <typename T,
              typename Alloc>
    bool operator!=(const circular_array<T, Alloc> &a,
                    const circular_array<T, Alloc> &b)
    {
        return a.size() != b.size() || !std::equal(a.begin(), a.end(), b.begin());
    }

    template <typename T,
              typename Alloc>
    bool operator<(const circular_array<T, Alloc> &a,
                   const circular_array<T, Alloc> &b)
    {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
    }

    template <typename T, typename Alloc>
    std::ostream& operator << (std::ostream& os, const circular_array<T, Alloc>& buf) {
        std::copy(buf.begin(), buf.end(), std::ostream_iterator<T>(os, ","));
        return os;
    }
}
#endif
