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

#ifndef BLT_STD_VARIANT_H
#define BLT_STD_VARIANT_H

#include <functional>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <variant>
#include <blt/std/types.h>

namespace blt
{
    /*
     * std::visit(blt::lambda_visitor{
     *      lambdas...
     * }, data_variant);
     */

    template <typename... TLambdas>
    struct lambda_visitor : TLambdas...
    {
        using TLambdas::operator()...;
    };

#if __cplusplus < 202002L

    // explicit deduction guide (not needed as of C++20)
    template <typename... TLambdas>
    lambda_visitor(TLambdas...) -> lambda_visitor<TLambdas...>;

#endif

    template <typename... Types>
    class variant_t
    {
    public:
        using value_type = std::variant<Types...>;
        size_t variant_size = sizeof...(Types);

        constexpr variant_t(): m_variant()
        {
        }

        constexpr variant_t(const variant_t& variant) noexcept(std::is_nothrow_copy_constructible_v<value_type>): m_variant(variant.m_variant)
        {
        }

        constexpr variant_t(variant_t&& variant) noexcept(std::is_nothrow_move_constructible_v<value_type>): m_variant(std::move(variant.m_variant))
        {
        }

        explicit constexpr variant_t(const value_type& variant) noexcept(std::is_nothrow_copy_constructible_v<value_type>): m_variant(variant)
        {
        }

        explicit constexpr variant_t(value_type&& variant) noexcept(std::is_nothrow_move_constructible_v<value_type>): m_variant(std::move(variant))
        {
        }

        explicit constexpr variant_t(Types&&... args) noexcept(std::is_nothrow_constructible_v<value_type, Types...>): m_variant(
            std::forward<Types>(args)...)
        {
        }

        template <typename T, typename... C_Args>
        explicit constexpr variant_t(std::in_place_type_t<T>, C_Args&&... args): m_variant(std::in_place_type<T>, std::forward<C_Args>(args)...)
        {
        }

        template <typename T, typename U, typename... C_Args>
        constexpr explicit variant_t(std::in_place_type_t<T>, std::initializer_list<U> il, C_Args&&... args): m_variant(
            std::in_place_type<T>, il, std::forward<C_Args>(args)...)
        {
        }

        template <size_t I, typename... C_Args>
        explicit constexpr variant_t(std::in_place_index_t<I>, C_Args&&... args): m_variant(std::in_place_index<I>, std::forward<C_Args>(args)...)
        {
        }

        template <std::size_t I, typename U, typename... C_Args>
        constexpr explicit variant_t(std::in_place_index_t<I>, std::initializer_list<U> il, C_Args&&... args): m_variant(
            std::in_place_index<I>, il, std::forward<C_Args>(args)...)
        {
        }

        template <typename T, typename... Args>
        T& emplace(Args&&... args)
        {
            return m_variant.template emplace<T>(std::forward<Args>(args)...);
        }

        template <typename T, typename U, typename... Args>
        T& emplace(std::initializer_list<U> il, Args&&... args)
        {
            return m_variant.template emplace<T>(il, std::forward<Args>(args)...);
        }

        template <std::size_t I, typename... Args>
        std::variant_alternative_t<I, value_type>& emplace(Args&&... args)
        {
            return m_variant.template emplace<I>(std::forward<Args>(args)...);
        }

        template <std::size_t I, typename U, typename... Args>
        std::variant_alternative_t<I, value_type>& emplace(std::initializer_list<U> il, Args&&... args)
        {
            return m_variant.template emplace<I>(il, std::forward<Args>(args)...);
        }

        [[nodiscard]] constexpr std::size_t index() const noexcept
        {
            return m_variant.index();
        }

        [[nodiscard]] constexpr bool valueless_by_exception() const noexcept
        {
            return m_variant.valueless_by_exception();
        }

        template <typename T>
        constexpr auto visit(T&& visitor) -> decltype(auto)
        {
            return std::visit(std::forward<T>(visitor), m_variant);
        }

        /**
         * Automatic visitor generation with empty default behavior
         * @param visitees user lambdas
         */
        template <typename... Visitee>
        constexpr void visit_empty(Visitee&&... visitees)
        {
            std::visit(lambda_visitor{
                           std::forward<Visitee>(visitees)...,
                           [](auto)
                           {
                           }
                       }, m_variant);
        }

        template <typename Default, typename... Visitee>
        constexpr auto visit_value(Default&& default_value, Visitee&&... visitees) -> decltype(auto)
        {
            return std::visit(lambda_visitor{
                std::forward<Visitee>(visitees)...,
                [default_value=std::forward<Default>(default_value)](auto&& value)
                {
                    return std::forward<decltype(value)>(value);
                }
            });
        }

        template <typename Default, typename... Visitee>
        constexpr auto visit_lambda(Default&& default_lambda, Visitee&&... visitees) -> decltype(auto)
        {
            return std::visit(lambda_visitor{
                std::forward<Visitee>(visitees)...,
                [default_lambda=std::forward<Default>(default_lambda)](auto&& value)
                {
                    return std::forward<Default>(default_lambda)(std::forward<decltype(value)>(value));
                }
            });
        }

