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

#ifndef BLT_ITERATOR_H
#define BLT_ITERATOR_H

#include <blt/std/types.h>
#include <blt/std/logging.h>
#include <blt/meta/meta.h>
#include <blt/meta/iterator.h>
#include <type_traits>
#include <iterator>

namespace blt
{
    
    // forward declare useful types
    template<typename Iter, typename = std::void_t<>>
    class enumerator;
    
    template<typename Iter, typename = std::void_t<>>
    class enumerator_rev;
    
    namespace iterator
    {
        template<typename Iter, typename = std::void_t<>>
        class enumerate_wrapper;
        
        // struct which is returned by the enumerator
        template<typename T>
        struct enumerate_item
        {
            blt::size_t index;
            T value;
        };
        
        template<typename Iter1, typename Iter2>
        class pair_iterator_base
        {
            public:
                explicit pair_iterator_base(Iter1 iter1, Iter2 iter2): m_iter1(std::move(iter1)), m_iter2(std::move(iter2))
                {}
                
                friend bool operator==(const pair_iterator_base& a, const pair_iterator_base& b)
                {
                    return a.m_iter1 == b.m_iter1;
                }
                
                friend bool operator!=(const pair_iterator_base& a, const pair_iterator_base& b)
                {
                    return a.m_iter1 != b.m_iter1;
                }
                
                auto iter1() const
                {
                    return m_iter1;
                }
                
                auto iter2() const
                {
                    return m_iter2;
                }
            
            protected:
                Iter1 m_iter1;
                Iter2 m_iter2;
        };
        
        template<typename Iter>
        class enumerate_iterator_base : public pair_iterator_base<Iter, blt::size_t>
        {
            public:
                explicit enumerate_iterator_base(Iter iter, blt::size_t place = 0):
                        pair_iterator_base<Iter, blt::size_t>(std::move(iter), place)
                {}
                
                enumerate_item<blt::meta::deref_return_t<Iter>> operator*() const
                {
                    return {this->m_iter2, *this->m_iter1};
                }
                
                auto base() const
                {
                    return this->iter1();
                }
                
                auto get_index() const
                {
                    return this->iter2();
                }
        };
        
        template<typename Iter>
        class enumerate_forward_iterator : public enumerate_iterator_base<Iter>
        {
            public:
                using enumerate_iterator_base<Iter>::enumerate_iterator_base;
                
                enumerate_forward_iterator& operator++()
                {
                    ++this->m_iter1;
                    ++this->m_iter2;
                    return *this;
                }
                
                enumerate_forward_iterator operator++(int)
                {
                    auto tmp = *this;
                    ++*this;
                    return tmp;
                }
        };
        
        template<typename Iter>
        class enumerate_bidirectional_iterator : public enumerate_forward_iterator<Iter>
        {
            public:
                using enumerate_forward_iterator<Iter>::enumerate_forward_iterator;
                
                enumerate_bidirectional_iterator& operator--()
                {
                    --this->m_iter1;
                    --this->m_iter2;
                    return *this;
                }
                
                enumerate_bidirectional_iterator operator--(int)
                {
                    auto tmp = *this;
                    --*this;
                    return tmp;
                }
        };
        
        template<typename Iter>
        class enumerate_wrapper<Iter, std::enable_if_t<blt::meta::is_forward_iterator_v<Iter>, std::void_t<std::forward_iterator_tag>>>
                : public enumerate_forward_iterator<Iter>
        {
            public:
                using iterator_category = std::forward_iterator_tag;
                using value_type = enumerate_item<blt::meta::deref_return_t<Iter>>;
                using difference_type = typename std::iterator_traits<Iter>::difference_type;
                using pointer = value_type;
                using reference = value_type;
                using iterator_type = Iter;
                
                using enumerate_forward_iterator<Iter>::enumerate_forward_iterator;
        };
        
