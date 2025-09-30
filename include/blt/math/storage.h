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

#include <blt/std/types.h>
#include <blt/math/vectors.h>

namespace blt::detail
{
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
			rows_    = copy.rows_;
			columns_ = copy.columns_;
			data_    = new T[rows_ * columns_];
			for (u32 i   = 0; i < rows_ * columns_; i++)
				data_[i] = copy.data_[i];
		}

		constexpr dynamic_matrix_t(dynamic_matrix_t&& move) noexcept
		{
			rows_      = move.rows_;
			columns_   = move.columns_;
			data_      = move.data_;
			move.data_ = nullptr;
		}

		constexpr dynamic_matrix_t& operator=(const dynamic_matrix_t& copy)
		{
			if (&copy == this)
				return *this;
			rows_    = copy.rows_;
			columns_ = copy.columns_;
			delete[] data_;
			data_ = new T[rows_ * columns_];
			for (u32 i   = 0; i < rows_ * columns_; i++)
				data_[i] = copy.data_[i];
			return *this;
		}

		constexpr dynamic_matrix_t& operator=(dynamic_matrix_t&& move) noexcept
		{
			if (&move == this)
				return *this;
			rows_    = move.rows_;
			columns_ = move.columns_;
			delete[] data_;
			data_      = move.data_;
			move.data_ = nullptr;
			return *this;
		}

		constexpr T* data() { return data_; }

		constexpr T* data() const { return data_; }

		[[nodiscard]] constexpr u32 rows() const { return rows_; }

		[[nodiscard]] constexpr u32 columns() const { return columns_; }

		dynamic_matrix_t empty_from() const { return dynamic_matrix_t{rows(), columns()}; }

		constexpr ~dynamic_matrix_t() { delete[] data_; }

	private:
		T*  data_;
		u32 rows_;
		u32 columns_;
	};


	template <typename T, u32 Rows, u32 Columns>
	struct static_matrix_t
	{
		static_matrix_t() = default;

		explicit static_matrix_t(T data[Rows * Columns]): data_{data}
		{}

		constexpr T* data() { return data_; }

		constexpr T* data() const { return data_; }

		[[nodiscard]] constexpr u32 rows() const { return Rows; }

		[[nodiscard]] constexpr u32 columns() const { return Columns; }

		static_matrix_t empty_from() const { return static_matrix_t{}; }

	private:
		T data_[Rows * Columns];
	};

	template<typename T>
	struct dynamic_vector_t
	{
		constexpr explicit dynamic_vector_t(const u32 size): size_{size}
		{
			data_ = new T[size];
		}

		constexpr dynamic_vector_t(const dynamic_vector_t& copy)
		{
			data_ = new T[copy.size_];
			size_ = copy.size_;
			for (u32 i = 0; i < size_; i++)
				data_[i] = copy.data_[i];
		}

		constexpr dynamic_vector_t(dynamic_vector_t&& move) noexcept
		{
			data_ = move.data_;
			size_ = move.size_;
			move.data_ = nullptr;
		}

		constexpr dynamic_vector_t& operator=(const dynamic_vector_t& copy)
		{
			if (this == &copy)
				return *this;
			delete[] data_;
			data_ = new T[copy.size_];
			size_ = copy.size_;
			for (u32 i = 0; i < size_; i++)
				data_[i] = copy.data_[i];
			return *this;
		}

		constexpr dynamic_vector_t& operator=(dynamic_vector_t&& move) noexcept
		{
			if (this == &move)
				return *this;
			delete[] data_;
			data_ = move.data_;
			size_ = move.size_;
			move.data_ = nullptr;
			return *this;
		}

		constexpr T* data() { return data_; }

		constexpr T* data() const { return data_; }

		[[nodiscard]] constexpr u32 size() const { return size_; }

		dynamic_vector_t empty_from() const { return dynamic_vector_t{size()}; }

		constexpr ~dynamic_vector_t()
		{
			delete[] data_;
		}
	private:
		T* data_;
		u32 size_;
	};

	template<typename T, u32 Size>
	struct static_vector_t
	{
		static_vector_t() = default;

		explicit static_vector_t(T data[Size]): data_{data}
		{
		}

		constexpr T* data() { return data_; }

		constexpr T* data() const { return data_; }

		[[nodiscard]] constexpr u32 size() const { return Size; }

		static_vector_t empty_from() const { return static_vector_t{}; }
	private:
		T data_[Size];
	};
}

#endif //BLT_MATH_STORAGE_H
