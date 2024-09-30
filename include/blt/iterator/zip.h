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

#include <blt/iterator/common.h>
#include <blt/meta/meta.h>
#include <blt/meta/iterator.h>
#include <tuple>

namespace blt
{
    namespace iterator
    {
        template<typename... Iter>
        struct zip_wrapper : public base_wrapper<zip_wrapper<Iter...>>
        {
            public:
                using iterator_category = meta::lowest_iterator_category_t<Iter...>;
                using value_type = std::tuple<meta::deref_return_t<Iter>...>;
                using difference_type = blt::ptrdiff_t;
                using pointer = value_type;
                using reference = value_type;
                
                explicit zip_wrapper(std::tuple<Iter...> iter): iter(std::move(iter))
                {}
                
                explicit zip_wrapper(Iter... iter): iter(std::make_tuple(iter...))
                {}
                
                std::tuple<meta::deref_return_t<Iter>...> operator*() const
                {
                    return std::apply([](auto& ... i) { return std::tuple<meta::deref_return_t<Iter>...>{*i...}; }, iter);
                }
                
                zip_wrapper& operator++()
                {
                    std::apply([](auto& ... i) { ((++i), ...); }, iter);
                    return *this;
                }
                
                zip_wrapper& operator--()
                {
                    std::apply([](auto& ... i) { ((--i), ...); }, this->iter);
                    return *this;
                }
                
                friend zip_wrapper operator+(const zip_wrapper& a, blt::ptrdiff_t n)
                {
                    static_assert(std::is_same_v<iterator_category, std::random_access_iterator_tag>,
                                  "Iterator must allow random access");
                    return std::apply([n](auto& ... i) { return zip_wrapper((i + n)...); }, a.iter);
                }
                
                friend zip_wrapper operator-(const zip_wrapper& a, blt::ptrdiff_t n)
                {
                    static_assert(std::is_same_v<iterator_category, std::random_access_iterator_tag>,
                                  "Iterator must allow random access");
                    return std::apply([n](auto& ... i) { return zip_wrapper((i - n)...); }, a.iter);
                }
                
                friend blt::ptrdiff_t operator-(const zip_wrapper& a, const zip_wrapper& b)
                {
                    return sub(a, b, std::index_sequence_for<Iter...>());
                }
                
                auto base() const
                {
                    return iter;
                }
            
            protected:
                std::tuple<Iter...> iter;
                
                template<typename T, T... n>
                static blt::ptrdiff_t sub(const zip_wrapper& a, const zip_wrapper& b,
                                          std::integer_sequence<T, n...>)
                {
                    blt::ptrdiff_t min = std::numeric_limits<blt::ptrdiff_t>::max();
                    ((min = std::min(min, std::get<n>(a.iter) - std::get<n>(b.iter))), ...);
                    return min;
                }
        };
    }
    
    template<typename Iter>
    struct iterator_pair
    {
        using type = Iter;
        
        iterator_pair(Iter begin, Iter end): begin(std::move(begin)), end(std::move(end))
        {}
        
        Iter begin;
        Iter end;
    };
    
    template<typename... Iter>
    class zip_iterator_storage : public iterator::iterator_container<iterator::zip_wrapper<Iter...>>
    {
        public:
            using iterator::iterator_container<iterator::zip_wrapper<Iter...>>::iterator_container;
            
            explicit zip_iterator_storage(iterator_pair<Iter>... iterator_pairs):
                    iterator::iterator_container<iterator::zip_wrapper<Iter...>>(iterator::zip_wrapper<Iter...>{std::move(iterator_pairs.begin)...},
                                                                                 iterator::zip_wrapper<Iter...>{std::move(iterator_pairs.end)...})
            {}
    };
    
    /*
     * CTAD for the zip containers
     */
    
    template<typename... Iter>
    zip_iterator_storage(iterator_pair<Iter>...) -> zip_iterator_storage<Iter...>;
    
    template<typename... Iter>
    zip_iterator_storage(std::initializer_list<Iter>...) -> zip_iterator_storage<Iter...>;
    
    
    /*
     * Helper functions for creating zip containers
     */
    
    template<typename... Container>
    auto zip(Container& ... container)
    {
        return zip_iterator_storage{iterator_pair{container.begin(), container.end()}...};
    }
}

#endif //BLT_ITERATOR_ZIP
