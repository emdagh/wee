#pragma once


#include <algorithm>
#include <iterator>
#include <iostream>
#include <tuple>

namespace wee {


    template<typename T>
        class zip_impl
        {

            public:

                typedef std::vector<T> container_t;

                template<typename... Args>
                    zip_impl(const T& head, const Args&... args)
                    : items_(head.size()),
                    min_(head.size())
            {
                zip_(head, args...);
            }

                inline operator container_t() const
                {
                    return items_;
                }

                inline container_t operator()() const
                {
                    return items_;
                }

            private:

                template<typename... Args>
                    void zip_(const T& head, const Args&... tail)
                    {
                        // If the current item's size is less than
                        //         // the one stored in min_, reset the min_
                        //                 // variable to the item's size
                        if (head.size() < min_) min_ = head.size();

                        for (std::size_t i = 0; i < min_; ++i)
                        {
                            // Use begin iterator and advance it instead
                            // of using the subscript operator adds support
                            // for lists. std::advance has constant complexity
                            // for STL containers whose iterators are
                            // RandomAccessIterators (e.g. vector or deque)
                            typename T::const_iterator itr = head.begin();

                            std::advance(itr, i);

                            items_[i].push_back(*itr);
                        }

                        // Recursive call to zip_(T, Args...)
                        // while the expansion of tail... is not empty
                        // else calls the overload with no parameters
                        return zip_(tail...);
                    }

                inline void zip_()
                {
                    // If min_ has shrunk since the
                    // constructor call
                    items_.resize(min_);
                }

                /*! Holds the items for iterating. */
                container_t items_;

                /*! The minimum number of values held by all items */
                std::size_t min_;

        };

    template<typename T, typename... Args>
        typename zip_impl<T>::container_t zip(const T& head, const Args&... tail)
        {
            return std::tie(zip_impl<T>(head, tail...));
        }

}
