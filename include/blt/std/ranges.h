#pragma once
/*
 * Created by Brett on 06/02/24.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_RANGES_H
#define BLT_RANGES_H

#include <blt/std/types.h>
#include <type_traits>
#include <iterator>
#include <utility>
#include <limits>

namespace blt
{
    template<typename TYPE_ITR>
    class enumerator
    {
        public:
            class iterator
            {
                public:
                    using iterator_category = std::input_iterator_tag;
                    using value_type = typename TYPE_ITR::value_type;
                    using difference_type = typename TYPE_ITR::difference_type;
                    using pointer = typename TYPE_ITR::pointer;
                    using reference = typename TYPE_ITR::reference;
                private:
                    blt::size_t index = 0;
                    TYPE_ITR current;
                public:
                    explicit iterator(TYPE_ITR current): current(std::move(current))
                    {}
                    
                    iterator& operator++()
                    {
                        ++index;
                        ++current;
                        return *this;
                    }
                    
                    bool operator==(iterator other) const
                    {
                        return current == other.current;
                    }
                    
                    bool operator!=(iterator other) const
                    {
                        return current != other.current;
                    }
                    
                    std::pair<blt::size_t, const reference> operator*() const
                    {
                        return {index, *current};
                    };
                    
                    std::pair<blt::size_t, reference> operator*()
                    {
                        return {index, *current};
                    };
            };
            
            explicit enumerator(TYPE_ITR begin, TYPE_ITR end): begin_(std::move(begin)), end_(std::move(end))
            {}
            
            iterator begin()
            {
                return begin_;
            }
            
            iterator end()
            {
                return end_;
            }
        
        private:
            iterator begin_;
            iterator end_;
    };
    
    template<typename T>
    static inline auto enumerate(const T& container)
    {
        return enumerator{container.begin(), container.end()};
    }
    
    template<typename T>
    static inline auto enumerate(T& container)
    {
        return enumerator{container.begin(), container.end()};
    }
    
    template<typename T>
    static inline auto enumerate(T&& container)
    {
        return enumerator{container.begin(), container.end()};
    }
    
    template<typename T>
    struct range
    {
        public:
            struct range_itr
            {
                public:
                    using iterator_category = std::bidirectional_iterator_tag;
                    using difference_type = T;
                    using value_type = T;
                    using pointer = T*;
                    using reference = T&;
                private:
                    T current;
                    bool forward;
                public:
                    
                    explicit range_itr(T current, bool forward): current(current), forward(forward)
                    {}
                    
                    value_type operator*() const
                    { return current; }
                    
                    value_type operator->()
                    { return current; }
                    
                    range_itr& operator++()
                    {
                        if (forward)
                            current++;
                        else
                            current--;
                        return *this;
                    }
                    
                    range_itr& operator--()
                    {
                        if (forward)
                            current--;
                        else
                            current++;
                        return *this;
                    }
                    
                    friend bool operator==(const range_itr& a, const range_itr& b)
                    {
                        return a.current == b.current;
                    }
                    
                    friend bool operator!=(const range_itr& a, const range_itr& b)
                    {
                        return a.current != b.current;
                    }
            };
        
        private:
            T _begin;
            T _end;
            T offset = 0;
        public:
            range(T begin, T end): _begin(begin), _end(end), offset(end < begin ? 1 : 0)
            {}
            
            range_itr begin()
            {
                return range_itr(_begin - offset, offset == 0);
            }
            
            range_itr end()
            {
                // not sure if i like this
                return range_itr(_end - offset, offset == 0);
            }
    };
    
    template<typename I>
    class itr_offset
    {
        private:
            I begin_;
            I end_;
        public:
            template<typename T>
            itr_offset(I begin, I end, T offset): begin_(begin), end_(end)
            {
                for (T t = 0; t < offset; t++)
                    ++begin_;
            }
            
            template<typename C, typename T>
            itr_offset(C& container, T offset): begin_(container.begin()), end_(container.end())
            {
                for (T t = 0; t < offset; t++)
                    ++begin_;
            }
            
            auto begin()
            {
                return begin_;
            }
            
            auto end()
            {
                return end_;
            }
    };

    template<typename C, typename T>
    itr_offset(C, T) -> itr_offset<typename C::iterator>;
    
    
    inline constexpr std::size_t dynamic_extent = std::numeric_limits<std::size_t>::max();
    
    template<typename T, std::size_t extend = dynamic_extent>
    class span
    {
        public:
            using element_type = T;
            using value_type = std::remove_cv_t<T>;
            using size_type = blt::size_t;
            using difference_type = std::ptrdiff_t;
            using pointer = T*;
            using const_pointer = const T*;
            using reference = T&;
            using const_reference = const T&;
        
        private:
        
        public:
    };
}

#endif //BLT_RANGES_H