        /**
         * Call a set of member functions on the types stored in the variant. If a type has more than one of these functions declared on it,
         * the implementation will use the first member function provided. By default, if the stored value doesn't have any of the member functions,
         * nothing will happen, if should_throw boolean is true, then the implementation will throw a runtime error.
         * @tparam should_throw Controls if the implementation should throw if the type stored in the variant doesn't have any matching member function
         * @return Result of calling the member functions. All functions should return the same value, otherwise this won't compile.
         */
        template <bool should_throw = false, typename... MemberFuncs>
        constexpr auto call_member(const MemberFuncs... funcs)
        {
            return std::visit([=](auto&& value)
            {
                using ValueType = std::decay_t<decltype(value)>;

                if constexpr (std::disjunction_v<std::is_invocable<std::decay_t<decltype(funcs)>, ValueType>...> && should_throw)
                {
                    throw std::runtime_error("No matching member function found");
                }

                return *(... || ([&](auto&& func) -> decltype(auto)
                {
                    using FuncType = std::decay_t<decltype(func)>;
                    using ReturnType = std::invoke_result_t<FuncType, ValueType>;
                    if constexpr (std::is_invocable_v<FuncType, ValueType>)
                    {
                        return std::make_optional(std::invoke(func, value));
                    }
                    return std::optional<ReturnType>{};
                }(std::forward<decltype(funcs)>(funcs))));
            }, m_variant);
        }


        template <size_t I>
        [[nodiscard]] constexpr bool has_index() const noexcept
        {
            return m_variant.index() == I;
        }

        template <typename T>
        [[nodiscard]] constexpr bool has_type() const noexcept
        {
            return std::holds_alternative<T>(m_variant);
        }

        template <typename T>
        [[nodiscard]] constexpr auto get() -> decltype(auto)
        {
            return std::get<T>(m_variant);
        }

        template <typename T>
        [[nodiscard]] constexpr auto get() const -> decltype(auto)
        {
            return std::get<T>(m_variant);
        }

        template <size_t I>
        [[nodiscard]] constexpr auto get() -> decltype(auto)
        {
            return std::get<I>(m_variant);
        }

        template <size_t I>
        [[nodiscard]] constexpr auto get() const -> decltype(auto)
        {
            return std::get<I>(m_variant);
        }

        template <size_t I>
        constexpr std::add_pointer_t<std::variant_alternative_t<I, value_type>> get_if() noexcept
        {
            return std::get_if<I>(m_variant);
        }

        template <size_t I>
        constexpr std::add_pointer_t<const std::variant_alternative_t<I, value_type>> get_if() noexcept
        {
            return std::get_if<I>(m_variant);
        }

        template <typename T>
        constexpr std::add_pointer_t<T> get_if() noexcept
        {
            return std::get_if<T>(m_variant);
        }

        template <typename T>
        constexpr std::add_pointer_t<const T> get_if() noexcept
        {
            return std::get_if<T>(m_variant);
        }

        template <typename T>
        constexpr T value_or(T&& t) const
        {
            if (has_type<T>())
                return get<T>();
            return std::forward<T>(t);
        }

        template <size_t I>
        constexpr std::variant_alternative_t<I, value_type> value_or(const std::variant_alternative_t<I, value_type>& t) const
        {
            if (has_type<std::variant_alternative_t<I, value_type>>())
                return get<I>();
            return t;
        }

        template <size_t I>
        constexpr std::variant_alternative_t<I, value_type> value_or(std::variant_alternative_t<I, value_type>&& t) const
        {
            if (has_type<std::variant_alternative_t<I, value_type>>())
                return get<I>();
            return t;
        }

        template <size_t>
        constexpr const value_type& variant() const
        {
            return m_variant;
        }

        constexpr value_type& variant()
        {
            return m_variant;
        }

        [[nodiscard]] constexpr size_t size() const
        {
            return variant_size;
        }

        friend bool operator==(const variant_t& lhs, const variant_t& rhs)
        {
            return lhs.m_variant == rhs.m_variant;
        }

        friend bool operator!=(const variant_t& lhs, const variant_t& rhs)
        {
            return lhs.m_variant != rhs.m_variant;
        }

        friend bool operator<(const variant_t& lhs, const variant_t& rhs)
        {
            return lhs.m_variant < rhs.m_variant;
        }

        friend bool operator>(const variant_t& lhs, const variant_t& rhs)
        {
            return lhs.m_variant > rhs.m_variant;
        }

        friend bool operator<=(const variant_t& lhs, const variant_t& rhs)
        {
            return lhs.m_variant <= rhs.m_variant;
        }

        friend bool operator>=(const variant_t& lhs, const variant_t& rhs)
        {
            return lhs.m_variant >= rhs.m_variant;
        }

    private:
        value_type m_variant;
    };

    namespace detail
    {
        template <typename>
        class variant_is_base_of
        {
        };

        template <typename... Types>
        class variant_is_base_of<variant_t<Types...>>
        {
        public:
            using value_type = bool;
            template <typename T>
            static constexpr bool value = std::conjunction_v<std::is_base_of<T, Types>...>;
        };

        template <typename... Types>
        class variant_is_base_of<std::variant<Types...>>
        {
        public:
            using value_type = bool;
            template <typename T>
            static constexpr bool value = std::conjunction_v<std::is_base_of<T, Types>...>;
        };

        template <typename T>
        static constexpr bool variant_is_base_of_v = variant_is_base_of<T>::value;
    }
}

#endif //BLT_STD_VARIANT_H