        template<typename Iter>
        class enumerate_wrapper<Iter, std::enable_if_t<blt::meta::is_bidirectional_or_better_v<Iter>, std::void_t<std::bidirectional_iterator_tag>>>
                : public enumerate_bidirectional_iterator<Iter>
        {
            public:
                using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
                using value_type = enumerate_item<blt::meta::deref_return_t<Iter>>;
                using difference_type = typename std::iterator_traits<Iter>::difference_type;
                using pointer = value_type;
                using reference = value_type;
                using iterator_type = Iter;
                
                using enumerate_bidirectional_iterator<Iter>::enumerate_bidirectional_iterator;
        };
        
        template<typename Iter, typename IterWrapper>
        class enumerator_base
        {
            public:
                explicit enumerator_base(Iter begin, Iter end): begin_(std::move(begin)), end_(std::move(end))
                {}
                
                explicit enumerator_base(IterWrapper begin, IterWrapper end): begin_(std::move(begin)), end_(std::move(end))
                {}
                
                auto begin()
                {
                    return begin_;
                }
                
                auto end()
                {
                    return end_;
                }
            
            protected:
                IterWrapper begin_;
                IterWrapper end_;
        };
        
        template<typename Iter, typename IterWrapper>
        class enumerator_reversible : public enumerator_base<Iter, IterWrapper>
        {
            public:
                explicit enumerator_reversible(Iter begin, Iter end, blt::size_t container_size):
                        enumerator_base<Iter, IterWrapper>{IterWrapper{enumerate_wrapper<Iter>{std::move(begin), 0}},
                                                           IterWrapper{enumerate_wrapper<Iter>{std::move(end), container_size}}}
                {}
                
                explicit enumerator_reversible(Iter begin, Iter end, blt::size_t begin_index, blt::size_t end_index):
                        enumerator_base<Iter, IterWrapper>(IterWrapper{enumerate_wrapper<Iter>{std::move(begin), begin_index}},
                                                           IterWrapper{enumerate_wrapper<Iter>{std::move(end), end_index}})
                {}
                
                auto rev() const
                {
                    return enumerator_rev<Iter>{this->end_.base(),
                                                this->begin_.base(),
                                                this->end_.get_index(),
                                                this->begin_.get_index()};
                }
                
                auto skip(blt::size_t amount)
                {
                    auto begin = this->begin_;
                    for (blt::size_t i = 0; i < amount; i++)
                        ++begin;
                    return enumerator<Iter>{begin.base(),
                                            this->end_.base(),
                                            begin.get_index(),
                                            this->end_.get_index()};
                }
                
                auto take(blt::size_t amount)
                {
                    auto end = this->begin();
                    for (blt::size_t i = 0; i < amount; i++)
                        ++end;
                    return enumerator<Iter>{this->begin_.base(),
                                            end.base(),
                                            this->begin_.get_index(),
                                            end.get_index()};
                }
        };
        
        template<typename Iter, typename IterWrapper>
        class enumerator_reversible_rev : public enumerator_reversible<Iter, IterWrapper>
        {
            public:
                using enumerator_reversible<Iter, IterWrapper>::enumerator_reversible;
                
                auto rev() const
                {
                    return enumerator<Iter>{this->end_.base().base(),
                                            this->begin_.base().base(),
                                            this->end_.base().get_index(),
                                            this->begin_.base().get_index()};
                }
                
                auto skip(blt::size_t amount)
                {
                    auto begin = this->begin_.base();
                    for (blt::size_t i = 0; i < amount; i++)
                        --begin;
                    return enumerator_rev<Iter>{begin.base(),
                                                this->end_.base().base(),
                                                begin.get_index(),
                                                this->end_.base().get_index()};
                }
                
                auto take(blt::size_t amount)
                {
                    auto end = this->begin_.base();
                    for (blt::size_t i = 0; i < amount; i++)
                        --end;
                    return enumerator_rev<Iter>{
                            this->begin_.base().base(),
                            end.base(),
                            this->begin_.base().get_index(),
                            end.get_index()};
                }
        };
    }
    
    template<typename Iter>
    class enumerator<Iter, std::enable_if_t<blt::meta::is_forward_iterator_v<Iter>, std::void_t<std::forward_iterator_tag>>>
            : public iterator::enumerator_base<Iter, iterator::enumerate_wrapper<Iter>>
    {
        public:
            using iterator::enumerator_base<Iter, iterator::enumerate_wrapper<Iter>>::enumerator_base;
    };
    
