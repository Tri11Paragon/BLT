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
#include <blt/std/types.h>
#include <blt/iterator/fwddecl.h>
#include <blt/meta/meta.h>
#include <blt/meta/iterator.h>
#include <functional>
#include <optional>

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
            static_assert(meta::is_bidirectional_or_better_category_v<typename Derived::iterator_category>, "Iterator must allow random access");
            auto tmp = *this;
            --*this;
            return tmp;
        }
        
        auto operator[](blt::ptrdiff_t n) const
        {
            static_assert(meta::is_random_access_iterator_category_v<typename Derived::iterator_category>, "Iterator must allow random access");
            return *(*this + n);
        }
        
        friend base_wrapper operator+(blt::ptrdiff_t n, const base_wrapper& a)
        {
            return a + n;
        }
        
        friend bool operator<(const base_wrapper& a, const base_wrapper& b)
        {
            static_assert(meta::is_random_access_iterator_category_v<typename Derived::iterator_category>, "Iterator must allow random access");
            return b - a > 0;
        }
        
        friend bool operator>(const base_wrapper& a, const base_wrapper& b)
        {
            static_assert(meta::is_random_access_iterator_category_v<typename Derived::iterator_category>, "Iterator must allow random access");
            return b < a;
        }
        
        friend bool operator>=(const base_wrapper& a, base_wrapper& b)
        {
            static_assert(meta::is_random_access_iterator_category_v<typename Derived::iterator_category>, "Iterator must allow random access");
            return !(a < b); // NOLINT
        }
        
        friend bool operator<=(const base_wrapper& a, const base_wrapper& b)
        {
            static_assert(meta::is_random_access_iterator_category_v<typename Derived::iterator_category>, "Iterator must allow random access");
            return !(a > b); // NOLINT
        }
        
        friend bool operator==(const base_wrapper& a, const base_wrapper& b)
        {
            return static_cast<const Derived&>(a).base() == static_cast<const Derived&>(b).base();
        }
        
        friend bool operator!=(const base_wrapper& a, const base_wrapper& b)
        {
            return !(static_cast<const Derived&>(a).base() == static_cast<const Derived&>(b).base()); // NOLINT
        }
    };
    
    template<typename Iter, typename Derived, bool dereference = false>
    struct passthrough_wrapper : public base_wrapper<Derived>
    {
        public:
            explicit passthrough_wrapper(Iter iter): iter(std::move(iter))
            {}
            
            auto base() const
            {
                return iter;
            }
            
            friend blt::ptrdiff_t operator-(const passthrough_wrapper& a, const passthrough_wrapper& b)
            {
                return a.base() - b.base();
            }
        
        protected:
            mutable Iter iter;
    };
    
    template<typename Iter, typename Derived>
    struct passthrough_wrapper<Iter, Derived, true> : public passthrough_wrapper<Iter, Derived>
    {
        using passthrough_wrapper<Iter, Derived>::passthrough_wrapper;
        
        meta::deref_return_t<Iter> operator*() const
        {
            return *this->iter;
        }
    };
    
    template<typename Iter, typename Derived>
    class deref_only_wrapper : public passthrough_wrapper<Iter, Derived, false>
    {
        public:
            using passthrough_wrapper<Iter, Derived, false>::passthrough_wrapper;
            
            deref_only_wrapper& operator++()
            {
                ++this->iter;
                return *this;
            }
            
            deref_only_wrapper& operator--()
            {
                --this->iter;
                return *this;
            }
            
            deref_only_wrapper& operator+(blt::ptrdiff_t n)
            {
                static_assert(meta::is_random_access_iterator_v<Iter>, "Iterator must allow random access");
                this->iter = this->iter + n;
                return *this;
            }
            
            deref_only_wrapper& operator-(blt::ptrdiff_t n)
            {
                static_assert(meta::is_random_access_iterator_v<Iter>, "Iterator must allow random access");
                this->iter = this->iter - n;
                return *this;
            }
    };
    
    template<typename Iter, typename Func>
    class map_wrapper : public deref_only_wrapper<Iter, map_wrapper<Iter, Func>>
    {
        public:
            using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
            using value_type = std::invoke_result_t<Func, meta::deref_return_t<Iter>>;
            using difference_type = blt::ptrdiff_t;
            using pointer = value_type;
            using reference = value_type;
            
            map_wrapper(Iter iter, Func func):
                    deref_only_wrapper<Iter, map_wrapper<Iter, Func>>(std::move(iter)), func(std::move(func))
            {}
            
            reference operator*() const
            {
                return func(*this->iter);
            }
        
        private:
            Func func;
    };
    
    template<typename Iter, typename Pred>
    class filter_wrapper : public deref_only_wrapper<Iter, filter_wrapper<Iter, Pred>>
    {
        public:
            using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
            using value_type = std::conditional_t<
                    std::is_reference_v<meta::deref_return_t<Iter>>,
                    std::optional<std::reference_wrapper<std::remove_reference_t<meta::deref_return_t<Iter>>>>,
                    std::optional<meta::deref_return_t<Iter>>>;
            using difference_type = blt::ptrdiff_t;
            using pointer = value_type;
            using reference = value_type;
            
            filter_wrapper(Iter iter, Pred func):
                    deref_only_wrapper<Iter, filter_wrapper<Iter, Pred>>(std::move(iter)), func(std::move(func))
            {}
            
            reference operator*() const
            {
                if (!func(*this->iter))
                {
                    return {};
                }
                return *this->iter;
            }
        
        private:
            Pred func;
    };
    
    namespace impl
    {
        template<typename Derived>
        class skip_t
        {
            private:
                template<bool check>
                auto skip_base(blt::size_t n)
                {
                    auto* d = static_cast<Derived*>(this);
                    auto begin = d->begin();
                    auto end = d->end();
                    
                    if constexpr (meta::is_random_access_iterator_category_v<typename Derived::iterator_category>)
                    {
                        // random access iterators can have math directly applied to them.
                        if constexpr (check)
                        {
                            return Derived{begin + std::min(static_cast<blt::ptrdiff_t>(n), std::distance(begin, end)), end};
                        } else
                        {
                            return Derived{begin + n, end};
                        }
                    } else
                    {
                        for (blt::size_t i = 0; i < n; i++)
                        {
                            if constexpr (check)
                            {
                                if (begin == end)
                                    break;
                            }
                            ++begin;
                        }
                        return Derived{std::move(begin), std::move(end)};
                    }
                }
            
            public:
                auto skip(blt::size_t n)
                { return skip_base<false>(n); }
                
                auto skip_or(blt::size_t n)
                { return skip_base<true>(n); }
        };
        
        template<typename Derived>
        class take_t
        {
            private:
                template<bool check>
                auto take_base(blt::size_t n)
                {
                    static_assert(!meta::is_input_iterator_category_v<typename Derived::iterator_category>,
                                  "Cannot .take() on an input iterator!");
                    auto* d = static_cast<Derived*>(this);
                    auto begin = d->begin();
                    auto end = d->end();
                    
                    // take variant for forward and bidirectional iterators
                    if constexpr (meta::is_forward_iterator_category_v<typename Derived::iterator_category> ||
                                  meta::is_bidirectional_iterator_category_v<typename Derived::iterator_category>)
                    {
                        // with these guys we have to loop forward to move the iterators. an unfortunate inefficiency
                        auto new_end = begin;
                        for (blt::size_t i = 0; i < n; i++)
                        {
                            if constexpr (check)
                            {
                                if (new_end == end)
                                    break;
                            }
                            ++new_end;
                        }
                        return Derived{std::move(begin), std::move(new_end)};
                    } else if constexpr (meta::is_random_access_iterator_category_v<typename Derived::iterator_category>)
                    {
                        // random access iterators can have math directly applied to them.
                        if constexpr (check)
                        {
                            return Derived{begin, begin + std::min(static_cast<blt::ptrdiff_t>(n), std::distance(begin, end))};
                        } else
                        {
                            return Derived{begin, begin + n};
                        }
                    }
                }
            
            public:
                auto take(blt::size_t n)
                { return take_base<false>(n); }
                
                auto take_or(blt::size_t n)
                { return take_base<true>(n); }
        };
    }
    
    template<typename IterBase>
    class iterator_container : public impl::take_t<iterator_container<IterBase>>,
                               public impl::skip_t<iterator_container<IterBase>>
    {
        public:
            using iterator_category = typename std::iterator_traits<IterBase>::iterator_category;
            using iterator = IterBase;
            
            iterator_container(IterBase begin, IterBase end): m_begin(std::move(begin)), m_end(std::move(end))
            {}
            
            template<typename Iter>
            iterator_container(Iter&& begin, Iter&& end): m_begin(std::forward<Iter>(begin)), m_end(std::forward<Iter>(end))
            {}
            
            auto rev() const
            {
                static_assert(meta::is_bidirectional_or_better_category_v<iterator_category>,
                              ".rev() must be used with bidirectional (or better) iterators!");
                return iterator_container<std::reverse_iterator<IterBase>>{std::reverse_iterator<IterBase>{end()},
                                                                           std::reverse_iterator<IterBase>{begin()}};
            }
            
            template<typename... Iter>
            auto zip(iterator_pair<Iter>... iterator_pairs) const
            {
                return zip_iterator_container(iterator_pair<decltype(begin())>{begin(), end()}, iterator_pairs...);
            }
            
            template<typename... Container>
            auto zip(Container& ... containers) const
            {
                return zip_iterator_container(iterator_pair<decltype(begin())>{begin(), end()},
                                              iterator_pair{containers.begin(), containers.end()}...);
            }
            
            template<typename... Container>
            auto zip(const Container& ... containers) const
            {
                return zip_iterator_container(iterator_pair<decltype(begin())>{begin(), end()},
                                              iterator_pair{containers.begin(), containers.end()}...);
            }
            
            auto enumerate() const
            {
                return enumerate_iterator_container{begin(), end(), static_cast<blt::size_t>(std::distance(begin(), end()))};
            }
            
            template<typename Func>
            auto map(Func func) const
            {
                return iterator_container<blt::iterator::map_wrapper<IterBase, Func>>{
                        blt::iterator::map_wrapper<IterBase, Func>{m_begin, func},
                        blt::iterator::map_wrapper<IterBase, Func>{m_end, func}};
            }
            
            template<typename Pred>
            auto filter(Pred pred) const
            {
                return iterator_container<blt::iterator::filter_wrapper<IterBase, Pred>>{
                        blt::iterator::filter_wrapper<IterBase, Pred>{m_begin, pred},
                        blt::iterator::filter_wrapper<IterBase, Pred>{m_end, pred}};
            }
            
            auto begin() const
            {
                return m_begin;
            }
            
            auto end() const
            {
                return m_end;
            }
        
        protected:
            IterBase m_begin;
            IterBase m_end;
    };
    
}

#endif //BLT_ITERATOR_ITER_COMMON