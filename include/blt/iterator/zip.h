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

#ifndef BLT_ITERATOR_ZIP
#define BLT_ITERATOR_ZIP

#include <blt/iterator/iter_common.h>
#include <tuple>

namespace blt
{
    
    namespace iterator
    {
        template<typename Tag, typename... Iter>
        class zip_wrapper;
        
        template<typename Tag, typename... Iter>
        class zip_iterator_storage;
        
        template<typename Tag, typename... Iter>
        class zip_iterator_storage_rev;
        
        template<typename... Iter>
        class zip_wrapper<std::forward_iterator_tag, Iter...>
        {
            public:
                using iterator_category = std::forward_iterator_tag;
                using value_type = std::tuple<blt::meta::deref_return_t<Iter>...>;
                using difference_type = blt::ptrdiff_t;
                using pointer = value_type;
                using reference = value_type;
                
                explicit zip_wrapper(Iter... iter): iter(std::make_tuple(iter...))
                {}
                
                std::tuple<blt::meta::deref_return_t<Iter>...> operator*() const
                {
                    return std::apply([](auto& ... i) { return std::make_tuple(*i...); }, iter);
                }
                
                friend bool operator==(const zip_wrapper& a, const zip_wrapper& b)
                {
                    return a.iter == b.iter;
                }
                
                friend bool operator!=(const zip_wrapper& a, const zip_wrapper& b)
                {
                    return !(a.iter == b.iter);
                }
                
                zip_wrapper& operator++()
                {
                    std::apply([](auto& ... i) { ((++i), ...); }, iter);
                    return *this;
                }
                
                zip_wrapper operator++(int)
                {
                    auto tmp = *this;
                    ++*this;
                    return tmp;
                }
                
                auto base()
                {
                    return iter;
                }
            
            protected:
                std::tuple<Iter...> iter;
        };
        
        template<typename... Iter>
        class zip_wrapper<std::bidirectional_iterator_tag, Iter...> : public zip_wrapper<std::forward_iterator_tag, Iter...>
        {
            public:
                using zip_wrapper<std::forward_iterator_tag, Iter...>::zip_wrapper;
                
                using iterator_category = std::bidirectional_iterator_tag;
                using value_type = std::tuple<blt::meta::deref_return_t<Iter>...>;
                using difference_type = blt::ptrdiff_t;
                using pointer = value_type;
                using reference = value_type;
                
                zip_wrapper& operator--()
                {
                    std::apply([](auto& ... i) { ((--i), ...); }, this->iter);
                    return *this;
                }
                
                zip_wrapper operator--(int)
                {
                    auto tmp = *this;
                    --*this;
                    return tmp;
                }
        };
        
        template<typename... Iter>
        class zip_wrapper<std::random_access_iterator_tag, Iter...> : public zip_wrapper<std::bidirectional_iterator_tag, Iter...>
        {
            private:
                template<typename T, T... n>
                static blt::ptrdiff_t sub(const zip_wrapper& a, const zip_wrapper& b,
                                          std::integer_sequence<T, n...>)
                {
                    auto min = std::min(std::get<n>(a.iter) - std::get<n>(b.iter)...);
                    return min;
                }
            public:
                using zip_wrapper<std::bidirectional_iterator_tag, Iter...>::zip_wrapper;
                
                using iterator_category = std::random_access_iterator_tag;
                using value_type = std::tuple<blt::meta::deref_return_t<Iter>...>;
                using difference_type = blt::ptrdiff_t;
                using pointer = value_type;
                using reference = value_type;
                
                using zip_bidirectional_iterator<Iter...>::zip_bidirectional_iterator;
                
                zip_wrapper& operator+=(blt::ptrdiff_t n)
                {
                    std::apply([n](auto& ... i) { ((i += n), ...); }, this->iter);
                    return *this;
                }
                
                zip_wrapper& operator-=(blt::ptrdiff_t n)
                {
                    std::apply([n](auto& ... i) { ((i -= n), ...); }, this->iter);
                    return *this;
                }
                
                friend zip_wrapper operator+(const zip_wrapper& a, blt::ptrdiff_t n)
                {
                    return std::apply([n](auto& ... i) { return zip_random_access_iterator{(i + n)...}; }, a.iter);
                }
                
                friend zip_wrapper operator+(blt::ptrdiff_t n, const zip_wrapper& a)
                {
                    return std::apply([n](auto& ... i) { return zip_random_access_iterator{(i + n)...}; }, a.iter);
                }
                
                friend zip_wrapper operator-(const zip_wrapper& a, blt::ptrdiff_t n)
                {
                    return std::apply([n](auto& ... i) { return zip_random_access_iterator{(i - n)...}; }, a.iter);
                }
                
                friend zip_wrapper operator-(blt::ptrdiff_t n, const zip_wrapper& a)
                {
                    return std::apply([n](auto& ... i) { return zip_random_access_iterator{(i - n)...}; }, a.iter);
                }
                
                friend blt::ptrdiff_t operator-(const zip_wrapper& a, const zip_wrapper& b)
                {
                    return sub(a, b, std::index_sequence_for<Iter...>());
                }
                
                auto operator[](blt::ptrdiff_t n) const
                {
                    return *(*this + n);
                }
                
                friend bool operator<(const zip_wrapper& a, const zip_wrapper& b)
                {
                    return b - a > 0;
                }
                
                friend bool operator>(const zip_wrapper& a, const zip_wrapper& b)
                {
                    return b < a;
                }
                
                friend bool operator>=(const zip_wrapper& a, const zip_wrapper& b)
                {
                    return !(a < b); // NOLINT
                }
                
                friend bool operator<=(const zip_wrapper& a, const zip_wrapper& b)
                {
                    return !(a > b); // NOLINT
                }
        };
        
        template<typename... Iter>
        class zip_iterator_storage<std::forward_iterator_tag, Iter...>
        {
        
        };
        
        template<typename... Iter>
        class zip_iterator_storage<std::bidirectional_iterator_tag, Iter...>
        {
        
        };
        
        template<typename... Iter>
        class zip_iterator_storage<std::random_access_iterator_tag, Iter...>
        {
        
        };
        
        template<typename... Iter>
        class zip_iterator_storage_rev<std::forward_iterator_tag, Iter...>
        {
        
        };
        
        template<typename... Iter>
        class zip_iterator_storage_rev<std::bidirectional_iterator_tag, Iter...>
        {
        
        };
        
        template<typename... Iter>
        class zip_iterator_storage_rev<std::random_access_iterator_tag, Iter...>
        {
        
        };
    }
    
}

#endif //BLT_ITERATOR_ZIP
