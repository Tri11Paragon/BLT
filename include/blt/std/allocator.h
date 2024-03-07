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
#include <cstdlib>
    
    #ifdef __unix__
        
        #include <sys/mman.h>
    
    #endif

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
                blt::size_t allocated_objects = 0;
                blt::u8* buffer = nullptr;
                blt::u8* offset = nullptr;
                
                explicit block(blt::u8* buffer): buffer(buffer), offset(buffer)
                {}
            };
            
            ALLOC<blt::u8> allocator;
            std::vector<block, ALLOC<block>> blocks;
            blt::size_t size_;
            blt::size_t allocations = 0;
            blt::size_t deallocations = 0;
            
            void expand()
            {
                auto ptr = static_cast<blt::u8*>(allocator.allocate(size_));
                blocks.push_back(block{ptr});
                allocations++;
            }
            
            template<typename T>
            T* allocate_back()
            {
                auto& back = blocks.back();
                size_t remaining_bytes = size_ - static_cast<size_t>(back.offset - back.buffer);
                auto pointer = static_cast<void*>(back.offset);
                const auto aligned_address = std::align(alignof(T), sizeof(T), pointer, remaining_bytes);
                if (aligned_address != nullptr)
                {
                    back.offset = static_cast<blt::u8*>(aligned_address) + sizeof(T);
                    back.allocated_objects++;
                }
                
                return static_cast<T*>(aligned_address);
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
                if (auto ptr = allocate_back<T>(); ptr == nullptr)
                    expand();
                else
                    return ptr;
                if (auto ptr = allocate_back<T>(); ptr == nullptr)
                    throw std::bad_alloc();
                else
                    return ptr;
            }
            
            template<typename T>
            void deallocate(T* p)
            {
                auto* ptr = reinterpret_cast<blt::u8*>(p);
                for (auto e : blt::enumerate(blocks))
                {
                    auto& block = e.second;
                    if (ptr >= block.buffer && ptr <= block.offset)
                    {
                        block.allocated_objects--;
                        if (block.allocated_objects == 0)
                        {
                            std::iter_swap(blocks.begin() + e.first, blocks.end() - 1);
                            allocator.deallocate(blocks.back().buffer, size_);
                            blocks.pop_back();
                            deallocations++;
                        }
                        return;
                    }
                }
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
                if (allocations != deallocations)
                    BLT_WARN("Allocator has blocks which have not been deallocated! Destructors might not have been called!");
                for (auto& v : blocks)
                    allocator.deallocate(v.buffer, size_);
            }
    };
    
    template<blt::size_t BLOCK_SIZE = 4096 * 512, bool USE_HUGE = true, blt::size_t HUGE_PAGE_SIZE = 4096 * 512, bool WARN_ON_FAIL = false>
    class bump_allocator2
    {
            // power of two
            static_assert(((BLOCK_SIZE & (BLOCK_SIZE - 1)) == 0) && "Must be a power of two!");
        private:
            template<typename LOG_FUNC>
            static void handle_mmap_error(LOG_FUNC func = BLT_ERROR_STREAM)
            {
#define BLT_WRITE(arg) func << arg << '\n';
                switch (errno)
                {
                    case EACCES:
                        BLT_WRITE("fd not set to open!");
                        break;
                    case EAGAIN:
                        BLT_WRITE("The file has been locked, or too much memory has been locked");
                        break;
                    case EBADF:
                        BLT_WRITE("fd is not a valid file descriptor");
                        break;
                    case EEXIST:
                        BLT_WRITE("MAP_FIXED_NOREPLACE was specified in flags, and the range covered "
                                  "by addr and length clashes with an existing mapping.");
                        break;
                    case EINVAL:
                        BLT_WRITE("We don't like addr, length, or offset (e.g., they are too large, or not aligned on a page boundary).");
                        BLT_WRITE("Or length was 0");
                        BLT_WRITE("Or flags contained none of MAP_PRIVATE, MAP_SHARED, or MAP_SHARED_VALIDATE.");
                        break;
                    case ENFILE:
                        BLT_WRITE("The system-wide limit on the total number of open files has been reached.");
                        break;
                    case ENODEV:
                        BLT_WRITE("The underlying filesystem of the specified file does not support memory mapping.");
                        break;
                    case ENOMEM:
                        BLT_WRITE("No memory is available.");
                        BLT_WRITE("Or The process's maximum number of mappings would have been exceeded.  "
                                  "This error can also occur for munmap(), when unmapping a region in the middle of an existing mapping, "
                                  "since this results in two smaller mappings on either side of the region being unmapped.");
                        BLT_WRITE("Or The process's RLIMIT_DATA limit, described in getrlimit(2), would have been exceeded.");
                        BLT_WRITE("Or We don't like addr, because it exceeds the virtual address space of the CPU.");
                        break;
                    case EOVERFLOW:
                        BLT_WRITE("On 32-bit architecture together with the large file extension (i.e., using 64-bit off_t): "
                                  "the number of pages used for length plus number of "
                                  "pages used for offset would overflow unsigned long (32 bits).");
                        break;
                    case EPERM:
                        BLT_WRITE("The prot argument asks for PROT_EXEC but the mapped area "
                                  "belongs to a file on a filesystem that was mounted no-exec.");
                        BLT_WRITE("Or The operation was prevented by a file seal");
                        BLT_WRITE("Or The MAP_HUGETLB flag was specified, but the caller "
                                  "was not privileged (did not have the CAP_IPC_LOCK capability) "
                                  "and is not a member of the sysctl_hugetlb_shm_group group; "
                                  "see the description of /proc/sys/vm/sysctl_hugetlb_shm_group");
                        break;
                    case ETXTBSY:
                        BLT_WRITE("MAP_DENYWRITE was set but the object specified by fd is open for writing.");
                        break;
                }
            }
            
            struct block
            {
                struct
                {
                    blt::size_t allocated_objects = 0;
                    block* next = nullptr;
                    block* prev = nullptr;
                    blt::u8* offset = nullptr;
                } metadata;
                blt::u8 buffer[BLOCK_SIZE - sizeof(metadata)]{};
                
                block()
                {
                    metadata.offset = buffer;
                }
            };
            
            block* base = nullptr;
            block* head = nullptr;
            
            block* allocate_block()
            {
                block* buffer;
#ifdef __unix__
                if constexpr (USE_HUGE)
                {
                    static_assert((BLOCK_SIZE & (HUGE_PAGE_SIZE - 1)) == 0 && "Must be multiple of the huge page size!");
                    buffer = static_cast<block*>(mmap(nullptr, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0));
                    // if we fail to allocate a huge page we can try to allocate normally
                    if (buffer == MAP_FAILED)
                    {
                        if constexpr (WARN_ON_FAIL)
                        {
                            BLT_WARN_STREAM << "We failed to allocate huge pages\n";
                            handle_mmap_error(BLT_WARN_STREAM);
                            BLT_WARN_STREAM << "\033[1;31mYou should attempt to enable "
                                               "huge pages as this will allocate normal pages and double the memory usage!\033[22m\n";
                        }
                        blt::size_t bytes = BLOCK_SIZE * 2;
                        buffer = static_cast<block*>(mmap(nullptr, bytes, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
                        if (buffer == MAP_FAILED)
                        {
                            BLT_ERROR_STREAM << "Failed to allocate normal pages\n";
                            handle_mmap_error(BLT_ERROR_STREAM);
                            throw std::bad_alloc();
                        }
                        if constexpr (WARN_ON_FAIL)
                        {
                            if (((size_t) buffer & (HUGE_PAGE_SIZE - 1)) != 0)
                                BLT_ERROR("Pointer is not aligned! %p", buffer);
                        }
                        auto* ptr = static_cast<void*>(buffer);
                        auto ptr_size = reinterpret_cast<blt::size_t>(ptr);
                        buffer = static_cast<block*>(std::align(BLOCK_SIZE, BLOCK_SIZE, ptr, bytes));
                        if constexpr (WARN_ON_FAIL)
                            BLT_ERROR("Offset by %ld pages, resulting: %p", (reinterpret_cast<blt::size_t>(buffer) - ptr_size) / 4096, buffer);
                    }
                } else
                    buffer = reinterpret_cast<block*>(std::aligned_alloc(BLOCK_SIZE, BLOCK_SIZE));
#else
                buffer = reinterpret_cast<block*>(std::aligned_alloc(BLOCK_SIZE, BLOCK_SIZE));
#endif
                construct(buffer);
                return buffer;
            }
            
            void allocate_forward()
            {
                auto* block = allocate_block();
                block->metadata.prev = head;
                head->metadata.next = block;
                head = block;
            }
            
            template<typename T>
            T* allocate_back()
            {
                blt::size_t remaining_bytes = BLOCK_SIZE - static_cast<blt::size_t>(head->metadata.offset - head->buffer);
                auto pointer = static_cast<void*>(head->metadata.offset);
                const auto aligned_address = std::align(alignof(T), sizeof(T), pointer, remaining_bytes);
                if (aligned_address != nullptr)
                {
                    head->metadata.allocated_objects++;
                    head->metadata.offset = static_cast<blt::u8*>(aligned_address) + sizeof(T);
                }
                return static_cast<T*>(aligned_address);
            }
            
            inline void del(block* p)
            {
                if constexpr (USE_HUGE)
                {
                    if (munmap(p, BLOCK_SIZE))
                    {
                        BLT_ERROR_STREAM << "FAILED TO DEALLOCATE BLOCK\n";
                        handle_mmap_error(BLT_ERROR_STREAM);
                    }
                } else
                    free(p);
            }
        
        public:
            bump_allocator2()
            {
                base = head = allocate_block();
            };
            
            explicit bump_allocator2(blt::size_t): bump_allocator2()
            {}
            
            template<typename T>
            [[nodiscard]] T* allocate()
            {
                if constexpr (sizeof(T) > BLOCK_SIZE)
                    throw std::bad_alloc();
                
                if (T* ptr = allocate_back<T>(); ptr == nullptr)
                    allocate_forward();
                else
                    return ptr;
                
                if (T* ptr = allocate_back<T>(); ptr == nullptr)
                    throw std::bad_alloc();
                else
                    return ptr;
            }
            
            template<typename T>
            void deallocate(T* p)
            {
                if (p == nullptr)
                    return;
                auto* blk = reinterpret_cast<block*>(reinterpret_cast<std::uintptr_t>(p) & static_cast<std::uintptr_t>(~(BLOCK_SIZE - 1)));
                if (--blk->metadata.allocated_objects == 0)
                {
                    if (blk == base)
                        base = allocate_block();
                    if (blk->metadata.prev != nullptr)
                        blk->metadata.prev->metadata.next = blk->metadata.next;
                    
                    del(blk);
                }
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
            
            ~bump_allocator2()
            {
                block* next = base;
                while (next != nullptr)
                {
                    auto* after = next->metadata.next;
                    del(next);
                    next = after;
                }
            }
    };
}

#define BLT_ALLOCATOR_H

#endif //BLT_ALLOCATOR_H
