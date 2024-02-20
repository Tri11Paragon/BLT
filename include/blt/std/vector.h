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

namespace blt
{
    
    template<typename T, size_t MAX_SIZE>
    class static_vector
    {
        private:
            T buffer_[MAX_SIZE];
            size_t size_ = 0;
            
            using iterator = T*;
            using const_iterator = const T*;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        public:
            constexpr static_vector() = default;
            
            constexpr inline bool push_back(const T& copy)
            {
                if (size_ >= MAX_SIZE)
                    return false;
                buffer_[size_++] = copy;
                return true;
            }
            
            constexpr inline bool push_back(T&& move)
            {
                if (size_ >= MAX_SIZE)
                    return false;
                buffer_[size_++] = std::move(move);
                return true;
            }
            
            constexpr inline T& at(size_t index)
            {
                if (index >= MAX_SIZE)
                    throw std::runtime_error("Array index " + std::to_string(index) + " out of bounds! (Max size: " + std::to_string(MAX_SIZE) + ')');
                return buffer_[index];
            }
            
            constexpr inline T& operator[](size_t index)
            {
                return buffer_[index];
            }
            
            constexpr inline const T& operator[](size_t index) const
            {
                return buffer_[index];
            }
            
            constexpr inline void reserve(size_t size)
            {
                if (size > MAX_SIZE)
                    size = MAX_SIZE;
                size_ = size;
            }
            
            [[nodiscard]] constexpr inline size_t size() const
            {
                return size_;
            }
            
            [[nodiscard]] constexpr inline size_t capacity() const
            {
                return MAX_SIZE;
            }
            
            constexpr inline T* data()
            {
                return buffer_;
            }
            
            constexpr inline T* operator*()
            {
                return buffer_;
            }
            
            constexpr inline T* data() const
            {
                return buffer_;
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
            
            constexpr inline reverse_iterator rbegin() noexcept
            {
                return reverse_iterator{end()};
            }
            
            constexpr inline reverse_iterator rend() noexcept
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
    };
    
    template<typename T, typename ALLOC>
    class vector
    {
        private:
            ALLOC allocator;
            T* buffer_;
            size_t capacity_ = 0;
            size_t size_ = 0;
            
            using iterator = T*;
            using const_iterator = const T*;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator = std::reverse_iterator<const_iterator>;
            
            constexpr inline void expand(size_t new_size = 0)
            {
                if (new_size == 0)
                    new_size = blt::mem::next_byte_allocation(capacity_);
                auto new_buffer = allocator.allocate(new_size);
                for (size_t i = 0; i < size_; i++)
                    new_buffer[i] = buffer_[i];
                allocator.deallocate(buffer_, capacity_);
                buffer_ = new_buffer;
                capacity_ = new_size;
            }
        
        public:
            constexpr vector(): capacity_(16)
            {
                buffer_ = allocator.allocate(capacity_);
            }
            
            constexpr explicit vector(size_t capacity): capacity_(capacity)
            {
                buffer_ = allocator.allocate(capacity_);
            }
            
            ~vector()
            {
                allocator.deallocate(buffer_, capacity_);
            }
            
            constexpr inline void push_back(const T& copy)
            {
                if (size_ >= capacity_)
                    expand();
                buffer_[size_++] = copy;
            }
            
            constexpr inline void push_back(T&& move)
            {
                if (size_ >= capacity_)
                    expand();
                buffer_[size_++] = std::move(move);
            }
            
            template<typename... Args>
            constexpr inline void emplace_back(Args&&... args)
            {
                if (size_ >= capacity_)
                    expand();
                buffer_[size_++] = T{std::forward<Args>(args)...};
            }
            
            constexpr inline T& at(size_t index)
            {
                if (index >= capacity_)
                    throw std::runtime_error(
                            "Array index " + std::to_string(index) + " out of bounds! (Max size: " + std::to_string(capacity_) + ')');
                return buffer_[index];
            }
            
            constexpr inline T& operator[](size_t index)
            {
                return buffer_[index];
            }
            
            constexpr inline const T& operator[](size_t index) const
            {
                return buffer_[index];
            }
            
            constexpr inline void reserve(size_t size)
            {
                expand(size_);
            }
            
            [[nodiscard]] constexpr inline size_t size() const
            {
                return size_;
            }
            
            [[nodiscard]] constexpr inline size_t capacity() const
            {
                return capacity_;
            }
            
            constexpr inline T* data()
            {
                return buffer_;
            }
            
            constexpr inline T* operator*()
            {
                return buffer_;
            }
            
            constexpr inline T* data() const
            {
                return buffer_;
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
            
            constexpr inline reverse_iterator rbegin() noexcept
            {
                return reverse_iterator{end()};
            }
            
            constexpr inline reverse_iterator rend() noexcept
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
    };
    
}

#endif //BLT_VECTOR_H
