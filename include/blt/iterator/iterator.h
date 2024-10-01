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
#include <blt/iterator/iter_common.h>
#include <blt/iterator/zip.h>
#include <blt/meta/meta.h>
#include <blt/meta/iterator.h>
#include <type_traits>
#include <iterator>
#include <tuple>

namespace blt
{
    
    // forward declare useful types
    template<typename Iter, typename = std::void_t<>>
    class enumerator;
    
    template<typename Iter, typename = std::void_t<>>
    class enumerator_rev;
    
    template<typename Iter1, typename Iter2, typename = std::void_t<>>
    class pair_iterator;
    
    template<typename Iter, typename Iter2, typename = std::void_t<>>
    class pair_iterator_rev;
    
    template<typename... Iter>
    class zip_iterator;
    
    template<typename... Iter>
    class zip_iterator_rev;
    
    namespace iterator
    {
        template<typename Iter, typename = std::void_t<>>
        class enumerate_wrapper;
        
        template<typename Iter1, typename Iter2, typename = std::void_t<>>
        class pair_wrapper;
        
        template<typename Tag, typename... Iter>
        class zip_wrapper;
        
        template<typename Tag, typename... Iter>
        class zip_iterator_storage;
        
        template<typename Tag, typename... Iter>
        class zip_iterator_storage_rev;
        
        template<typename... Iter>
        class zip_forward_iterator
        {
            public:
                explicit zip_forward_iterator(Iter... iter): iter(std::make_tuple(iter...))
                {}
                
                std::tuple<blt::meta::deref_return_t<Iter>...> operator*() const
                {
                    return std::apply([](auto& ... i) { return std::make_tuple(*i...); }, iter);
                }
                
                friend bool operator==(const zip_forward_iterator& a, const zip_forward_iterator& b)
                {
                    return a.iter == b.iter;
                }
                
                friend bool operator!=(const zip_forward_iterator& a, const zip_forward_iterator& b)
                {
                    return !(a.iter == b.iter);
                }
                
                zip_forward_iterator& operator++()
                {
                    std::apply([](auto& ... i) { ((++i), ...); }, iter);
                    return *this;
                }
                
                zip_forward_iterator operator++(int)
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
        class zip_bidirectional_iterator : public zip_forward_iterator<Iter...>
        {
            public:
                using zip_forward_iterator<Iter...>::zip_forward_iterator;
                
                zip_bidirectional_iterator& operator--()
                {
                    std::apply([](auto& ... i) { ((--i), ...); }, this->iter);
                    return *this;
                }
                
                zip_bidirectional_iterator operator--(int)
                {
                    auto tmp = *this;
                    --*this;
                    return tmp;
                }
        };
        
        template<typename... Iter>
        class zip_wrapper<std::forward_iterator_tag, Iter...> : public zip_forward_iterator<Iter...>
        {
            public:
                using zip_forward_iterator<Iter...>::zip_forward_iterator;
                
                using iterator_category = std::forward_iterator_tag;
                using value_type = std::tuple<blt::meta::deref_return_t<Iter>...>;
                using difference_type = blt::ptrdiff_t;
                using pointer = value_type;
                using reference = value_type;
        };
        
        template<typename... Iter>
        class zip_wrapper<std::bidirectional_iterator_tag, Iter...> : public zip_bidirectional_iterator<Iter...>
        {
            public:
                using zip_bidirectional_iterator<Iter...>::zip_bidirectional_iterator;
                
                using iterator_category = std::bidirectional_iterator_tag;
                using value_type = std::tuple<blt::meta::deref_return_t<Iter>...>;
                using difference_type = blt::ptrdiff_t;
                using pointer = value_type;
                using reference = value_type;
        };
        
        template<typename... Iter>
        class zip_wrapper<std::random_access_iterator_tag, Iter...> : public zip_bidirectional_iterator<Iter...>
        {
            public:
                using zip_bidirectional_iterator<Iter...>::zip_bidirectional_iterator;
                
                using iterator_category = std::bidirectional_iterator_tag;
                using value_type = std::tuple<blt::meta::deref_return_t<Iter>...>;
                using difference_type = blt::ptrdiff_t;
                using pointer = value_type;
                using reference = value_type;
        };
        
