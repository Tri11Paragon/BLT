/*
 *  <Short Description>
 *  Copyright (C) 2023  Brett Terpstra
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

#ifndef BLT_ALLOCATOR_H

#include <optional>
#include <limits>
#include <vector>
#include <blt/std/utility.h>
#include <blt/std/types.h>
#include <blt/compatibility.h>
#include <stdexcept>
#include "logging.h"

namespace blt
{
    
    template<typename value_type, typename pointer, typename const_pointer>
    class allocator_base
    {
        public:
            template<class U, class... Args>
            inline void construct(U* p, Args&& ... args)
            {
                ::new((void*) p) U(std::forward<Args>(args)...);
            }
            
            template<class U>
            inline void destroy(U* p)
            {
                if (p != nullptr)
                    p->~U();
            }
            
            [[nodiscard]] inline size_t max_size() const
            {
                return std::numeric_limits<size_t>::max();
            }
            
            inline const_pointer address(const value_type& val)
            {
                return std::addressof(val);
            }
            
            inline pointer address(value_type& val)
            {
                return std::addressof(val);
            }
    };
    
    template<typename T, size_t BLOCK_SIZE = 8192>
    class area_allocator : public allocator_base<T, T*, const T*>
    {
        public:
            using value = T;
            using type = T;
            using value_type = type;
            using pointer = type*;
            using const_pointer = const type*;
            using void_pointer = void*;
            using const_void_pointer = const void*;
            using reference = value_type&;
            using const_reference = const value_type&;
            using size_type = size_t;
            using difference_type = size_t;
            using propagate_on_container_move_assignment = std::false_type;
            template<class U>
            struct rebind
            {
                typedef blt::area_allocator<U, BLOCK_SIZE> other;
            };
            using allocator_base<value_type, pointer, const_pointer>::allocator_base;
        private:
            /**
             * Stores a view to a region of memory that has been deallocated
             * This is a non-owning reference to the memory block
             *
             * pointer p is the pointer to the beginning of the block of memory
             * size_t n is the number of elements that this block can hold
             */
            struct pointer_view
            {
                pointer p;
                size_t n;
            };
            
            /**
             * Stores the actual data for allocated blocks. Since we would like to be able to allocate an arbitrary number of items
             * we need a way of storing that data. The block storage holds an owning pointer to a region of memory with used elements
             * Only up to used has to have their destructors called, which should be handled by the deallocate function
             * it is UB to not deallocate memory allocated by this allocator
             *
             * an internal vector is used to store the regions of memory which have been deallocated. the allocate function will search for
             * free blocks with sufficient size in order to maximize memory usage. In the future more advanced methods should be used
             * for both faster access to deallocated blocks of sufficient size and to ensure coherent memory.
             */
            struct block_storage
            {
                pointer data;
                size_t used = 0;
                // TODO: b-tree?
                std::vector<pointer_view> unallocated_blocks;
            };
            
            /**
             * Stores an index to a pointer_view along with the amount of memory leftover after the allocation
             * it also stores the block being allocated to in question. The new inserted leftover should start at old_ptr + size
             */
            struct block_view
            {
                block_storage* blk;
                size_t index;
                size_t leftover;
                
                block_view(block_storage* blk, size_t index, size_t leftover): blk(blk), index(index), leftover(leftover)
                {}
            };
            
            /**
             * Allocate a new block of memory and push it to the back of blocks.
             */
            inline void allocate_block()
            {
                //BLT_INFO("Allocating a new block of size %d", BLOCK_SIZE);
                auto* blk = new block_storage();
                blk->data = static_cast<pointer>(malloc(sizeof(T) * BLOCK_SIZE));
                blocks.push_back(blk);
            }
            
            /**
             * Searches for a free block inside the block storage with sufficient space and returns an optional view to it
             * The optional will be empty if no open block can be found.
             */
            inline std::optional<block_view> search_for_block(block_storage* blk, size_t n)
            {
                for (auto kv : blt::enumerate(blk->unallocated_blocks))
                {
                    if (kv.second.n >= n)
                        return block_view{blk, kv.first, kv.second.n - n};
                }
                return {};
            }
            
            /**
             * removes the block of memory from the unallocated_blocks storage in the underlying block, inserting a new unallocated block if
             * there was any leftover. Returns a pointer to the beginning of the new block.
             */
            inline pointer swap_pop_resize_if(const block_view& view, size_t n)
            {
                pointer_view ptr = view.blk->unallocated_blocks[view.index];
                std::iter_swap(view.blk->unallocated_blocks.begin() + view.index, view.blk->unallocated_blocks.end() - 1);
                view.blk->unallocated_blocks.pop_back();
                if (view.leftover > 0)
                    view.blk->unallocated_blocks.push_back({ptr.p + n, view.leftover});
                return ptr.p;
            }
            
            /**
             * Finds the next available unallocated block of memory, or empty if there is none which meet size requirements
             */
            inline std::optional<pointer> find_available_block(size_t n)
            {
                for (auto* blk : blocks)
                {
                    if (auto view = search_for_block(blk, n))
                        return swap_pop_resize_if(view.value(), n);
                }
                return {};
            }
            
            /**
             * returns a pointer to a block of memory along with an offset into that pointer that the requested block can be found at
             */
            inline std::pair<pointer, size_t> getBlock(size_t n)
            {
                if (auto blk = find_available_block(n))
                    return {blk.value(), 0};
                
                if (blocks.back()->used + n > BLOCK_SIZE)
                    allocate_block();
                
                auto ptr = std::pair<pointer, size_t>{blocks.back()->data, blocks.back()->used};
                blocks.back()->used += n;
                return ptr;
            }
            
            /**
             * Calls the constructor on elements if they require construction, otherwise constructor will not be called and this function is useless
             *
             * ALLOCATORS RETURN UNINIT STORAGE!! THIS HAS BEEN DISABLED.
             */
            inline void allocate_in_block(pointer, size_t)
            {
//                if constexpr (std::is_default_constructible_v<T> && !std::is_trivially_default_constructible_v<T>)
//                {
//                    for (size_t i = 0; i < n; i++)
//                        new(&begin[i]) T();
//                }
            }
        
        public:
            area_allocator()
            {
                allocate_block();
            }
            
            area_allocator(const area_allocator& copy) = delete;
            
            area_allocator(area_allocator&& move) noexcept
            {
                blocks = move.blocks;
            }
            
            area_allocator& operator=(const area_allocator& copy) = delete;
            
            area_allocator& operator=(area_allocator&& move) noexcept
            {
                std::swap(move.blocks, blocks);
            }
            
            [[nodiscard]] pointer allocate(size_t n)
            {
                if (n > BLOCK_SIZE)
                    throw std::runtime_error("Requested allocation is too large!");
                
                auto block_info = getBlock(n);
                
                auto* ptr = &block_info.first[block_info.second];
                // call constructors on the objects if they require it
                allocate_in_block(ptr, n);
                
                return ptr;
            }
            
            void deallocate(pointer p, size_t n) noexcept
            {
                if (p == nullptr)
                    return;
//                for (size_t i = 0; i < n; i++)
//                    p[i].~T();
                for (auto*& blk : blocks)
                {
                    if (p >= blk->data && p <= (blk->data + BLOCK_SIZE))
                    {
                        blk->unallocated_blocks.push_back(pointer_view{p, n});
                        break;
                    }
                }
            }
            
            ~area_allocator()
            {
                for (auto*& blk : blocks)
                {
                    free(blk->data);
                    delete blk;
                }
            }
        
        private:
            std::vector<block_storage*> blocks;
    };

