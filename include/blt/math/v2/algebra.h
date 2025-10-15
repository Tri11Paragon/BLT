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

#ifndef BLT_MATHv2_ALGEBRA_H
#define BLT_MATHv2_ALGEBRA_H

#include <type_traits>
#include <blt/math/v2/storage.h>
#include <tuple>

#if __cplusplus >= BLT_CPP20
#include <concepts>
#endif

namespace blt
{
#if __cplusplus >= BLT_CPP20

    namespace detail
    {
        template <typename T>
        concept is_pointer = std::is_pointer_v<T>;

        template <typename T>
        concept is_const_pointer = std::is_const_v<std::remove_pointer_t<T>> && is_pointer<T>;
    }

    template <typename Storage>
    concept StorageConcept = requires(Storage& t, const Storage& ct)
    {
        { t.data() } -> detail::is_pointer;
        { ct.data() } -> detail::is_const_pointer;
        { t.rows() } -> std::convertible_to<u32>;
        { t.columns() } -> std::convertible_to<u32>;
        { ct.empty_from() } -> std::same_as<Storage>;
        { requires { Storage(ct); Storage(std::move(t)); } };
    };

#else
#define StorageConcept typename
#endif

    template <StorageConcept Storage, template<typename> typename... Dep>
    struct matrix_t;

    namespace detail
    {
        template<typename T>
        struct empty_t
        {

        };

        template <StorageConcept S1, StorageConcept>
        struct prefer_dynamic
        {
            using type = S1;
        };

        template <typename S, typename T>
        struct prefer_dynamic<S, dynamic_matrix_t<T>>
        {
            using type = dynamic_matrix_t<T>;
        };

        template <typename S1, typename S2>
        using prefer_dynamic_t = typename prefer_dynamic<S1, S2>::type;

        template <typename, typename, typename = void>
        struct allowed_t : std::true_type
        {
        };

        template <typename T, typename Storage>
        struct allowed_t<T, Storage, std::void_t<typename T::template require<Storage>>> : T::template require<Storage>
        {
        };

        template<class T>
        struct type_identity { using type = T; };

        // https://stackoverflow.com/questions/55941964/how-to-filter-duplicate-types-from-tuple-c
        template <typename T, typename...>
        struct unique : type_identity<T>
        {
        };

        template <typename... Ts, typename U, typename... Us>
        struct unique<std::tuple<Ts...>, U, Us...>
            : std::conditional_t<std::disjunction_v<std::is_same<U, Ts>...>
                                 , unique<std::tuple<Ts...>, Us...>
                                 , unique<std::tuple<Ts..., U>, Us...>>
        {
        };

        template<StorageConcept S1, StorageConcept S2>
        struct filter_matrix_tuple
        {
            template<template<typename> typename... Args>
            auto operator()(const Args<void>...) -> matrix_t<prefer_dynamic_t<S1, S2>, Args...>
            {
                return {};
            }
        };

        template <typename... Ts>
        using unique_tuple = typename unique<std::tuple<>, Ts...>::type;

        template <StorageConcept S1, StorageConcept S2,
            template<typename> typename... D>
        using common_return_t = decltype(std::apply(filter_matrix_tuple<S1, S2>{}, std::declval<unique_tuple<
                std::conditional_t<allowed_t<D<void>,
                                             prefer_dynamic_t<S1, S2>>::value, D<void>,
                                   empty_t<void>>...>>()));

        template <typename Matrix>
        struct set_identity_t
        {
            Matrix set_identity() const
            {
                auto mat = *static_cast<const Matrix*>(this);
                BLT_ASSERT(mat.rows() == mat.columns());
                std::memset(mat.data(), 0, mat.rows() * mat.columns() * sizeof(decltype(*mat.data())));
                for (u32 i = 0; i < mat.rows(); i++)
                    mat.data()[i * mat.rows() + i] = 1;
                return mat;
            }
        };

        template <typename Matrix>
        struct print_t
        {
            void print(std::ostream& stream) const
            {
                const auto& mat = *static_cast<const Matrix*>(this);
                for (u32 i = 0; i < mat.columns(); i++)
                {
                    stream << "[";
                    for (u32 j = 0; j < mat.rows(); j++)
                    {
                        stream << mat.data()[i * mat.columns() + j];
                        if (j != mat.rows() - 1)
                            stream << ' ';
                    }
                    stream << "]" << std::endl;
                }
            }
        };
    }

    template <StorageConcept Storage, template<typename> typename... Dep>
    struct matrix_t : Storage, Dep<matrix_t<Storage, Dep...>>...
    {
        using Dep<matrix_t>::Dep...;
        using Storage::Storage;

        matrix_t() = default;

        [[nodiscard]] constexpr decltype(auto) m(u32 row, u32 column) const
        {
            const auto& self = *static_cast<const Storage*>(this);
            return self.data()[column * self.columns() + row];
        };

        [[nodiscard]] constexpr decltype(auto) m(u32 row, u32 column)
        {
            auto& self = *static_cast<Storage*>(this);
            return self.data()[column * self.columns() + row];
        };

        template <typename T>
        constexpr auto m(u32 row, u32 column, T value)
        {
            auto& self = *static_cast<Storage*>(this);
            return self.data()[column * self.columns() + row] = value;
        };
    };

    template <StorageConcept S1,
        template<typename> typename... D1, StorageConcept S2, template<typename> typename... D2, typename Return =
        detail::common_return_t<S1, S2, D1..., D2...>>
    static auto operator*(const matrix_t<S1, D1...>& a,
                          const matrix_t<S2, D2...>& b) -> Return
    {
        BLT_ASSERT(a.columns() == b.rows());
        Return ret{Return::empty_from(detail::value_t{a.rows()}, detail::value_t{b.columns()})};

        for (u32 i = 0; i < a.rows(); i++)
        {
            for (u32 j = 0; j < b.columns(); j++)
            {
                for (u32 k = 0; k < a.columns(); k++)
                {
                    ret.m(i, j) = ret.m(i, j) + a.m(i, k) * b.m(k, j);
                }
            }
        }

        return ret;
    }
}

#if __cplusplus < BLT_CPP20
#undef StorageConcept
#endif

#endif //BLT_MATHv2_ALGEBRA_H