        /**
         * struct which is returned by the enumerator.
         * @tparam T type to store.
         */
        template<typename T>
        struct enumerate_item
        {
            blt::size_t index;
            T value;
        };
        
        /**
         * base class for iterators which operate on pairs of values. Handles comparison.
         * @tparam Iter1 first iterator type. this will be used for comparison.
         * @tparam Iter2 second iterator type. this value is not modified by this class.
         */
        template<typename Iter1, typename Iter2>
        class dual_iterator_base
        {
            public:
                explicit dual_iterator_base(Iter1 iter1, Iter2 iter2): m_iter1(std::move(iter1)), m_iter2(std::move(iter2))
                {}
                
                friend bool operator==(const dual_iterator_base& a, const dual_iterator_base& b)
                {
                    return a.m_iter1 == b.m_iter1;
                }
                
                friend bool operator!=(const dual_iterator_base& a, const dual_iterator_base& b)
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
        
        /**
         * Base class for all enumerator iterators. Handles the deference (*) operator.
         * @tparam Iter iterator type
         */
        template<typename Iter>
        class enumerate_iterator_base : public dual_iterator_base<Iter, blt::size_t>
        {
            public:
                explicit enumerate_iterator_base(Iter iter, blt::size_t place = 0):
                        dual_iterator_base<Iter, blt::size_t>(std::move(iter), place)
                {}
                
                enumerate_item<blt::meta::deref_return_t<Iter>> operator*() const
                {
                    return {this->m_iter2, *this->m_iter1};
                }
        };
        
        template<typename Iter1, typename Iter2>
        class pair_iterator_base : public dual_iterator_base<Iter1, Iter2>
        {
            public:
                using dual_iterator_base<Iter1, Iter2>::dual_iterator_base;
                
                std::pair<blt::meta::deref_return_t<Iter1>, blt::meta::deref_return_t<Iter2>> operator*() const
                {
                    return {*this->m_iter1, *this->m_iter2};
                }
        };
        
        /**
         * Forward iterator base class. Contains the ++ operator.
         * @tparam Base iterator base type.
         */
        template<typename Base>
        class forward_iterator_base : public Base
        {
            public:
                using Base::Base;
                
                forward_iterator_base& operator++()
                {
                    ++this->m_iter1;
                    ++this->m_iter2;
                    return *this;
                }
                
                forward_iterator_base operator++(int)
                {
                    auto tmp = *this;
                    ++*this;
                    return tmp;
                }
        };
        
        /**
         * Bidirectional iterator base class. Contains the -- operator.
         * @tparam Base iterator base type.
         */
        template<typename Base>
        class bidirectional_iterator_base : public Base
        {
            public:
                using Base::Base;
                
                bidirectional_iterator_base& operator--()
                {
                    --this->m_iter1;
                    --this->m_iter2;
                    return *this;
                }
                
                bidirectional_iterator_base operator--(int)
                {
                    auto tmp = *this;
                    --*this;
                    return tmp;
                }
        };
        
        template<typename Iter>
        using enumerate_forward_iterator = forward_iterator_base<enumerate_iterator_base<Iter>>;
        
        template<typename Iter>
        using enumerate_bidirectional_iterator = bidirectional_iterator_base<enumerate_forward_iterator<Iter>>;
        
        template<typename Iter1, typename Iter2>
        using pair_forward_iterator = forward_iterator_base<pair_iterator_base<Iter1, Iter2>>;
        
        template<typename Iter1, typename Iter2>
        using pair_bidirectional_iterator = bidirectional_iterator_base<pair_forward_iterator<Iter1, Iter2>>;
        
        /**
         * Enumerator wrapper class specialization for forward iterators.
         * @tparam Iter iterator type
         */
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
                
                using enumerate_forward_iterator<Iter>::enumerate_forward_iterator;
        };
        
