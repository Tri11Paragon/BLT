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
                
                auto base()
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
        
        template<typename Iter, bool checked>
        struct skip_wrapper : public passthrough_wrapper<Iter, skip_wrapper<Iter, checked>>
        {
            public:
                using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
                using value_type = typename std::iterator_traits<Iter>::value_type;
                using difference_type = typename std::iterator_traits<Iter>::difference_type;
                using pointer = typename std::iterator_traits<Iter>::pointer;
                using reference = typename std::iterator_traits<Iter>::reference;
                
                explicit skip_wrapper(Iter iter, blt::size_t n): passthrough_wrapper<Iter, skip_wrapper<Iter, checked>>(std::move(iter)), skip(n)
                {}
                
                skip_wrapper& operator++()
                {
                    if constexpr (std::is_same_v<iterator_category, std::random_access_iterator_tag>)
                    {
                        if (skip > 0)
                        {
                            this->base() += skip;
                            skip = 0;
                        }
                    } else
                    {
                        while (skip > 0)
                        {
                            ++this->base();
                            --skip;
                        }
                    }
                    
                    ++this->base();
                    return *this;
                }
                
                skip_wrapper& operator--()
                {
                    if constexpr (std::is_same_v<iterator_category, std::random_access_iterator_tag>)
                    {
                        if (skip > 0)
                        {
                            this->base() -= skip;
                            skip = 0;
                        }
                    } else
                    {
                        while (skip > 0)
                        {
                            --this->base();
                            --skip;
                        }
                    }
                    --this->base();
                    return *this;
                }
                
                friend skip_wrapper operator+(const skip_wrapper& a, blt::ptrdiff_t n)
                {
                    static_assert(std::is_same_v<iterator_category, std::random_access_iterator_tag>,
                                  "Iterator must allow random access");
                    return {a.base() + static_cast<blt::ptrdiff_t>(a.skip) + n, 0};
                }
                
                friend skip_wrapper operator-(const skip_wrapper& a, blt::ptrdiff_t n)
                {
                    static_assert(std::is_same_v<iterator_category, std::random_access_iterator_tag>,
                                  "Iterator must allow random access");
                    return {a.base() - static_cast<blt::ptrdiff_t>(a.skip) - n, 0};
                }
            
            private:
                blt::size_t skip;
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
    class zip_iterator_storage;
    
    template<typename... Iter>
    class zip_iterator_storage_rev;
    
    template<typename... Iter>
    class zip_iterator_storage
    {
        public:
            using iterator_category = meta::lowest_iterator_category_t<Iter...>;
        public:
            zip_iterator_storage(iterator_pair<Iter>... iterator_pairs):
                    m_begins(std::move(iterator_pairs.begin)...), m_ends(std::move(iterator_pairs.end)...)
            {}
            
            zip_iterator_storage(iterator::zip_wrapper<Iter...> begins, iterator::zip_wrapper<Iter...> ends):
                    m_begins(std::move(begins)), m_ends(std::move(ends))
            {}
            
            auto rev()
            {
                static_assert((std::is_same_v<iterator_category, std::bidirectional_iterator_tag> ||
                               std::is_same_v<iterator_category, std::random_access_iterator_tag>),
                              ".rev() must be used with bidirectional (or better) iterators!");
                return zip_iterator_storage_rev{m_ends, m_begins};
            }
            
            auto skip(blt::size_t n)
            {
            
            }
            
            auto begin() const
            {
                return m_begins;
            }
            
            auto end() const
            {
                return m_ends;
            }
        
        private:
            iterator::zip_wrapper<Iter...> m_begins;
            iterator::zip_wrapper<Iter...> m_ends;
    };
    
    template<typename... Iter>
    class zip_iterator_storage_rev
    {
        public:
            using iterator_category = meta::lowest_iterator_category_t<Iter...>;
        public:
            zip_iterator_storage_rev(iterator_pair<Iter>... iterator_pairs): m_begins(iterator_pairs.begin...), m_ends(iterator_pairs.end...)
            {
                static_assert((std::is_same_v<iterator_category, std::bidirectional_iterator_tag> ||
                               std::is_same_v<iterator_category, std::random_access_iterator_tag>),
                              "reverse iteration is only supported on bidirectional or better iterators!");
            }
            
            zip_iterator_storage_rev(iterator::zip_wrapper<Iter...> begins,
                                     iterator::zip_wrapper<Iter...> ends): m_begins(std::move(begins)), m_ends(std::move(ends))
            {
                static_assert((std::is_same_v<iterator_category, std::bidirectional_iterator_tag> ||
                               std::is_same_v<iterator_category, std::random_access_iterator_tag>),
                              "reverse iteration is only supported on bidirectional or better iterators!");
            }
            
            auto rev()
            {
                return zip_iterator_storage{m_ends.base(), m_begins.base()};
            }
            
            auto begin() const
            {
                return m_begins;
            }
            
            auto end() const
            {
                return m_ends;
            }
        
        private:
            std::reverse_iterator<iterator::zip_wrapper<Iter...>> m_begins;
            std::reverse_iterator<iterator::zip_wrapper<Iter...>> m_ends;
    };
    
    /*
     * CTAD for the zip containers
     */
    
    template<typename... Iter>
    zip_iterator_storage(iterator_pair<Iter>...) -> zip_iterator_storage<Iter...>;
    
    template<typename... Iter>
    zip_iterator_storage(std::initializer_list<Iter>...) -> zip_iterator_storage<Iter...>;
    
    template<typename... Iter>
    zip_iterator_storage_rev(iterator_pair<Iter>...) -> zip_iterator_storage_rev<Iter...>;
    
    template<typename... Iter>
    zip_iterator_storage_rev(std::initializer_list<Iter>...) -> zip_iterator_storage_rev<Iter...>;
    
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