    template<typename Iter>
    class enumerator<Iter, std::enable_if_t<blt::meta::is_bidirectional_iterator_v<Iter>, std::void_t<std::bidirectional_iterator_tag>>>
            : public iterator::enumerator_reversible<Iter, iterator::enumerate_wrapper<Iter>>
    {
        public:
            using iterator::enumerator_reversible<Iter, iterator::enumerate_wrapper<Iter>>::enumerator_reversible;
    };
    
    template<typename Iter>
    class enumerator<Iter, std::enable_if_t<blt::meta::is_random_access_iterator_v<Iter>, std::void_t<std::random_access_iterator_tag>>>
            : public iterator::enumerator_reversible<Iter, iterator::enumerate_wrapper<Iter>>
    {
        public:
            using iterator::enumerator_reversible<Iter, iterator::enumerate_wrapper<Iter>>::enumerator_reversible;

            auto skip(blt::size_t amount)
            {
                return enumerator<Iter>{this->begin_.base() + amount,
                                        this->end_.base(),
                                        this->begin_.get_index() + amount,
                                        this->end_.get_index()};
            }

            auto take(blt::size_t amount)
            {
                return enumerator<Iter>{this->begin_.base(),
                                        this->begin_.base() + amount,
                                        this->begin_.get_index(),
                                        this->begin_.get_index() + amount};
            }
    };
    
    template<typename Iter>
    class enumerator_rev<Iter, std::enable_if_t<blt::meta::is_bidirectional_iterator_v<Iter>, std::void_t<std::bidirectional_iterator_tag>>>
            : public iterator::enumerator_reversible_rev<Iter, std::reverse_iterator<iterator::enumerate_wrapper<Iter>>>
    {
        public:
            using iterator::enumerator_reversible_rev<Iter, std::reverse_iterator<iterator::enumerate_wrapper<Iter>>>::enumerator_reversible_rev;
    };
    
    template<typename Iter>
    class enumerator_rev<Iter, std::enable_if_t<blt::meta::is_random_access_iterator_v<Iter>, std::void_t<std::random_access_iterator_tag>>>
            : public iterator::enumerator_reversible_rev<Iter, std::reverse_iterator<iterator::enumerate_wrapper<Iter>>>
    {
        public:
            using iterator::enumerator_reversible_rev<Iter, std::reverse_iterator<iterator::enumerate_wrapper<Iter>>>::enumerator_reversible_rev;

            auto skip(blt::size_t amount)
            {
                return enumerator_rev<Iter>{this->begin_.base().base() - amount,
                                            this->end_.base().base(),
                                            this->begin_.base().get_index() - amount,
                                            this->end_.base().get_index()};
            }
            
            auto take(blt::size_t amount)
            {
                return enumerator_rev<Iter>{this->begin_.base().base(),
                                            this->begin_.base().base() - amount,
                                            this->begin_.base().get_index(),
                                            this->begin_.base().get_index() - amount};
            }
    };
    
    template<typename Iter>
    enumerator(Iter, Iter) -> enumerator<Iter>;
    
    template<typename Iter>
    enumerator(Iter, Iter, blt::size_t) -> enumerator<Iter>;
    
    template<typename Iter>
    enumerator(Iter, Iter, blt::size_t, blt::size_t) -> enumerator<Iter>;
    
    template<typename T>
    static inline auto enumerate(const T& container)
    {
        return enumerator{container.begin(), container.end(), container.size()};
    }
    
    template<typename T, blt::size_t size>
    static inline auto enumerate(const T(& container)[size])
    {
        return enumerator{&container[0], &container[size], size};
    }
    
    template<typename T, blt::size_t size>
    static inline auto enumerate(T(& container)[size])
    {
        return enumerator{&container[0], &container[size], size};
    }
    
    template<typename T>
    static inline auto enumerate(T& container)
    {
        return enumerator{container.begin(), container.end(), container.size()};
    }
    
    template<typename T>
    static inline auto enumerate(T&& container)
    {
        return enumerator{container.begin(), container.end(), container.size()};
    }
    
}

#endif //BLT_ITERATOR_H