        /**
         * Pair wrapper class specialization for forward iterators.
         * @tparam Iter iterator type
         */
        template<typename Iter1, typename Iter2>
        class pair_wrapper<Iter1, Iter2, std::enable_if_t<
                blt::meta::is_forward_iterator_category_v<blt::meta::lowest_iterator_category_t<Iter1, Iter2>>,
                std::void_t<std::forward_iterator_tag>>> : public pair_forward_iterator<Iter1, Iter2>
        {
            public:
                using iterator_category = std::forward_iterator_tag;
                using value_type = std::pair<blt::meta::deref_return_t<Iter1>, blt::meta::deref_return_t<Iter2>>;
                using difference_type = std::common_type_t<typename std::iterator_traits<Iter1>::difference_type, typename std::iterator_traits<Iter2>::difference_type>;
                using pointer = value_type;
                using reference = value_type;
                
                using pair_forward_iterator<Iter1, Iter2>::pair_forward_iterator;
        };
        
        /**
         * Enumerator wrapper class for bidirectional iterators or random access iterators.
         * @tparam Iter iterator type.
         */
        template<typename Iter>
        class enumerate_wrapper<Iter, std::enable_if_t<blt::meta::is_bidirectional_or_better_v<Iter>, std::void_t<std::bidirectional_iterator_tag>>>
                : public enumerate_bidirectional_iterator<Iter>
        {
            public:
                using iterator_category = std::bidirectional_iterator_tag;
                using value_type = enumerate_item<blt::meta::deref_return_t<Iter>>;
                using difference_type = typename std::iterator_traits<Iter>::difference_type;
                using pointer = value_type;
                using reference = value_type;
                
                using enumerate_bidirectional_iterator<Iter>::enumerate_bidirectional_iterator;
        };
        
        /**
         * Pair wrapper class for bidirectional iterators or random access iterators.
         * @tparam Iter iterator type.
         */
        template<typename Iter1, typename Iter2>
        class pair_wrapper<Iter1, Iter2, std::enable_if_t<
                blt::meta::is_bidirectional_or_better_category_v<blt::meta::lowest_iterator_category_t<Iter1, Iter2>>,
                std::void_t<std::bidirectional_iterator_tag>>> : public pair_bidirectional_iterator<Iter1, Iter2>
        {
            public:
                using iterator_category = std::bidirectional_iterator_tag;
                using value_type = std::pair<blt::meta::deref_return_t<Iter1>, blt::meta::deref_return_t<Iter2>>;
                using difference_type = std::common_type_t<typename std::iterator_traits<Iter1>::difference_type, typename std::iterator_traits<Iter2>::difference_type>;
                using pointer = value_type;
                using reference = value_type;
                
                using pair_bidirectional_iterator<Iter1, Iter2>::pair_bidirectional_iterator;
        };
        
        /**
         * Base class for storing begin/end iterators.
         * @tparam IterWrapper wrapper used to iterate
         * @tparam CompleteClass completed class returned from skip/take methods
         */
        template<typename IterWrapper, typename CompleteClass>
        class iterator_storage_base
        {
            public:
                explicit iterator_storage_base(IterWrapper begin, IterWrapper end): begin_(std::move(begin)), end_(std::move(end))
                {}
                
                auto begin()
                {
                    return begin_;
                }
                
                auto end()
                {
                    return end_;
                }
                
                /**
                 * Creates an enumerator that skips the first n elements.
                 * @param amount amount of values to skip.
                 */
                auto skip(blt::size_t amount)
                {
                    auto begin = this->begin_;
                    for (blt::size_t i = 0; i < amount; i++)
                        ++begin;
                    return CompleteClass{begin.iter1(),
                                         this->end_.iter1(),
                                         begin.iter2(),
                                         this->end_.iter2()};
                }
                
                /**
                 * Creates an enumerator that yields the first n elements, or UB if the underlying iterator ends sooner.
                 * @param amount amount to take.
                 */
                auto take(blt::size_t amount)
                {
                    auto end = this->begin();
                    for (blt::size_t i = 0; i < amount; i++)
                        ++end;
                    return CompleteClass{this->begin_.iter1(),
                                         end.iter1(),
                                         this->begin_.iter2(),
                                         end.iter2()};
                }
            
            protected:
                IterWrapper begin_;
                IterWrapper end_;
        };
        
