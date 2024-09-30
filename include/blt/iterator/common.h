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
    struct base_wrapper
    {
        base_wrapper operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }
        
        base_wrapper operator--(int)
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
        
        friend base_wrapper operator+(blt::ptrdiff_t n, const base_wrapper& a)
        {
            return a + n;
        }
        
        friend bool operator<(const base_wrapper& a, const base_wrapper& b)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            return b - a > 0;
        }
        
        friend bool operator>(const base_wrapper& a, const base_wrapper& b)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            return b < a;
        }
        
        friend bool operator>=(const base_wrapper& a, base_wrapper& b)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            return !(a < b); // NOLINT
        }
        
        friend bool operator<=(const base_wrapper& a, const base_wrapper& b)
        {
            static_assert(std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>,
                          "Iterator must allow random access");
            return !(a > b); // NOLINT
        }
        
        friend bool operator==(const base_wrapper& a, const base_wrapper& b)
        {
            return a.base() == b.base();
        }
        
        friend bool operator!=(const base_wrapper& a, const base_wrapper& b)
        {
            return !(a.base() == b.base()); // NOLINT
        }
    };
    
    template<typename Iter, typename Derived>
    struct passthrough_wrapper : public base_wrapper<Derived>
    {
        public:
            explicit passthrough_wrapper(Iter iter): iter(std::move(iter))
            {}
            
            meta::deref_return_t<Iter> operator*() const
            {
                return *iter;
            }
            
            auto base() const
            {
                return iter;
            }
            
            friend blt::ptrdiff_t operator-(const passthrough_wrapper& a, const passthrough_wrapper& b)
            {
                return a.base() - b.base();
            }
        
        protected:
            Iter iter;
    };
}

#endif //BLT_ITERATOR_ITER_COMMON
