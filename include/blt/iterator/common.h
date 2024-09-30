#pragma once
/*
 *  Copyright (C) 2024  Brett Terpstra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef BLT_ITERATOR_ITER_COMMON
#define BLT_ITERATOR_ITER_COMMON

#include <type_traits>
#include <iterator>

namespace blt::iterator
{
    template<typename Derived>
    struct wrapper_base
    {
        wrapper_base operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
        
        wrapper_base operator--(int)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::bidirectional_iterator_tag> ||
                          std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            auto tmp = *this;
            --*this;
            return tmp;
        }
        
        auto operator[](blt::ptrdiff_t n) const
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            return *(*this + n);
        }
        
        friend wrapper_base operator+(const wrapper_base& a, blt::ptrdiff_t n)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            auto copy = a;
            copy += n;
            return copy;
        }
        
        friend wrapper_base operator+(blt::ptrdiff_t n, const wrapper_base& a)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            auto copy = a;
            copy += n;
            return copy;
        }
        
        friend wrapper_base operator-(const wrapper_base& a, blt::ptrdiff_t n)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            auto copy = a;
            copy -= n;
            return copy;
        }
        
        friend wrapper_base operator-(blt::ptrdiff_t n, const wrapper_base& a)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            auto copy = a;
            copy -= n;
            return copy;
        }
        
        friend bool operator<(const wrapper_base& a, const wrapper_base& b)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            return b - a > 0;
        }
        
        friend bool operator>(const wrapper_base& a, const wrapper_base& b)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            return b < a;
        }
        
        friend bool operator>=(const wrapper_base& a, wrapper_base& b)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            return !(a < b); // NOLINT
        }
        
        friend bool operator<=(const wrapper_base& a, const wrapper_base& b)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            return !(a > b); // NOLINT
        }
        
        friend bool operator==(const wrapper_base& a, const wrapper_base& b)
        {
            return a.base() == b.base();
        }
        
        friend bool operator!=(const wrapper_base& a, const wrapper_base& b)
        {
            return !(a.base() == b.base()); // NOLINT
        }
    };
}

#endif //BLT_ITERATOR_ITER_COMMON