        /**
         * Reversible (bidirectional) base class storing the begin / end iterators.
         * @tparam Iter iterator type.
         * @tparam IterWrapper wrapper used to iterate.
         * @tparam CompleteClass completed class returned from skip/take methods
         * @tparam CompleteClassRev reverse version of CompleteClass, returned from rev
         */
        template<typename IterWrapper, typename CompleteClass, typename CompleteClassRev>
        class iterator_storage_reversible : public iterator_storage_base<IterWrapper, CompleteClass>
        {
            public:
                explicit iterator_storage_reversible(IterWrapper begin, IterWrapper end):
                        iterator_storage_base<IterWrapper, CompleteClass>{std::move(begin), std::move(end)}
                {}
                
                /**
                 * Reverses the enumerator’s direction.
                 */
                auto rev() const
                {
                    return CompleteClassRev{this->end_.iter1(),
                                            this->begin_.iter1(),
                                            this->end_.iter2(),
                                            this->begin_.iter2()};
                }
        };
        
        /**
         * Random access base class storage for begin/end iterators.
         * Has updated skip and take methods which make use of the random access nature of the iterator.
         * @tparam Iter iterator type.
         * @tparam IterWrapper wrapper used to iterate.
         * @tparam CompleteClass completed class returned from skip/take methods
         * @tparam CompleteClassRev reverse version of CompleteClass, returned from rev
         */
        template<typename IterWrapper, typename CompleteClass, typename CompleteClassRev>
        class iterator_storage_random_access : public iterator_storage_reversible<IterWrapper, CompleteClass, CompleteClassRev>
        {
            public:
                using iterator_storage_reversible<IterWrapper, CompleteClass, CompleteClassRev>::iterator_storage_reversible;
                
                auto skip(blt::size_t amount)
                {
                    return CompleteClass{this->begin_.iter1() + amount,
                                         this->end_.iter1(),
                                         this->begin_.iter2() + amount,
                                         this->end_.iter2()};
                }
                
                auto take(blt::size_t amount)
                {
                    return CompleteClass{this->begin_.iter1(),
                                         this->begin_.iter1() + amount,
                                         this->begin_.iter2(),
                                         this->begin_.iter2() + amount};
                }
        };
        
        /**
         * Reversible (bidirectional) base class for storing the begin/end iterators, operates in reverse for reverse iteration.
         * @tparam Iter iterator type.
         * @tparam IterWrapper wrapper used to iterate (std::reverse_iterator<enumerate_wrapper>).
         * @tparam CompleteClass completed class returned from skip/take methods
         * @tparam CompleteClassRev reverse version of CompleteClass, returned from rev
         */
        template<typename IterWrapper, typename CompleteClass, typename CompleteClassRev>
        class iterator_storage_reversible_rev : public iterator_storage_reversible<IterWrapper, CompleteClass, CompleteClassRev>
        {
            public:
                using iterator_storage_reversible<IterWrapper, CompleteClass, CompleteClassRev>::iterator_storage_reversible;
                
                auto rev() const
                {
                    return CompleteClass{this->end_.base().iter1(),
                                         this->begin_.base().iter1(),
                                         this->end_.base().iter2(),
                                         this->begin_.base().iter2()};
                }
                
                auto skip(blt::size_t amount)
                {
                    auto begin = this->begin_.base();
                    for (blt::size_t i = 0; i < amount; i++)
                        --begin;
                    return CompleteClassRev{begin.iter1(),
                                            this->end_.base().iter1(),
                                            begin.iter2(),
                                            this->end_.base().iter2()};
                }
                
                auto take(blt::size_t amount)
                {
                    auto end = this->begin_.base();
                    for (blt::size_t i = 0; i < amount; i++)
                        --end;
                    return CompleteClassRev{
                            this->begin_.base().iter1(),
                            end.iter1(),
                            this->begin_.base().iter2(),
                            end.iter2()};
                }
        };
        
        /**
         * Random access base class for storing the begin/end iterator.
         * Has updated skip and take methods which make use of the random access nature of the iterator.
         * Operates in reverse for reverse iteration.
         * @tparam Iter iterator type.
         * @tparam IterWrapper wrapper used to iterate (std::reverse_iterator<enumerate_wrapper>).
         * @tparam CompleteClass completed class returned from skip/take methods
         * @tparam CompleteClassRev reverse version of CompleteClass, returned from rev
         */
        template<typename IterWrapper, typename CompleteClass, typename CompleteClassRev>
        class iterator_storage_random_access_rev : public iterator_storage_reversible_rev<IterWrapper, CompleteClass, CompleteClassRev>
        {
            public:
                using iterator_storage_reversible_rev<IterWrapper, CompleteClass, CompleteClassRev>::iterator_storage_reversible_rev;
                
