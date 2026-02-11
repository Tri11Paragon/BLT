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
#include <blt/meta/type_traits.h>

namespace blt::iterator
{
    namespace detail
    {
        template <typename T>
        static auto forward_as_tuple(T&& t) -> decltype(auto)
        {
            using Decay = std::decay_t<T>;
            static_assert(!(meta::is_tuple_v<Decay> || meta::is_pair_v<Decay>),
                          "Tuple or pair passed to forward_as_tuple! Must not be a tuple!");
            if constexpr (std::is_lvalue_reference_v<T>)
            {
                return std::forward_as_tuple(std::forward<T>(t));
            }
            else
            {
                return std::make_tuple(std::forward<T>(t));
            }
        }

        template <typename Tuple>
        static auto ensure_tuple(Tuple&& tuple) -> decltype(auto)
        {
            using Decay = std::decay_t<Tuple>;
            if constexpr (meta::is_tuple_v<Decay> || meta::is_pair_v<Decay>)
            {
                return std::forward<Tuple>(tuple);
            }
            else
            {
                return forward_as_tuple(std::forward<Tuple>(tuple));
            }
        }
    }

    template <typename Derived>
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
            static_assert(meta::is_bidirectional_or_better_category_v<typename Derived::iterator_category>,
                          "Iterator must allow bidirectional access");
            auto tmp = *this;
            --*this;
            return tmp;
        }

        auto operator[](ptrdiff_t n) const
        {
            static_assert(meta::is_random_access_iterator_category_v<typename Derived::iterator_category>,
                          "Iterator must allow random access");
            return *(*this + n);
        }

        friend base_wrapper operator+(ptrdiff_t n, const base_wrapper& a)
        {
            return a + n;
        }

        friend bool operator<(const base_wrapper& a, const base_wrapper& b)
        {
            return std::distance(static_cast<const Derived&>(b), static_cast<const Derived&>(a)) > 0;
        }

        friend bool operator>(const base_wrapper& a, const base_wrapper& b)
        {
            return b < a;
        }

        friend bool operator>=(const base_wrapper& a, base_wrapper& b)
        {
            return !(a < b); // NOLINT
        }

        friend bool operator<=(const base_wrapper& a, const base_wrapper& b)
        {
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

    template <typename Iter, typename Derived, bool dereference = false>
    struct passthrough_wrapper : base_wrapper<Derived>
    {
        explicit passthrough_wrapper(Iter iter) : iter(std::move(iter))
        {
        }

        auto base() const
        {
            return iter;
        }

        friend ptrdiff_t operator-(const passthrough_wrapper& a, const passthrough_wrapper& b)
        {
            return a.base() - b.base();
        }

    protected:
        mutable Iter iter;
    };

    template <typename Iter, typename Derived>
    struct passthrough_wrapper<Iter, Derived, true> : passthrough_wrapper<Iter, Derived>
    {
        using passthrough_wrapper<Iter, Derived>::passthrough_wrapper;

        meta::deref_return_t<Iter> operator*() const
        {
            return *this->iter;
        }
    };

    template <typename Iter, typename Derived>
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

        deref_only_wrapper& operator-(ptrdiff_t n)
        {
            static_assert(meta::is_random_access_iterator_v<Iter>, "Iterator must allow random access");
            this->iter = this->iter - n;
            return *this;
        }
    };

    template <typename Iter, typename Func, typename... Args>
    class map_wrapper : public deref_only_wrapper<Iter, map_wrapper<Iter, Func, Args...>>
    {
    public:
        using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
        using value_type = std::invoke_result_t<Func, meta::deref_return_t<Iter>, Args...>;
        using difference_type = ptrdiff_t;
        using pointer = value_type;
        using reference = value_type;

        map_wrapper(Iter iter, Func func, Args&&... args) :
            deref_only_wrapper<Iter, map_wrapper>(std::move(iter)), func(std::move(func)),
            args(std::forward_as_tuple(std::forward<Args>(args)...))
        {
        }

        reference operator*() const
        {
            return std::apply(func, std::tuple_cat(std::forward_as_tuple(*this->iter), args));
        }

    private:
        Func func;
        std::tuple<Args...> args;
    };

    template <typename Iter, typename Pred, typename... Args>
    class filter_wrapper : public deref_only_wrapper<Iter, filter_wrapper<Iter, Pred, Args...>>
    {
    public:
        using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
        using value_type = std::conditional_t<
            std::is_reference_v<meta::deref_return_t<Iter>>,
            std::optional<std::reference_wrapper<std::remove_reference_t<meta::deref_return_t<Iter>>>>,
            std::optional<meta::deref_return_t<Iter>>>;
        using difference_type = ptrdiff_t;
        using pointer = value_type;
        using reference = value_type;

        filter_wrapper(Iter iter, Pred func, Args... args) :
            deref_only_wrapper<Iter, filter_wrapper>(std::move(iter)), func(std::move(func)),
            args(std::forward_as_tuple(std::forward<Args>(args)...))
        {
        }

        reference operator*() const
        {
            decltype(auto) iter_val = *this->iter;
            if (!std::apply(func, std::tuple_cat(std::forward_as_tuple(iter_val), args)))
            {
                return {};
            }
            return iter_val;
        }

    private:
        Pred func;
        std::tuple<Args...> args;
    };

    template<typename Iter>
    struct move_wrapper : deref_only_wrapper<Iter, move_wrapper<Iter>>
    {
        using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
        using value_type = std::add_rvalue_reference_t<meta::deref_return_t<Iter>>;
        using difference_type = ptrdiff_t;
        using pointer = value_type;
        using reference = value_type;

        explicit move_wrapper(Iter iter) :
            deref_only_wrapper<Iter, move_wrapper>(std::move(iter))
        {

        }

        reference operator*() const
        {
            return std::move(*this->iter);
        }
    };

    template <typename Iter>
    class const_wrapper : public deref_only_wrapper<Iter, const_wrapper<Iter>>
    {
    public:
        using ref_return = meta::deref_return_t<Iter>;

        using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
        using value_type = std::conditional_t<std::is_reference_v<ref_return>, std::remove_reference_t<ref_return>,
                                              ref_return>;
        using difference_type = ptrdiff_t;
        using pointer = const value_type*;
        using reference = const value_type&;

        explicit const_wrapper(Iter iter) : deref_only_wrapper<Iter, const_wrapper>(std::move(iter))
        {
        }

        template <typename T>
        static auto make_const(T&& value) -> decltype(auto)
        {
            using Decay = std::decay_t<T>;
            if constexpr (std::is_lvalue_reference_v<T>)
            {
                return const_cast<const Decay&>(value);
            }
            else
            {
                return static_cast<const Decay>(value);
            }
        }

        auto operator*() const -> decltype(auto)
        {
            if constexpr (std::is_reference_v<ref_return>)
            {
                return const_cast<const value_type&>(*this->iter);
            }
            else if constexpr (meta::is_tuple_v<value_type> || meta::is_pair_v<value_type>)
            {
                return std::apply([](auto&&... args)
                {
                    return std::tuple<decltype(make_const(std::forward<decltype(args)>(args)))...>{
                        make_const(std::forward<decltype(args)>(args))...
                    };
                }, *this->iter);
            }
            else
            {
                return *this->iter;
            }
        }
    };

    namespace impl
    {
        template <typename Derived>
        class skip_t
        {
        private:
            template <bool check>
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
                        return Derived{
                            begin + std::min(static_cast<blt::ptrdiff_t>(n), std::distance(begin, end)), end
                        };
                    }
                    else
                    {
                        return Derived{begin + n, end};
                    }
                }
                else
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
            {
                return skip_base<false>(n);
            }

            auto skip_or(blt::size_t n)
            {
                return skip_base<true>(n);
            }
        };

        template <typename Derived>
        class take_t
        {
        private:
            template <bool check>
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
                }
                else if constexpr (meta::is_random_access_iterator_category_v<typename Derived::iterator_category>)
                {
                    // random access iterators can have math directly applied to them.
                    if constexpr (check)
                    {
                        return Derived{
                            begin, begin + std::min(static_cast<blt::ptrdiff_t>(n), std::distance(begin, end))
                        };
                    }
                    else
                    {
                        return Derived{begin, begin + n};
                    }
                }
            }

        public:
            auto take(blt::size_t n)
            {
                return take_base<false>(n);
            }

            auto take_or(blt::size_t n)
            {
                return take_base<true>(n);
            }
        };
    }

    template <typename IterBase>
    class iterator_container : public impl::take_t<iterator_container<IterBase>>,
                               public impl::skip_t<iterator_container<IterBase>>
    {
    public:
        using iterator_category = typename std::iterator_traits<IterBase>::iterator_category;
        using iterator = IterBase;

        iterator_container(IterBase begin, IterBase end) : m_begin(std::move(begin)), m_end(std::move(end))
        {
        }

        template <typename Iter>
        iterator_container(Iter&& begin, Iter&& end) : m_begin(std::forward<Iter>(begin)),
                                                       m_end(std::forward<Iter>(end))
        {
        }

        auto rev() const
        {
            static_assert(meta::is_bidirectional_or_better_category_v<iterator_category>,
                          ".rev() must be used with bidirectional (or better) iterators!");
            return iterator_container<std::reverse_iterator<IterBase>>{
                std::reverse_iterator<IterBase>{end()},
                std::reverse_iterator<IterBase>{begin()}
            };
        }

        template <typename... Iter>
        auto zip(iterator_pair<Iter>... iterator_pairs) const
        {
            return zip_iterator_container(iterator_pair<decltype(begin())>{begin(), end()}, iterator_pairs...);
        }

        template <typename... Container>
        auto zip(Container&... containers) const
        {
            return zip_iterator_container(iterator_pair<decltype(begin())>{begin(), end()},
                                          iterator_pair{containers.begin(), containers.end()}...);
        }

        template <typename... Container>
        auto zip(const Container&... containers) const
        {
            return zip_iterator_container(iterator_pair<decltype(begin())>{begin(), end()},
                                          iterator_pair{containers.begin(), containers.end()}...);
        }

        template <typename Iter>
        auto zip(Iter begin, Iter end)
        {
            return zip(iterator_pair<Iter>{begin, end});
        }

        auto enumerate() const
        {
            return enumerate_iterator_container{
                begin(), end(), static_cast<blt::size_t>(std::distance(begin(), end()))
            };
        }

        auto flatten() const
        {
            return iterator_container<flatten_wrapper<IterBase, false>>{
                blt::iterator::flatten_wrapper<IterBase, false>{m_begin},
                blt::iterator::flatten_wrapper<IterBase, false>{m_end}
            };
        }

        auto flatten_all() const
        {
            return iterator_container<flatten_wrapper<IterBase, true>>{
                blt::iterator::flatten_wrapper<IterBase, true>{m_begin},
                blt::iterator::flatten_wrapper<IterBase, true>{m_end}
            };
        }

        auto as_const() const
        {
            return iterator_container<const_wrapper<IterBase>>{
                const_wrapper<IterBase>{m_begin},
                const_wrapper<IterBase>{m_end}
            };
        }

        auto move() const
        {
            return iterator_container<move_wrapper<IterBase>>{
                move_wrapper<IterBase>{m_begin},
                move_wrapper<IterBase>{m_end}
            };
        }

        template <typename Func, typename... Args>
        auto map(Func func, Args&&... args) const
        {
            return iterator_container<map_wrapper<IterBase, Func, Args...>>{
                blt::iterator::map_wrapper<IterBase, Func, Args...>{m_begin, func, std::forward<Args>(args)...},
                blt::iterator::map_wrapper<IterBase, Func, Args...>{m_end, func, std::forward<Args>(args)...}
            };
        }

        template <typename Pred, typename... Args>
        auto filter(Pred pred, Args&&... args) const
        {
            return iterator_container<filter_wrapper<IterBase, Pred, Args...>>{
                blt::iterator::filter_wrapper<IterBase, Pred, Args...>{m_begin, pred, std::forward<Args>(args)...},
                blt::iterator::filter_wrapper<IterBase, Pred, Args...>{m_end, pred, std::forward<Args>(args)...}
            };
        }

        bool any()
        {
            for (decltype(auto) val : *this)
            {
                if (val)
                    return true;
            }
            return false;
        }

        bool all()
        {
            for (decltype(auto) val : *this)
            {
                if (!val)
                    return false;
            }
            return true;
        }

        template<typename T = decltype(*std::declval<IterBase>())>
        T sum(T initial = T{})
        {
            for (decltype(auto) val : *this)
                initial += val;

            return initial;
        }

        /**
         * Fills the container using emplace with the direct output of the iterator.
         */
        template <typename Container>
        auto collect_raw(Container& container) -> decltype(auto)
        {
            for (decltype(auto) val : *this)
                container.emplace(container.end(), std::forward<decltype(val)>(val));
            return container;
        }

        /**
         * Fills an std::vector using emplace with the direct output of the iterator.
         */
        auto collect_raw()
        {
            std::vector<meta::deref_return_t<decltype(this->begin())>> container{};
            return collect_raw(container);
        }

        /**
         * Fills the container using emplace, taking only values from optionals with a value, or values from expected
         * If an expected value returns an error the expected is propagated via the return of the function.
         */
        template <typename Container>
        auto collect_flatten(Container& container) -> decltype(auto)
        {
            using return_t = meta::deref_return_t<decltype(this->begin())>;

            if constexpr (meta::is_optional_v<return_t>)
            {
                for (decltype(auto) val : *this)
                    if (val)
                        container.emplace(container.end(), std::forward<decltype(*val)>(*val));
                return container;
            }
            else if constexpr (meta::is_expected_v<return_t>)
            {
                // TODO: reference wrapper?
                using meta_type = meta::is_expected<return_t>;
                using new_return_type = typename meta_type::template expected_type<
                    decltype(container), meta_type::E_type>;
                for (decltype(auto) val : *this)
                {
                    if (val)
                        container.emplace(container.end(), std::forward<decltype(val.value())>(val.value()));
                    else
                        return new_return_type{typename meta_type::type::unexpected_type{val.error()}};
                }
                return new_return_type{container};
            }
            else
            {
                return collect_raw(container);
            }
        }

        /**
         * Fills the container using emplace, taking only values from optionals with a value, or values from expected
         * If an expected value returns an error the expected is propagated via the return of the function.
         */
        auto collect_flatten()
        {
            using return_t = meta::deref_return_t<decltype(this->begin())>;
            using collapsed_type = meta::expected_optional_value_t<return_t>;
            std::vector<collapsed_type> container{};
            return collect_flatten(container);
        }

        template <typename Container>
        auto collect(Container& container) -> decltype(auto)
        {
            return collect_flatten(container);
        }

        auto collect()
        {
            return collect_flatten();
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

    struct equals_t
    {
        template <typename T, typename U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return t == u;
        }
    };

    struct not_equals_t
    {
        template <typename T, typename U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return t != u;
        }
    };

    struct less_t
    {
        template <typename T, typename U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return t < u;
        }
    };

    struct greater_t
    {
        template <typename T, typename U>
        constexpr bool operator()(T&& t, U&& u) const
        {
            return t > u;
        }
    };

    template <typename T, typename U>
    constexpr auto operator||(T&& t, U&& u)
    {
        return [t=std::forward<T>(t), u=std::forward<U>(u)](auto&& v, auto&& g)
        {
            return t(v, g) || u(v, g);
        };
    }

    template <typename T, typename U>
    constexpr auto operator&&(T&& t, U&& u)
    {
        return [t=std::forward<T>(t), u=std::forward<U>(u)](auto&& v, auto&& g)
        {
            return t(v, g) && u(v, g);
        };
    }

    inline constexpr equals_t equals{};
    inline constexpr not_equals_t not_equals{};
    inline constexpr less_t less{};
    inline constexpr greater_t greater{};
}

#endif //BLT_ITERATOR_ITER_COMMON
