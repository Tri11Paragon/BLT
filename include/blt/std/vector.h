/*
 *  <Short Description>
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

#ifndef BLT_VECTOR_H
#define BLT_VECTOR_H

#include <iterator>
#include <blt/std/memory_util.h>
#include <blt/std/allocator.h>
#include <blt/compatibility.h>
#include <blt/meta/iterator.h>
#include "ranges.h"
#include <stdexcept>
#include <array>

namespace blt
{
	template <typename T, size_t MAX_SIZE>
	class static_vector
	{
	private:
		std::array<T, MAX_SIZE> buffer_;
		size_t size_ = 0;

	public:
		using value_type = T;
		using reference = T&;
		using pointer = T*;
		using const_reference = const T&;
		using const_pointer = const T*;
		using iterator = T*;
		using const_iterator = const T*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		constexpr static_vector() = default;

		constexpr explicit static_vector(const size_t size): size_(size)
		{
			if (size > MAX_SIZE)
				throw std::out_of_range("Vector cannot be initialized to size " + std::to_string(size) + " max size is " + std::to_string(MAX_SIZE));
			for (size_t i = 0; i < size; i++)
				buffer_[i] = T{};
		}

		constexpr explicit static_vector(const size_t size, const T& t): size_(size)
		{
			if (size > MAX_SIZE)
				throw std::out_of_range("Vector cannot be initialized to size " + std::to_string(size) + " max size is " + std::to_string(MAX_SIZE));
			for (size_t i = 0; i < size; i++)
				buffer_[i] = t;
		}

		template <typename InputIt, std::enable_if_t<meta::is_forward_iterator_v<InputIt> || meta::is_bidirectional_or_better_v<InputIt>, bool>  =
				true>
		constexpr static_vector(InputIt begin, InputIt end)
		{
			if (std::distance(begin, end) > MAX_SIZE)
				throw std::out_of_range(
					"Requested range cannot be fit inside this vector! Max size is " + std::to_string(MAX_SIZE) + " got " + std::to_string(
						std::distance(begin, end)) + " elements to be inserted.");
			for (size_t i = 0; begin != end; ++i, ++begin)
				buffer_[i] = *begin;
		}

		constexpr static_vector(const static_vector&) = default;
		constexpr static_vector(static_vector&&) noexcept = default;
		static_vector& operator=(const static_vector&) = default;
		static_vector& operator=(static_vector&&) noexcept = default;

		static_vector& operator=(std::initializer_list<T> list)
		{
			auto begin = list.begin();
			if (std::distance(begin, list.end()) > MAX_SIZE)
				throw std::out_of_range(
					"Requested initializer list cannot be fit inside this vector! Max size is " + std::to_string(MAX_SIZE) + " got " + std::to_string(
						std::distance(begin, list.end())) + " elements to be inserted.");
			for (size_t i = 0; begin != list.end(); ++i, ++begin)
				buffer_[i] = *begin;
			return *this;
		}

		constexpr static_vector(std::initializer_list<T> list)
		{
			auto begin = list.begin();
			if (std::distance(begin, list.end()) > MAX_SIZE)
				throw std::out_of_range(
					"Requested initializer list cannot be fit inside this vector! Max size is " + std::to_string(MAX_SIZE) + " got " + std::to_string(
						std::distance(begin, list.end())) + " elements to be inserted.");
			for (size_t i = 0; begin != list.end(); ++i, ++begin)
				buffer_[i] = *begin;
		}

		constexpr bool push_back(const T& copy)
		{
			if (size_ >= MAX_SIZE)
				return false;
			buffer_[size_++] = copy;
			return true;
		}

		constexpr bool push_back(T&& move)
		{
			if (size_ >= MAX_SIZE)
				return false;
			buffer_[size_++] = std::move(move);
			return true;
		}

		//TODO: replace with placement new / byte data
		template <typename... Args>
		constexpr bool emplace_back(Args&&... args)
		{
			if (size_ >= MAX_SIZE)
				return false;
			buffer_[size_++] = T{std::forward<Args>(args)...};
			return true;
		}

		constexpr void pop_back()
		{
			if (empty())
				throw std::out_of_range("No elements contained in vector. Cannot pop_back()");
			size_--;
		}

		constexpr void reserve(size_t size)
		{
			if (size > MAX_SIZE)
				throw std::out_of_range("Requested size cannot be reserved in this vecotor. Max size is " + std::to_string(MAX_SIZE));
			size_ = size;
		}

		[[nodiscard]] constexpr bool empty() const
		{
			return size() == 0;
		}

		constexpr inline void clear()
		{
			for (auto& v : *this)
				v = {};
			size_ = 0;
		}

		[[nodiscard]] constexpr inline size_t size() const
		{
			return size_;
		}

		[[nodiscard]] constexpr inline size_t capacity() const
		{
			return MAX_SIZE;
		}

		[[nodiscard]] constexpr inline blt::size_t max_size() const
		{
			return MAX_SIZE;
		}

		constexpr inline reference at(size_t index)
		{
			if (index >= MAX_SIZE)
				throw std::runtime_error("Array index " + std::to_string(index) + " out of bounds! (Max size: " + std::to_string(MAX_SIZE) + ')');
			return buffer_[index];
		}

		constexpr inline reference operator[](size_t index)
		{
			return buffer_[index];
		}

		constexpr inline const_reference operator[](size_t index) const
		{
			return buffer_[index];
		}

		constexpr inline reference operator*()
		{
			return *buffer_.data();
		}

		constexpr inline const_reference operator*() const
		{
			return *buffer_.data();
		}

		constexpr inline pointer data()
		{
			return buffer_.data();
		}

		constexpr inline const_pointer data() const
		{
			return buffer_.data();
		}

		constexpr inline reference front()
		{
			return data()[0];
		}

		constexpr inline const_reference front() const
		{
			return data()[0];
		}

		constexpr inline reference back()
		{
			return data()[size() - 1];
		}

		constexpr inline const_reference back() const
		{
			return data()[size() - 1];
		}

		constexpr inline iterator begin() noexcept
		{
			return data();
		}

		constexpr inline iterator end() noexcept
		{
			return data() + size();
		}

		constexpr inline const_iterator cbegin() const noexcept
		{
			return data();
		}

		constexpr inline const_iterator cend() const noexcept
		{
			return data() + size();
		}

		constexpr inline reverse_iterator rbegin() const noexcept
		{
			return reverse_iterator{end()};
		}

		constexpr inline reverse_iterator rend() const noexcept
		{
			return reverse_iterator{begin()};
		}

		constexpr inline const_iterator crbegin() const noexcept
		{
			return const_reverse_iterator{cend()};
		}

		constexpr inline reverse_iterator crend() const noexcept
		{
			return reverse_iterator{cbegin()};
		}

		template <typename G, std::enable_if_t<std::is_convertible_v<G, T>, bool>  = true>
		constexpr iterator insert(const_iterator pos, G&& ref)
		{
			blt::ptrdiff_t loc = pos - buffer_;
			if (size_ + 1 >= capacity())
			{
				BLT_ABORT("Inserting exceeds size of internal buffer!");
			}
			for (auto insert = end() - 1; (insert - buffer_) != loc - 1; insert--)
			{
				auto new_pos = insert + 1;
				*new_pos = *insert;
			}
			buffer_[loc] = ref;
			size_++;
			return buffer_ + loc;
		}

		constexpr iterator erase(const_iterator pos)
		{
			blt::ptrdiff_t loc = pos - buffer_;

			for (auto fetch = begin() + loc + 1; fetch != end(); fetch++)
			{
				auto insert = fetch - 1;
				*insert = *fetch;
			}

			size_--;
			return buffer_ + loc + 1;
		}

		constexpr iterator erase(const_iterator first, const_iterator last)
		{
			blt::ptrdiff_t first_pos = first - buffer_;
			blt::ptrdiff_t last_pos = last - buffer_;
			blt::ptrdiff_t remove_amount = last_pos - first_pos;

			for (auto fetch = begin() + last_pos, insert = begin() + first_pos; fetch != end(); fetch++, insert++)
			{
				*insert = *fetch;
			}

			size_ -= remove_amount;
			return buffer_ + first_pos + 1;
		}

		template <typename T1, blt::size_t size1, typename T2, blt::size_t size2>
		constexpr friend bool operator==(const static_vector<T1, size1>& v1, const static_vector<T2, size2>& v2)
		{
			if (v1.size() != v2.size())
				return false;
			for (blt::size_t i = 0; i < v1.size(); i++)
			{
				if (v1[i] != v2[i])
					return false;
			}
			return true;
		}
	};

	template <typename T, blt::size_t BUFFER_SIZE = 8 / sizeof(T), typename ALLOC = std::allocator<T>>
	class svo_vector
	{
	public:
		using value_type = T;
		using reference = T&;
		using pointer = T*;
		using const_reference = const T&;
		using const_pointer = const T*;
		using iterator = T*;
		using const_iterator = const T*;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		constexpr svo_vector() = default;

		template <typename G, std::enable_if_t<std::is_convertible_v<G, T>, bool>  = true>
		constexpr svo_vector(std::initializer_list<G> list)
		{
			if (list.size() > BUFFER_SIZE)
			{
				// TODO: how to avoid copy here?
				data.buffer_pointer = alloc.allocate(list.size());
				for (const auto& v : blt::enumerate(list))
					new(&data.buffer_pointer[v.first]) T(v.second);
				capacity = list.size();
				used = list.size();
			} else
			{
				for (const auto& v : blt::enumerate(list))
					new(&data.buffer[v.first]) T(v.second);
				used = list.size();
			}
		}

		template <typename G, std::enable_if_t<std::is_same_v<blt::vector<T>, G> || std::is_same_v<std::vector<T>, G>, bool>  = true>
		constexpr explicit svo_vector(G&& universal)
		{
			if (universal.size() > BUFFER_SIZE)
			{} else
			{}
		}

		[[nodiscard]] constexpr blt::size_t size() const
		{
			return used;
		}

		[[nodiscard]] constexpr bool is_heap() const
		{
			return used > BUFFER_SIZE;
		}

		BLT_CPP20_CONSTEXPR ~svo_vector()
		{
			if (is_heap())
			{
				for (blt::size_t i = 0; i < used; i++)
					data.buffer_pointer[i].~T();
				alloc.deallocate(data.buffer_pointer, capacity);
			} else
			{
				for (blt::size_t i = 0; i < used; i++)
					data.buffer[i].~T();
			}
		}

	private:
		union buffer_data
		{
			T* buffer_pointer;
			T buffer[BUFFER_SIZE];
		} data;

		ALLOC alloc;
		blt::size_t used = 0;
		blt::size_t capacity = BUFFER_SIZE;
	};
}

#endif //BLT_VECTOR_H