                auto skip(blt::size_t amount)
                {
                    return CompleteClassRev{this->begin_.base().iter1() - amount,
                                            this->end_.base().iter1(),
                                            this->begin_.base().iter2() - amount,
                                            this->end_.base().iter2()};
                }
                
                auto take(blt::size_t amount)
                {
                    return CompleteClassRev{this->begin_.base().iter1(),
                                            this->begin_.base().iter1() - amount,
                                            this->begin_.base().iter2(),
                                            this->begin_.base().iter2() - amount};
                }
        };
        
        /**
         * Base class for types which can be converted to an enumerator
         */
        template<typename Derived, typename CompleteEnumerator>
        class enumerator_convertible
        {
            public:
                auto enumerate()
                {
                    auto* b = static_cast<Derived*>(this);
                    return CompleteEnumerator{b->begin(), b->end(), static_cast<blt::size_t>(std::distance(b->begin(), b->end()))};
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
        
    }
    
    /**
     * Enumerator specialization for forward iterators
     */
    template<typename Iter>
    class enumerator<Iter, std::enable_if_t<blt::meta::is_forward_iterator_v<Iter>, std::void_t<std::forward_iterator_tag>>>
            : public iterator::iterator_storage_base<iterator::enumerate_wrapper<Iter>, enumerator<Iter>>
    {
        public:
            explicit enumerator(Iter begin, Iter end, blt::size_t container_size):
                    iterator::iterator_storage_base<iterator::enumerate_wrapper<Iter>, enumerator<Iter>>
                            {iterator::enumerate_wrapper<Iter>{std::move(begin), 0},
                             iterator::enumerate_wrapper<Iter>{std::move(end), container_size}}
            {}
            
            explicit enumerator(Iter begin, Iter end, blt::size_t begin_index, blt::size_t end_index):
                    iterator::iterator_storage_base<iterator::enumerate_wrapper<Iter>, enumerator<Iter>>{
                            iterator::enumerate_wrapper<Iter>{std::move(begin), begin_index},
                            iterator::enumerate_wrapper<Iter>{std::move(end), end_index}}
            {}
    };
    
    /**
     * Enumerator specialization for bidirectional iterators
     */
    template<typename Iter>
    class enumerator<Iter, std::enable_if_t<blt::meta::is_bidirectional_iterator_v<Iter>, std::void_t<std::bidirectional_iterator_tag>>>
            : public iterator::iterator_storage_reversible<iterator::enumerate_wrapper<Iter>, enumerator<Iter>, enumerator_rev<Iter>>
    {
        public:
            explicit enumerator(Iter begin, Iter end, blt::size_t container_size):
                    iterator::iterator_storage_reversible<iterator::enumerate_wrapper<Iter>, enumerator<Iter>, enumerator_rev<Iter>>
                            {iterator::enumerate_wrapper<Iter>{std::move(begin), 0},
                             iterator::enumerate_wrapper<Iter>{std::move(end), container_size}}
            {}
            
            explicit enumerator(Iter begin, Iter end, blt::size_t begin_index, blt::size_t end_index):
                    iterator::iterator_storage_reversible<iterator::enumerate_wrapper<Iter>, enumerator<Iter>, enumerator_rev<Iter>>{
                            iterator::enumerate_wrapper<Iter>{std::move(begin), begin_index},
                            iterator::enumerate_wrapper<Iter>{std::move(end), end_index}}
            {}
    };
    
