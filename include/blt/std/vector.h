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
#include "ranges.h"
#include <stdexcept>

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
    
    template<typename T, typename ALLOC = std::allocator<T>>
    class vector
    {
        private:
            ALLOC allocator;
            T* buffer_;
            size_t capacity_ = 0;
            size_t size_ = 0;
            
            using value_type = T;
            using allocator_type = ALLOC;
            using size_type = size_t;
            using difference_type = std::ptrdiff_t;
            using reference = value_type&;
            using const_reference = const value_type&;
            using pointer = value_type*;
            using const_pointer = const pointer;
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
            
            template<typename G, std::enable_if_t<std::is_convertible_v<G, T>, bool> = true>
            constexpr vector(std::initializer_list<G>&& list): size_(list.size()), capacity_(list.size())
            {
                buffer_ = allocator.allocate(capacity_);
                for (auto e : blt::enumerate(list))
                    buffer_[e.first] = e.second;
            }
            
            template<typename G, std::enable_if_t<std::is_same_v<blt::vector<T>, G> || std::is_same_v<std::vector<T>, G>, bool> = true>
            constexpr explicit vector(const G& copy): size_(copy.size()), capacity_(copy.capacity())
            {
                buffer_ = allocator.allocate(capacity_);
                for (auto e : blt::enumerate(copy))
                    buffer_[e.first] = e.second;
            }
            
            template<typename G, std::enable_if_t<std::is_same_v<blt::vector<T>, G> || std::is_same_v<std::vector<T>, G>, bool> = true>
            constexpr explicit vector(G&& move): size_(move.size()), capacity_(move.capacity()), buffer_(move.buffer_)
            {
                move.buffer_ = nullptr;
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
            constexpr inline void emplace_back(Args&& ... args)
            {
                if (size_ >= capacity_)
                    expand();
                new(&buffer_[size_++]) T(std::forward<Args>(args)...);
            }
            
            constexpr inline T& at(size_t index)
            {
                if (index >= capacity_)
                    throw std::runtime_error(
                            "Array index " + std::to_string(index) + " out of bounds! (Max size: " + std::to_string(capacity_) + ')');
                return buffer_[index];
            }
            
            constexpr inline const T& at(size_t index) const
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
                expand(size);
            }
            
            [[nodiscard]] constexpr inline size_t size() const
            {
                return size_;
            }
            
            [[nodiscard]] constexpr inline size_t capacity() const
            {
                return capacity_;
            }
            
            constexpr inline reference front()
            {
                return *buffer_;
            }
            
            constexpr inline const_reference front() const
            {
                return *buffer_;
            }
            
            constexpr inline reference back()
            {
                return buffer_[size_ - 1];
            }
            
            constexpr inline const_reference back() const
            {
                return buffer_[size_ - 1];
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
            
            [[nodiscard]] constexpr inline bool empty() const
            {
                return size_ == 0;
            }
            
            template<typename G, std::enable_if_t<std::is_convertible_v<G, T>, bool> = true>
            constexpr iterator insert(const_iterator pos, G&& ref)
            {
                difference_type loc = pos - buffer_;
                if (size_ + 1 >= capacity_)
                    expand();
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
                difference_type loc = pos - buffer_;
                
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
                difference_type first_pos = first - buffer_;
                difference_type last_pos = last - buffer_;
                difference_type remove_amount = last_pos - first_pos;
                
                for (auto fetch = begin() + last_pos, insert = begin() + first_pos; fetch != end(); fetch++, insert++)
                {
                    *insert = *fetch;
                }
                
                size_ -= remove_amount;
                return buffer_ + first_pos + 1;
            }
            
            constexpr inline iterator begin() const noexcept
            {
                return data();
            }
            
            constexpr inline iterator end() const noexcept
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
    };
    
}

#endif //BLT_VECTOR_H
