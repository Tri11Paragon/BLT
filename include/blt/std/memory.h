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
     * This is a simple buffer meant to be used only inside of a function and not moved around, with a few minor exceptions.
     * The internal buffer is allocated on the heap.
     * The operator * has been overloaded to return the internal buffer.
     * @tparam T type that is stored in buffer eg char
     */
    template<typename T>
    struct scoped_buffer
    {
        private:
            T* _buffer;
            size_t _size;
        public:
            explicit scoped_buffer(size_t size): _size(size)
            {
                _buffer = new T[size];
            }
            
            scoped_buffer(const scoped_buffer& copy) = delete;
            
            scoped_buffer(scoped_buffer&& move) noexcept
            {
                _buffer = move._buffer;
                _size = move.size();
                move._buffer = nullptr;
            }
            
            scoped_buffer operator=(scoped_buffer& copyAssignment) = delete;
            
            scoped_buffer& operator=(scoped_buffer&& moveAssignment) noexcept
            {
                _buffer = moveAssignment._buffer;
                _size = moveAssignment.size();
                moveAssignment._buffer = nullptr;
                
                return *this;
            }
            
            inline T& operator[](unsigned long index)
            {
                return _buffer[index];
            }
            
            inline const T& operator[](unsigned long index) const
            {
                return _buffer[index];
            }
            
            inline T* operator*()
            {
                return _buffer;
            }
            
            [[nodiscard]] inline size_t size() const
            {
                return _size;
            }
            
            inline T* ptr()
            {
                return _buffer;
            }
            
            inline T* ptr() const
            {
                return _buffer;
            }
            
            inline T* data() const
            {
                return _buffer;
            }
            
            inline T* data()
            {
                return _buffer;
            }
            
            ptr_iterator<T> begin()
            {
                return ptr_iterator{_buffer};
            }
            
            ptr_iterator<T> end()
            {
                return ptr_iterator{&_buffer[_size]};
            }
            
            ~scoped_buffer()
            {
                delete[] _buffer;
            }
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
    
    template<typename T>
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
                const_pointer p;
                size_t n;
            };
            
            void expand()
            {
                size_t new_size = m_size * 2;
                T* data = new T[new_size];
                if constexpr (std::is_trivially_copyable_v<T>)
                    std::memcpy(data, m_data, m_size);
                else if constexpr (std::is_move_assignable_v<T>)
                {
                    for (size_t i = 0; i < m_size; i++)
                        data[i] = std::move(m_data[i]);
                } else if constexpr (std::is_move_constructible_v<T>)
                {
                    // is this bad? probably
                    for (size_t i = 0; i < m_size; i++)
                        data[i] = T(std::move(m_data));
                } else if constexpr (std::is_copy_assignable_v<T>)
                {
                    for (size_t i = 0; i < m_size; i++)
                        data[i] = m_data[i];
                } else
                {
                    static_assert("Unable to use this type with this allocator!");
                }
                delete[] m_data;
                m_data = data;
                m_size = new_size;
            }
            
            void realign()
            {
            
            }
        
        public:
            area_allocator()
            {
                m_data = new T[m_size];
            }
            
            [[nodiscard]] pointer* allocate(size_t n)
            {
                if (m_last + n > m_size)
                    expand();
                pointer loc = &m_data[m_last];
                m_last += n;
                return loc;
            }
            
            void deallocate(pointer* p, size_t n) noexcept
            {
                deallocated_blocks.push({p, n});
                m_deallocated += n;
                // TODO: magic number
                if (static_cast<double>(m_deallocated) / static_cast<double>(m_last) > 0.25)
                    realign();
            }
            
            ~area_allocator()
            {
                delete[] m_data;
            }
        
        private:
            // current size of the data
            size_t m_size = 1;
            // last allocated location
            size_t m_last = 0;
            // how many values have been deallocated
            size_t m_deallocated = 0;
            T* m_data = nullptr;
            blt::flat_queue<pointer_view> deallocated_blocks;
    };
    
}

#endif //BLT_TESTS_MEMORY_H