    /**
     * Enumerator specialization for random access iterators
     */
    template<typename Iter>
    class enumerator<Iter, std::enable_if_t<blt::meta::is_random_access_iterator_v<Iter>, std::void_t<std::random_access_iterator_tag>>>
            : public iterator::iterator_storage_random_access<iterator::enumerate_wrapper<Iter>, enumerator<Iter>, enumerator_rev<Iter>>
    {
        public:
            explicit enumerator(Iter begin, Iter end, blt::size_t container_size):
                    iterator::iterator_storage_random_access<iterator::enumerate_wrapper<Iter>, enumerator<Iter>, enumerator_rev<Iter>>
                            {iterator::enumerate_wrapper<Iter>{std::move(begin), 0},
                             iterator::enumerate_wrapper<Iter>{std::move(end), container_size}}
            {}
            
            explicit enumerator(Iter begin, Iter end, blt::size_t begin_index, blt::size_t end_index):
                    iterator::iterator_storage_random_access<iterator::enumerate_wrapper<Iter>, enumerator<Iter>, enumerator_rev<Iter>>{
                            iterator::enumerate_wrapper<Iter>{std::move(begin), begin_index},
                            iterator::enumerate_wrapper<Iter>{std::move(end), end_index}}
            {}
    };
    
    /**
     * Reverse enumerator specialization for bidirectional iterators
     */
    template<typename Iter>
    class enumerator_rev<Iter, std::enable_if_t<blt::meta::is_bidirectional_iterator_v<Iter>, std::void_t<std::bidirectional_iterator_tag>>>
            : public iterator::iterator_storage_reversible_rev<std::reverse_iterator<iterator::enumerate_wrapper<Iter>>, enumerator<Iter>, enumerator_rev<Iter>>
    {
        public:
            explicit enumerator_rev(Iter begin, Iter end, blt::size_t container_size):
                    iterator::iterator_storage_reversible_rev<std::reverse_iterator<iterator::enumerate_wrapper<Iter>>, enumerator<Iter>, enumerator_rev<Iter>>
                            {std::reverse_iterator<iterator::enumerate_wrapper<Iter>>{iterator::enumerate_wrapper<Iter>{std::move(begin), 0}},
                             std::reverse_iterator<iterator::enumerate_wrapper<Iter>>{
                                     iterator::enumerate_wrapper<Iter>{std::move(end), container_size}}}
            {}
            
            explicit enumerator_rev(Iter begin, Iter end, blt::size_t begin_index, blt::size_t end_index):
                    iterator::iterator_storage_reversible_rev<std::reverse_iterator<iterator::enumerate_wrapper<Iter>>, enumerator<Iter>, enumerator_rev<Iter>>{
                            std::reverse_iterator<iterator::enumerate_wrapper<Iter>>{
                                    iterator::enumerate_wrapper<Iter>{std::move(begin), begin_index}},
                            std::reverse_iterator<iterator::enumerate_wrapper<Iter>>{iterator::enumerate_wrapper<Iter>{std::move(end), end_index}}}
            {}
    };
    
    /**
     * Reverse enumerator specialization for random access iterators
     */
    template<typename Iter>
    class enumerator_rev<Iter, std::enable_if_t<blt::meta::is_random_access_iterator_v<Iter>, std::void_t<std::random_access_iterator_tag>>>
            : public iterator::iterator_storage_random_access_rev<std::reverse_iterator<iterator::enumerate_wrapper<Iter>>, enumerator<Iter>, enumerator_rev<Iter>>
    {
        public:
            explicit enumerator_rev(Iter begin, Iter end, blt::size_t container_size):
                    iterator::iterator_storage_random_access_rev<std::reverse_iterator<iterator::enumerate_wrapper<Iter>>, enumerator<Iter>, enumerator_rev<Iter>>
                            {std::reverse_iterator<iterator::enumerate_wrapper<Iter>>{iterator::enumerate_wrapper<Iter>{std::move(begin), 0}},
                             std::reverse_iterator<iterator::enumerate_wrapper<Iter>>{
                                     iterator::enumerate_wrapper<Iter>{std::move(end), container_size}}}
            {}
            
            explicit enumerator_rev(Iter begin, Iter end, blt::size_t begin_index, blt::size_t end_index):
                    iterator::iterator_storage_random_access_rev<std::reverse_iterator<iterator::enumerate_wrapper<Iter>>, enumerator<Iter>, enumerator_rev<Iter>>{
                            std::reverse_iterator<iterator::enumerate_wrapper<Iter>>{
                                    iterator::enumerate_wrapper<Iter>{std::move(begin), begin_index}},
                            std::reverse_iterator<iterator::enumerate_wrapper<Iter>>{iterator::enumerate_wrapper<Iter>{std::move(end), end_index}}}
            {}
    };
    
