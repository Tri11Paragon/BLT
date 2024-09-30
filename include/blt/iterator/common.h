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
#include <blt/meta/meta.h>

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
                          std::is_same_v<typename Derived::iterator_category,
                                  std::random_access_iterator_tag>,
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
    
    namespace impls
    {
        template<typename Iter>
        struct skip_wrapper : public passthrough_wrapper<Iter, skip_wrapper<Iter>>
        {
            public:
                using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
                using value_type = typename std::iterator_traits<Iter>::value_type;
                using difference_type = typename std::iterator_traits<Iter>::difference_type;
                using pointer = typename std::iterator_traits<Iter>::pointer;
                using reference = typename std::iterator_traits<Iter>::reference;
                
                explicit skip_wrapper(Iter iter, blt::size_t n): passthrough_wrapper<Iter, skip_wrapper<Iter>>(std::move(iter)), skip(n)
                {}
                
                meta::deref_return_t<Iter> operator*() const
                {
                    BLT_TRACE("Dereference Skip");
                    forward_skip();
                    return *this->iter;
                }
                
                skip_wrapper& operator++()
                {
                    BLT_TRACE("Forward Skip");
                    forward_skip();
                    ++this->iter;
                    return *this;
                }
                
                skip_wrapper& operator--()
                {
                    BLT_TRACE("Backward Skip");
                    forward_skip();
                    --this->iter;
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
                void forward_skip() const
                {
                    if constexpr (std::is_same_v<iterator_category, std::random_access_iterator_tag>)
                    {
                        if (skip > 0)
                        {
                            this->iter = this->iter + skip;
                            skip = 0;
                        }
                    } else
                    {
                        while (skip > 0)
                        {
                            ++this->iter;
                            --skip;
                        }
                    }
                }
                
                mutable blt::size_t skip;
        };
        
        template<typename Derived>
        class take_t
        {
            private:
                template<bool check>
                auto take_base(blt::size_t n)
                {
                    static_assert(!std::is_same_v<typename Derived::iterator_category, std::input_iterator_tag>,
                                  "Cannot .take() on an input iterator!");
                    auto* d = static_cast<Derived*>(this);
                    auto begin = d->begin();
                    auto end = d->end();
                    
                    // take variant for forward and bidirectional iterators
                    if constexpr (std::is_same_v<typename Derived::iterator_category, std::forward_iterator_tag> ||
                                  std::is_same_v<typename Derived::iterator_category, std::bidirectional_iterator_tag>)
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
                    } else if constexpr (std::is_same_v<typename Derived::iterator_category, std::random_access_iterator_tag>)
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
    class iterator_container : public impls::take_t<iterator_container<IterBase>>
    {
        public:
            using iterator_category = typename IterBase::iterator_category;
            
            iterator_container(IterBase begin, IterBase end): m_begin(std::move(begin)), m_end(std::move(end))
            {}
            
            template<typename Iter>
            iterator_container(Iter&& begin, Iter&& end): m_begin(std::forward<Iter>(begin)), m_end(std::forward<Iter>(end))
            {}
            
            auto rev()
            {
                static_assert((std::is_same_v<typename IterBase::iterator_category, std::bidirectional_iterator_tag> ||
                               std::is_same_v<typename IterBase::iterator_category, std::random_access_iterator_tag>),
                              ".rev() must be used with bidirectional (or better) iterators!");
                return iterator_container<std::reverse_iterator<IterBase>>{std::reverse_iterator<IterBase>{end()},
                                                                           std::reverse_iterator<IterBase>{begin()}};
            }
            
            auto skip(blt::size_t n)
            {
                return iterator_container<impls::skip_wrapper<IterBase>>{impls::skip_wrapper<IterBase>{begin(), n},
                                                                         impls::skip_wrapper<IterBase>{end(), n}};
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