//    template<typename T>
//    class bump_allocator : public allocator_base<T, T*, const T*>
//    {
//        public:
//            using value = T;
//            using type = T;
//            using value_type = type;
//            using pointer = type*;
//            using const_pointer = const type*;
//            using void_pointer = void*;
//            using const_void_pointer = const void*;
//            using reference = value_type&;
//            using const_reference = const value_type&;
//            using size_type = size_t;
//            using difference_type = size_t;
//            using propagate_on_container_move_assignment = std::false_type;
//            template<class U>
//            struct rebind
//            {
//                typedef blt::bump_allocator<U> other;
//            };
//            using allocator_base<value_type, pointer, const_pointer>::allocator_base;
//        private:
//            pointer buffer_;
//            blt::size_t offset_;
//            blt::size_t size_;
//        public:
//            explicit bump_allocator(blt::size_t size): buffer_(static_cast<pointer>(malloc(size * sizeof(T)))), offset_(0), size_(size)
//            {}
//
//            template<typename... Args>
//            explicit bump_allocator(blt::size_t size, Args&& ... defaults):
//                    buffer_(static_cast<pointer>(malloc(size * sizeof(type)))), offset_(0), size_(size)
//            {
//                for (blt::size_t i = 0; i < size_; i++)
//                    ::new(&buffer_[i]) T(std::forward<Args>(defaults)...);
//            }
//
//            bump_allocator(pointer buffer, blt::size_t size): buffer_(buffer), offset_(0), size_(size)
//            {}
//
//            bump_allocator(const bump_allocator& copy) = delete;
//
//            bump_allocator(bump_allocator&& move) noexcept
//            {
//                buffer_ = move.buffer_;
//                size_ = move.size_;
//                offset_ = move.offset_;
//            }
//
//            bump_allocator& operator=(const bump_allocator& copy) = delete;
//
//            bump_allocator& operator=(bump_allocator&& move) noexcept
//            {
//                std::swap(move.buffer_, buffer_);
//                std::swap(move.size_, size_);
//                std::swap(move.offset_, offset_);
//            }
//
//            pointer allocate(blt::size_t n)
//            {
//                auto nv = offset_ + n;
//                if (nv > size_)
//                    throw std::bad_alloc();
//                pointer b = &buffer_[offset_];
//                offset_ = nv;
//                return b;
//            }
//
//            void deallocate(pointer, blt::size_t)
//            {}
//
//            ~bump_allocator()
//            {
//                free(buffer_);
//            }
//    };
    
    /**
     * The bump allocator is meant to be a faster area allocator which will only allocate forward through either a supplied buffer or size
     * or will create a linked list type data structure of buffered blocks.
     * @tparam ALLOC allocator to use for any allocations. In the case of the non-linked variant, this will be used if a size is supplied. The supplied buffer must be allocated with this allocator!
     * @tparam linked use a linked list to allocate with the allocator or just use the supplied buffer and throw an exception of we cannot allocate
     */
    template<bool linked, template<typename> typename ALLOC = std::allocator>
    class bump_allocator;
    
    template<template<typename> typename ALLOC>
    class bump_allocator<false, ALLOC>
    {
        private:
            ALLOC<blt::u8> allocator;
            blt::u8* buffer_;
            blt::u8* offset_;
            blt::size_t size_;
        public:
            explicit bump_allocator(blt::size_t size): buffer_(static_cast<blt::u8*>(allocator.allocate(size))), offset_(buffer_), size_(size)
            {}
            
            explicit bump_allocator(blt::u8* buffer, blt::size_t size): buffer_(buffer), offset_(buffer), size_(size)
            {}
            
            template<typename T>
            [[nodiscard]] T* allocate()
            {
                size_t remaining_num_bytes = size_ - static_cast<size_t>(buffer_ - offset_);
                auto pointer = static_cast<void*>(offset_);
                const auto aligned_address = std::align(alignof(T), sizeof(T), pointer, remaining_num_bytes);
                if (aligned_address == nullptr)
                    throw std::bad_alloc{};
                offset_ = static_cast<blt::u8*>(aligned_address) + sizeof(T);
                return static_cast<T*>(aligned_address);
            }
            
            template<typename T, typename... Args>
            [[nodiscard]] T* emplace(Args&& ... args)
            {
                const auto allocated_memory = allocate<T>();
                return new(allocated_memory) T{std::forward<Args>(args)...};
            }
            
            template<class U, class... Args>
            inline void construct(U* p, Args&& ... args)
            {
                ::new((void*) p) U(std::forward<Args>(args)...);
            }
            
            template<class U>
            inline void destroy(U* p)
            {
                if (p != nullptr)
                    p->~U();
            }
            
            ~bump_allocator()
            {
                allocator.deallocate(buffer_, size_);
            }
    };
    
    template<template<typename> typename ALLOC>
    class bump_allocator<true, ALLOC>
    {
        private:
            struct block
            {
                blt::u8* buffer = nullptr;
                blt::size_t offset = 0;
                blt::size_t allocated_objects = 0;
                blt::size_t deallocated_objects = 0;
            };
            ALLOC<blt::u8> allocator;
            std::vector<block, ALLOC<block>> blocks;
            blt::size_t size_;
            
            void expand()
            {
                BLT_INFO("I have expanded!");
                blocks.push_back({static_cast<blt::u8*>(allocator.allocate(size_)), 0});
            }
            
            template<typename T>
            T* allocate_back()
            {
                auto& back = blocks.back();
                size_t remaining_bytes = size_ - back.offset;
                auto void_ptr = reinterpret_cast<void*>(&back.buffer[back.offset]);
                auto new_ptr = static_cast<blt::u8*>(std::align(alignof(T), sizeof(T), void_ptr, remaining_bytes));
                if (new_ptr == nullptr)
                    expand();
                else
                {
                    back.offset += (back.buffer - new_ptr + sizeof(T));
                    back.allocated_objects++;
                }
                return static_cast<T*>(new_ptr);
            }
        
        public:
            /**
             * @param size of the list blocks
             */
            explicit bump_allocator(blt::size_t size): size_(size)
            {
                expand();
            }
            
            template<typename T>
            [[nodiscard]] T* allocate()
            {
                if (blocks.back().offset + sizeof(T) > size_)
                    expand();
                if (auto ptr = allocate_back<T>(); ptr == nullptr)
                {
                    BLT_INFO("Not enough space for me");
                    expand();
                } else
                    return ptr;
                if (auto ptr = allocate_back<T>(); ptr == nullptr)
                    throw std::bad_alloc();
                else
                    return ptr;
            }
            
            template<typename T>
            void deallocate(T* p)
            {
                auto* ptr = static_cast<blt::u8*>(p);
                blt::i64 remove_index = -1;
                for (auto e : blt::enumerate(blocks))
                {
                    auto& block = e.second;
                    if (ptr >= block.buffer && ptr <= &block.buffer[block.offset])
                    {
                        block.deallocated_objects++;
                        if (block.deallocated_objects == block.allocated_objects)
                            remove_index = static_cast<blt::i64>(e.first);
                        break;
                    }
                }
                if (remove_index < 0)
                    return;
                std::iter_swap(blocks.begin() + remove_index, blocks.end() - 1);
                allocator.deallocate(blocks.back().buffer, size_);
                BLT_DEBUG("I have freed a block!");
                blocks.pop_back();
            }
            
            template<typename T, typename... Args>
            [[nodiscard]] T* emplace(Args&& ... args)
            {
                const auto allocated_memory = allocate<T>();
                return new(allocated_memory) T{std::forward<Args>(args)...};
            }
            
            template<class U, class... Args>
            inline void construct(U* p, Args&& ... args)
            {
                ::new((void*) p) U(std::forward<Args>(args)...);
            }
            
            template<class U>
            inline void destroy(U* p)
            {
                if (p != nullptr)
                    p->~U();
            }
            
            ~bump_allocator()
            {
                for (auto& v : blocks)
                    allocator.deallocate(v.buffer, size_);
            }
    };
    
    template<typename T>
    class constexpr_allocator
    {
        public:
            constexpr constexpr_allocator() = default;
            
            constexpr T* allocate(blt::size_t n)
            {
                return ::new T[n];
            }
            
            constexpr void deallocate(T* t, blt::size_t)
            {
                ::delete[] t;
            }
            
            BLT_CPP20_CONSTEXPR ~constexpr_allocator() = default;
    };
}

#define BLT_ALLOCATOR_H

#endif //BLT_ALLOCATOR_H