    // CTAD for enumerators
    
    template<typename Iter>
    enumerator(Iter, Iter) -> enumerator<Iter>;
    
    template<typename Iter>
    enumerator(Iter, Iter, blt::size_t) -> enumerator<Iter>;
    
    template<typename Iter>
    enumerator(Iter, Iter, blt::size_t, blt::size_t) -> enumerator<Iter>;
    
    template<typename Iter1, typename Iter2>
    class pair_iterator<Iter1, Iter2,
            std::enable_if_t<
                    blt::meta::is_forward_iterator_category_v<blt::meta::lowest_iterator_category_t<Iter1, Iter2>>,
                    std::void_t<std::forward_iterator_tag>>>
            : public iterator::iterator_storage_base<iterator::pair_wrapper<Iter1, Iter2>, pair_iterator<Iter1, Iter2>>,
              public iterator::enumerator_convertible<pair_iterator<Iter1, Iter2>, enumerator<iterator::pair_wrapper<Iter1, Iter2>>>
    {
        public:
            explicit pair_iterator(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2):
                    iterator::iterator_storage_base<iterator::pair_wrapper<Iter1, Iter2>, pair_iterator<Iter1, Iter2>>
                            {iterator::pair_wrapper<Iter1, Iter2>{std::move(begin1), std::move(begin2)},
                             iterator::pair_wrapper<Iter1, Iter2>{std::move(end1), std::move(end2)}}
            {}
    };
    
    template<typename Iter1, typename Iter2>
    class pair_iterator<Iter1, Iter2,
            std::enable_if_t<
                    blt::meta::is_bidirectional_iterator_category_v<blt::meta::lowest_iterator_category_t<Iter1, Iter2>>,
                    std::void_t<std::bidirectional_iterator_tag>>>
            : public iterator::iterator_storage_reversible<iterator::pair_wrapper<Iter1, Iter2>, pair_iterator<Iter1, Iter2>, pair_iterator_rev<Iter1, Iter2>>,
              public iterator::enumerator_convertible<pair_iterator<Iter1, Iter2>, enumerator<iterator::pair_wrapper<Iter1, Iter2>>>
    {
        public:
            explicit pair_iterator(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2):
                    iterator::iterator_storage_reversible<iterator::pair_wrapper<Iter1, Iter2>, pair_iterator<Iter1, Iter2>, pair_iterator_rev<Iter1, Iter2>>
                            {iterator::pair_wrapper<Iter1, Iter2>{std::move(begin1), std::move(begin2)},
                             iterator::pair_wrapper<Iter1, Iter2>{std::move(end1), std::move(end2)}}
            {}
    };
    
    template<typename Iter1, typename Iter2>
    class pair_iterator<Iter1, Iter2,
            std::enable_if_t<
                    blt::meta::is_random_access_iterator_category_v<blt::meta::lowest_iterator_category_t<Iter1, Iter2>>,
                    std::void_t<std::random_access_iterator_tag>>>
            : public iterator::iterator_storage_random_access<iterator::pair_wrapper<Iter1, Iter2>, pair_iterator<Iter1, Iter2>, pair_iterator_rev<Iter1, Iter2>>,
              public iterator::enumerator_convertible<pair_iterator<Iter1, Iter2>, enumerator<iterator::pair_wrapper<Iter1, Iter2>>>
    {
        public:
            explicit pair_iterator(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2):
                    iterator::iterator_storage_random_access<iterator::pair_wrapper<Iter1, Iter2>, pair_iterator<Iter1, Iter2>, pair_iterator_rev<Iter1, Iter2>>
                            {iterator::pair_wrapper<Iter1, Iter2>{std::move(begin1), std::move(begin2)},
                             iterator::pair_wrapper<Iter1, Iter2>{std::move(end1), std::move(end2)}}
            {}
    };
    
