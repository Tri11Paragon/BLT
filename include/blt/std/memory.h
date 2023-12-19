/*
 * Created by Brett on 08/02/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_MEMORY_H
#define BLT_TESTS_MEMORY_H

#include <initializer_list>
#include <iterator>
#include <cstring>
#include "queue.h"
#include "utility.h"
#include <blt/std/assert.h>
#include <blt/std/logging.h>
#include <cstdint>
#include <type_traits>
#include <algorithm>
#include <utility>
#include <cstring>
#include <array>

#if defined(__clang__) || defined(__llvm__) || defined(__GNUC__) || defined(__GNUG__)
    
    #include <byteswap.h>
    
    #define SWAP16(val) bswap_16(val)
    #define SWAP32(val) bswap_32(val)
    #define SWAP64(val) bswap_64(val)
#if __cplusplus >= 202002L
    
    #include <bit>

    #define ENDIAN_LOOKUP(little_endian) (std::endian::native == std::endian::little && !little_endian) || \
                                         (std::endian::native == std::endian::big && little_endian)
#else
    #define ENDIAN_LOOKUP(little_endian) !little_endian
#endif
#elif defined(_MSC_VER)
    #include <intrin.h>
    #define SWAP16(val) _byteswap_ushort(val)
    #define SWAP32(val) _byteswap_ulong(val)
    #define SWAP64(val) _byteswap_uint64(val)
    #define ENDIAN_LOOKUP(little_endian) !little_endian
#endif

namespace blt
{
    
    namespace mem
    {
        // Used to grab the byte-data of any T element. Defaults to Big Endian, however can be configured to use little endian
        template<bool little_endian = false, typename BYTE_TYPE, typename T>
        inline static int toBytes(const T& in, BYTE_TYPE* out)
        {
            if constexpr (!(std::is_same_v<BYTE_TYPE, std::int8_t> || std::is_same_v<BYTE_TYPE, std::uint8_t>))
                static_assert("Must provide a signed/unsigned int8 type");
            std::memcpy(out, (void*) &in, sizeof(T));
            
            if constexpr (ENDIAN_LOOKUP(little_endian))
            {
                // TODO: this but better.
                for (size_t i = 0; i < sizeof(T) / 2; i++)
                    std::swap(out[i], out[sizeof(T) - 1 - i]);
            }
            
            return 0;
        }
        
        // Used to cast the binary data of any T object, into a T object. Assumes data is in big ending (configurable)
        template<bool little_endian = false, typename BYTE_TYPE, typename T>
        inline static int fromBytes(const BYTE_TYPE* in, T& out)
        {
            if constexpr (!(std::is_same_v<BYTE_TYPE, std::int8_t> || std::is_same_v<BYTE_TYPE, std::uint8_t>))
                static_assert("Must provide a signed/unsigned int8 type");
            
            std::array<BYTE_TYPE, sizeof(T)> data;
            std::memcpy(data.data(), in, sizeof(T));
            
            if constexpr (ENDIAN_LOOKUP(little_endian))
            {
                // if we need to swap find the best way to do so
                if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>)
                    out = SWAP16(*reinterpret_cast<T*>(data.data()));
                else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t>)
                    out = SWAP32(*reinterpret_cast<T*>(data.data()));
                else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t>)
                    out = SWAP64(*reinterpret_cast<T*>(data.data()));
                else
                {
                    std::reverse(data.begin(), data.end());
                    out = *reinterpret_cast<T*>(data.data());
                }
            }
            
            return 0;
        }
        
        template<bool little_endian = false, typename BYTE_TYPE, typename T>
        inline static int fromBytes(const BYTE_TYPE* in, T* out)
        {
            return fromBytes(in, *out);
        }
    }
    
    template<typename V>
    struct ptr_iterator
    {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = V;
            using pointer = value_type*;
            using reference = value_type&;
            
            explicit ptr_iterator(V* v): _v(v)
            {}
            
            reference operator*() const
            { return *_v; }
            
            pointer operator->()
            { return _v; }
            
            ptr_iterator& operator++()
            {
                _v++;
                return *this;
            }
            
            ptr_iterator& operator--()
            {
                _v--;
                return *this;
            }
            
            ptr_iterator operator++(int)
            {
                auto tmp = *this;
                ++(*this);
                return tmp;
            }
            
            ptr_iterator operator--(int)
            {
                auto tmp = *this;
                --(*this);
                return tmp;
            }
            
            friend bool operator==(const ptr_iterator& a, const ptr_iterator& b)
            {
                return a._v == b._v;
            }
            
            friend bool operator!=(const ptr_iterator& a, const ptr_iterator& b)
            {
                return a._v != b._v;
            }
        
        private:
            V* _v;
    };

/**
     * Creates an encapsulation of a T array which will be automatically deleted when this object goes out of scope.
     * This is a simple buffer meant to be used only inside of a function and not copied around.
     * The internal buffer is allocated on the heap.
     * The operator * has been overloaded to return the internal buffer.
     * @tparam T type that is stored in buffer eg char
     */
    template<typename T, bool = std::is_copy_constructible_v<T> || std::is_copy_assignable_v<T>>
    class scoped_buffer
    {
        private:
            T* buffer_ = nullptr;
            size_t size_;
        public:
            scoped_buffer(): buffer_(nullptr), size_(0)
            {}
            
            explicit scoped_buffer(size_t size): size_(size)
            {
                if (size > 0)
                    buffer_ = new T[size];
                else
                    buffer_ = nullptr;
            }
            
            scoped_buffer(const scoped_buffer& copy)
            {
                if (copy.size() == 0)
                {
                    buffer_ = nullptr;
                    size_ = 0;
                    return;
                }
                buffer_ = new T[copy.size()];
                size_ = copy.size_;
                
                if constexpr (std::is_trivially_copyable_v<T>)
                {
                    std::memcpy(buffer_, copy.buffer_, copy.size() * sizeof(T));
                } else
                {
                    if constexpr (std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>)
                    {
                        for (size_t i = 0; i < this->size_; i++)
                            buffer_[i] = T(copy[i]);
                    } else
                        for (size_t i = 0; i < this->size_; i++)
                            buffer_[i] = copy[i];
                }
            }
            
            scoped_buffer& operator=(const scoped_buffer& copy)
            {
                if (&copy == this)
                    return *this;
                
                if (copy.size() == 0)
                {
                    buffer_ = nullptr;
                    size_ = 0;
                    return *this;
                }
                
                delete[] this->buffer_;
                buffer_ = new T[copy.size()];
                size_ = copy.size_;
                
                if constexpr (std::is_trivially_copyable_v<T>)
                {
                    std::memcpy(buffer_, copy.buffer_, copy.size() * sizeof(T));
                } else
                {
                    if constexpr (std::is_copy_constructible_v<T> && !std::is_copy_assignable_v<T>)
                    {
                        for (size_t i = 0; i < this->size_; i++)
                            buffer_[i] = T(copy[i]);
                    } else
                        for (size_t i = 0; i < this->size_; i++)
                            buffer_[i] = copy[i];
                }
                return *this;
            }
            
            scoped_buffer(scoped_buffer&& move) noexcept
            {
                delete[] buffer_;
                buffer_ = move.buffer_;
                size_ = move.size();
                move.buffer_ = nullptr;
            }
            
            scoped_buffer& operator=(scoped_buffer&& moveAssignment) noexcept
            {
                delete[] buffer_;
                buffer_ = moveAssignment.buffer_;
                size_ = moveAssignment.size();
                moveAssignment.buffer_ = nullptr;
                
                return *this;
            }
            
            inline T& operator[](size_t index)
            {
                return buffer_[index];
            }
            
            inline const T& operator[](size_t index) const
            {
                return buffer_[index];
            }
            
            inline T* operator*()
            {
                return buffer_;
            }
            
            [[nodiscard]] inline size_t size() const
            {
                return size_;
            }
            
            inline T*& ptr()
            {
                return buffer_;
            }
            
            inline const T* const& ptr() const
            {
                return buffer_;
            }
            
            inline const T* const& data() const
            {
                return buffer_;
            }
            
            inline T*& data()
            {
                return buffer_;
            }
            
            inline ptr_iterator<T> begin()
            {
                return ptr_iterator{buffer_};
            }
            
            inline ptr_iterator<T> end()
            {
                return ptr_iterator{&buffer_[size_]};
            }
            
            ~scoped_buffer()
            {
                delete[] buffer_;
            }
    };
    
    template<typename T, size_t MAX_SIZE>
    class static_vector
    {
        private:
            T buffer_[MAX_SIZE];
            size_t size_ = 0;
        public:
            static_vector() = default;
            
            inline bool push_back(const T& copy)
            {
                if (size_ >= MAX_SIZE)
                    return false;
                buffer_[size_++] = copy;
                return true;
            }
            
            inline bool push_back(T&& move)
            {
                if (size_ >= MAX_SIZE)
                    return false;
                buffer_[size_++] = std::move(move);
                return true;
            }
            
            inline T& at(size_t index)
            {
                if (index >= MAX_SIZE)
                    throw std::runtime_error("Array index " + std::to_string(index) + " out of bounds! (Max size: " + std::to_string(MAX_SIZE) + ')');
            }
            
            inline T& operator[](size_t index)
            {
                return buffer_[index];
            }
            
            inline const T& operator[](size_t index) const
            {
                return buffer_[index];
            }
            
            inline void reserve(size_t size)
            {
                if (size > MAX_SIZE)
                    size = MAX_SIZE;
                size_ = size;
            }
            
            [[nodiscard]] inline size_t size() const
            {
                return size_;
            }
            
            [[nodiscard]] inline size_t capacity() const
            {
                return MAX_SIZE;
            }
            
            inline T* data()
            {
                return buffer_;
            }
            
            inline T* operator*()
            {
                return buffer_;
            }
            
            inline T* data() const
            {
                return buffer_;
            }
            
            inline T* begin()
            {
                return buffer_;
            }
            
            inline T* end()
            {
                return &buffer_[size_];
            }
    };
    
    template<typename T>
    class scoped_buffer<T, false> : scoped_buffer<T, true>
    {
            using scoped_buffer<T, true>::scoped_buffer;
        public:
            scoped_buffer(const scoped_buffer& copy) = delete;
            
            scoped_buffer operator=(scoped_buffer& copyAssignment) = delete;
    };
    
    template<typename T>
    struct nullptr_initializer
    {
        private:
            T* m_ptr = nullptr;
        public:
            nullptr_initializer() = default;
            
            explicit nullptr_initializer(T* ptr): m_ptr(ptr)
            {}
            
            nullptr_initializer(const nullptr_initializer<T>& ptr): m_ptr(ptr.m_ptr)
            {}
            
            nullptr_initializer(nullptr_initializer<T>&& ptr) noexcept: m_ptr(ptr.m_ptr)
            {}
            
            nullptr_initializer<T>& operator=(const nullptr_initializer<T>& ptr)
            {
                if (&ptr == this)
                    return *this;
                this->m_ptr = ptr.m_ptr;
                return *this;
            }
            
            nullptr_initializer<T>& operator=(nullptr_initializer<T>&& ptr) noexcept
            {
                if (&ptr == this)
                    return *this;
                this->m_ptr = ptr.m_ptr;
                return *this;
            }
            
            inline T* operator->()
            {
                return m_ptr;
            }
            
            ~nullptr_initializer() = default;
    };
    
    /**
     * Creates a hash-map like association between an enum key and any arbitrary value.
     * The storage is backed by a contiguous array for faster access.
     * @tparam K enum value
     * @tparam V associated value
     */
    template<typename K, typename V>
    class enum_storage
    {
        private:
            V* m_values;
            size_t m_size = 0;
        public:
            enum_storage(std::initializer_list<std::pair<K, V>> init)
            {
                for (auto& i : init)
                    m_size = std::max((size_t) i.first, m_size);
                m_values = new V[m_size];
                for (auto& v : init)
                    m_values[(size_t) v.first] = v.second;
            }
            
            inline V& operator[](size_t index)
            {
                return m_values[index];
            }
            
            inline const V& operator[](size_t index) const
            {
                return m_values[index];
            }
            
            [[nodiscard]] inline size_t size() const
            {
                return m_size;
            }
            
            ptr_iterator<V> begin()
            {
                return ptr_iterator{m_values};
            }
            
            ptr_iterator<V> end()
            {
                return ptr_iterator{&m_values[m_size]};
            }
            
            ~enum_storage()
            {
                delete[] m_values;
            }
    };
    
    template<typename T, size_t BLOCK_SIZE = 8192>
    class area_allocator
    {
        public:
            typedef T value_type;
            typedef T* pointer;
            typedef const T* const_pointer;
            typedef void* void_pointer;
            typedef const void* const_void_pointer;
        private:
            struct pointer_view
            {
                pointer p;
                size_t n;
            };
            
            struct block_storage
            {
                pointer data;
                size_t used = 0;
                // TODO: b-tree?
                std::vector<pointer_view> unallocated_blocks;
            };
            
            inline void allocate_block()
            {
                BLT_INFO("Allocating a new block of size %d", BLOCK_SIZE);
                auto* blk = new block_storage();
                blk->data = static_cast<pointer>(malloc(sizeof(T) * BLOCK_SIZE));
                blocks.push_back(blk);
            }
            
            inline pointer find_available_block(size_t n)
            {
                for (auto* blk : blocks)
                {
                    size_t index = -1ull;
                    size_t leftover = 0;
                    for (auto kv : blt::enumerate(blk->unallocated_blocks))
                    {
                        if (kv.second.n >= n)
                        {
                            index = kv.first;
                            leftover = kv.second.n - n;
                            break;
                        }
                    }
                    if (index != -1ull)
                    {
                        pointer_view ptr = blk->unallocated_blocks[index];
                        std::iter_swap(blk->unallocated_blocks.begin() + index, blk->unallocated_blocks.end() - 1);
                        blk->unallocated_blocks.pop_back();
//                        BLT_INFO("Found block! %d, Unallocated leftover %d", index, leftover);
                        if (leftover > 0)
                            blk->unallocated_blocks.push_back({ptr.p + n, leftover});
                        return ptr.p;
                    }
                }
                return nullptr;
            }
            
            inline std::pair<pointer, size_t> getBlock(size_t n)
            {
                auto* blk = find_available_block(n);
                if (blk != nullptr)
                    return {blk, 0};
                
                if (blocks.back()->used + n > BLOCK_SIZE)
                    allocate_block();
                
                auto ptr = std::pair<pointer, size_t>{blocks.back()->data, blocks.back()->used};
                blocks.back()->used += n;
                return ptr;
            }
            
            inline void allocate_in_block(pointer begin, size_t n)
            {
                if constexpr (std::is_default_constructible_v<T> && !std::is_trivially_default_constructible_v<T>)
                {
                    for (size_t i = 0; i < n; i++)
                        new(&begin[i]) T();
                }
            }
        
        public:
            area_allocator()
            {
                allocate_block();
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
                for (size_t i = 0; i < n; i++)
                    p[i].~T();
                for (auto*& blk : blocks)
                {
                    if (p >= blk->data && p <= (blk->data + BLOCK_SIZE))
                    {
                        blk->unallocated_blocks.push_back({p, n});
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
    
}

#endif //BLT_TESTS_MEMORY_H
