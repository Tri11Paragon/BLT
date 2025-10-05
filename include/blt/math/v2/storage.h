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

#ifndef BLT_MATH_STORAGE_H
#define BLT_MATH_STORAGE_H

#include <blt/compatibility.h>
#include <blt/std/types.h>
#include <blt/math/vectors.h>

namespace blt::detail
{
    inline constexpr u32 DYNAMIC_EXTENT = static_cast<u32>(-1);

    template <u32 N>
    struct value_t
    {
        static constexpr u32 value = N;

        operator u32() const { return value; } // NOLINT
    };

    template<>
    struct value_t<DYNAMIC_EXTENT>
    {
        constexpr value_t(const u32 value) : value{value} // NOLINT
        {
        }

        operator u32() const { return value; } // NOLINT

        size_t value;
    };

    using dynamic_value_t = value_t<DYNAMIC_EXTENT>;

    template<u32 N>
    value_t(std::integral_constant<u32, N>) -> value_t<N>;

    template <typename T>
    struct dynamic_matrix_t
    {
        constexpr dynamic_matrix_t(const u32 rows, const u32 columns) : rows_(rows),
                                                                        columns_(columns)
        {
            data_ = new T[rows * columns];
        }

        constexpr dynamic_matrix_t(const dynamic_matrix_t& copy)
        {
            rows_ = copy.rows_;
            columns_ = copy.columns_;
            data_ = new T[rows_ * columns_];
            for (u32 i = 0; i < rows_ * columns_; i++)
                data_[i] = copy.data_[i];
        }

        constexpr dynamic_matrix_t(dynamic_matrix_t&& move) noexcept
        {
            rows_ = move.rows_;
            columns_ = move.columns_;
            data_ = move.data_;
            move.data_ = nullptr;
        }

        constexpr dynamic_matrix_t& operator=(const dynamic_matrix_t& copy)
        {
            if (&copy == this)
                return *this;
            rows_ = copy.rows_;
            columns_ = copy.columns_;
            delete[] data_;
            data_ = new T[rows_ * columns_];
            for (u32 i = 0; i < rows_ * columns_; i++)
                data_[i] = copy.data_[i];
            return *this;
        }

        constexpr dynamic_matrix_t& operator=(dynamic_matrix_t&& move) noexcept
        {
            if (&move == this)
                return *this;
            rows_ = move.rows_;
            columns_ = move.columns_;
            delete[] data_;
            data_ = move.data_;
            move.data_ = nullptr;
            return *this;
        }

        constexpr T* data() { return data_; }

        constexpr const T* data() const { return data_; }

        [[nodiscard]] constexpr dynamic_value_t rows() const { return rows_; }

        [[nodiscard]] constexpr dynamic_value_t columns() const { return columns_; }

        constexpr dynamic_matrix_t empty_from() const { return dynamic_matrix_t{rows(), columns()}; }

        static constexpr dynamic_matrix_t empty_from(const dynamic_value_t r, const dynamic_value_t c)
        {
            return dynamic_matrix_t{r.value, c.value};
        }

        BLT_CPP20_CONSTEXPR ~dynamic_matrix_t() { delete[] data_; }

    private:
        T* data_;
        u32 rows_;
        u32 columns_;
    };


    template <typename T, u32 Rows, u32 Columns>
    struct static_matrix_t
    {
        static_matrix_t()
        {
            std::memset(data_, 0, sizeof(T) * Rows * Columns);
        };

        static_matrix_t(std::initializer_list<T> list): data_{}
        {
            for (auto b = list.begin(); b != list.end(); ++b)
                data_[std::distance(list.begin(), b)] = *b;
        }

        static_matrix_t(std::initializer_list<static_matrix_t> list): data_{}
        {
            BLT_ASSERT(std::distance(list.begin(), list.end()) == 1);
            auto& v = *list.begin();
            auto mr = std::min(Rows, v.rows().value);
            auto mc = std::min(Columns, v.columns().value);
            std::memcpy(data_, v.data(), sizeof(T) * mr * mc);
        }

        explicit static_matrix_t(T data[Rows * Columns]) : data_{data}
        {
        }

        constexpr T* data() { return data_; }

        constexpr const T* data() const { return data_; }

        [[nodiscard]] constexpr value_t<Rows> rows() const { return value_t<Rows>{}; }

        [[nodiscard]] constexpr value_t<Columns> columns() const { return value_t<Columns>{}; }

        static_matrix_t empty_from() const { return static_matrix_t{}; }

        template <u32 R, u32 C>
        static constexpr static_matrix_t empty_from(const value_t<R> r, const value_t<C> c)
        {
            return static_matrix_t<T, r.value, c.value>{};
        }

    private:
        T data_[Rows * Columns];
    };
}

#endif //BLT_MATH_STORAGE_H