    template<typename Iter1, typename Iter2>
    class pair_iterator_rev<Iter1, Iter2,
            std::enable_if_t<
                    blt::meta::is_bidirectional_iterator_category_v<blt::meta::lowest_iterator_category_t<Iter1, Iter2>>,
                    std::void_t<std::bidirectional_iterator_tag>>>
            : public iterator::iterator_storage_reversible_rev<std::reverse_iterator<iterator::pair_wrapper<Iter1, Iter2>>, pair_iterator<Iter1, Iter2>, pair_iterator_rev<Iter1, Iter2>>,
              public iterator::enumerator_convertible<pair_iterator<Iter1, Iter2>, enumerator<iterator::pair_wrapper<Iter1, Iter2>>>
    {
        public:
            explicit pair_iterator_rev(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2):
                    iterator::iterator_storage_reversible_rev<std::reverse_iterator<iterator::pair_wrapper<Iter1, Iter2>>, pair_iterator<Iter1, Iter2>, pair_iterator_rev<Iter1, Iter2>>
                            {std::reverse_iterator<iterator::pair_wrapper<Iter1, Iter2>>{
                                    iterator::pair_wrapper<Iter1, Iter2>{std::move(begin1), std::move(begin2)}},
                             std::reverse_iterator<iterator::pair_wrapper<Iter1, Iter2>>{
                                     iterator::pair_wrapper<Iter1, Iter2>{std::move(end1), std::move(end2)}}}
            {}
    };
    
    template<typename Iter1, typename Iter2>
    class pair_iterator_rev<Iter1, Iter2,
            std::enable_if_t<
                    blt::meta::is_random_access_iterator_category_v<blt::meta::lowest_iterator_category_t<Iter1, Iter2>>,
                    std::void_t<std::random_access_iterator_tag>>>
            : public iterator::iterator_storage_random_access_rev<std::reverse_iterator<iterator::pair_wrapper<Iter1, Iter2>>, pair_iterator<Iter1, Iter2>, pair_iterator_rev<Iter1, Iter2>>,
              public iterator::enumerator_convertible<pair_iterator<Iter1, Iter2>, enumerator<iterator::pair_wrapper<Iter1, Iter2>>>
    {
        public:
            explicit pair_iterator_rev(Iter1 begin1, Iter1 end1, Iter2 begin2, Iter2 end2):
                    iterator::iterator_storage_random_access_rev<std::reverse_iterator<iterator::pair_wrapper<Iter1, Iter2>>, pair_iterator<Iter1, Iter2>, pair_iterator_rev<Iter1, Iter2>>
                            {std::reverse_iterator<iterator::pair_wrapper<Iter1, Iter2>>{
                                    iterator::pair_wrapper<Iter1, Iter2>{std::move(begin1), std::move(begin2)}},
                             std::reverse_iterator<iterator::pair_wrapper<Iter1, Iter2>>{
                                     iterator::pair_wrapper<Iter1, Iter2>{std::move(end1), std::move(end2)}}}
            {}
    };
    
    // CTAD for pair iterators
    
    template<typename Iter1, typename Iter2>
    pair_iterator(Iter1, Iter1, Iter2, Iter2) -> pair_iterator<Iter1, Iter2>;
    
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
    
    template<typename T>
    static inline auto enumerate(const T& container)
    {
        return enumerator{container.begin(), container.end(), container.size()};
    }
    
    template<typename T, typename G>
    static inline auto in_pairs(const T& container1, const G& container2)
    {
        return pair_iterator{container1.begin(), container1.end(), container2.begin(), container2.end()};
    }
    
    template<typename T, typename G>
    static inline auto in_pairs(T& container1, G& container2)
    {
        return pair_iterator{container1.begin(), container1.end(), container2.begin(), container2.end()};
    }
    
    template<typename T, typename G, blt::size_t size>
    static inline auto in_pairs(const T(& container1)[size], const G(& container2)[size])
    {
        return pair_iterator{&container1[0], &container1[size], &container2[0], &container2[size]};
    }
    
    template<typename T, typename G, blt::size_t size>
    static inline auto in_pairs(T(& container1)[size], G(& container2)[size])
    {
        return pair_iterator{&container1[0], &container1[size], &container2[0], &container2[size]};
    }
    
    template<typename T, typename G>
    static inline auto in_pairs(T&& container1, G&& container2)
    {
        return pair_iterator{container1.begin(), container1.end(), container2.begin(), container2.end()};
    }
    
}

#endif //BLT_ITERATOR_H
