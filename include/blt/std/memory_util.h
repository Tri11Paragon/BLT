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

#ifndef BLT_MEMORY_UTIL_H
#define BLT_MEMORY_UTIL_H

#include <blt/std/types.h>
#include <type_traits>
#include <array>
#include <cstring>
#include <algorithm>
#include <cstdint>

#if defined(__clang__) || defined(__llvm__) || defined(__GNUC__) || defined(__GNUG__)

#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(WIN32)

#include <byteswap.h>

#define SWAP16(val) bswap_16(val)
#define SWAP32(val) bswap_32(val)
#define SWAP64(val) bswap_64(val)
#else
        #define SWAP16(val) __builtin_bswap16(val)
        #define SWAP32(val) __builtin_bswap32(val)
        #define SWAP64(val) __builtin_bswap64(val)
#endif


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

namespace blt::mem
{
    template <typename R, typename T>
    inline static R type_cast(T type)
    {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable to be type casted!");
        static_assert(sizeof(T) == sizeof(R));
        R r;
        std::memcpy(&r, &type, sizeof(type));
        return r;
    }

    template <typename T>
    inline void reverse(T& out)
    {
        static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable to be reversible!");
        // if we need to swap find the best way to do so
        if constexpr (std::is_same_v<T, int16_t> || std::is_same_v<T, uint16_t>)
            out = type_cast<T>(SWAP16(type_cast<uint16_t>(out)));
        else if constexpr (std::is_same_v<T, int32_t> || std::is_same_v<T, uint32_t> || std::is_same_v<T, f32>)
            out = type_cast<T>(SWAP32(type_cast<uint32_t>(out)));
        else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, uint64_t> || std::is_same_v<T, f64>)
            out = type_cast<T>(SWAP64(type_cast<uint64_t>(out)));
        else
        {
            std::array<std::byte, sizeof(T)> data;
            std::memcpy(data.data(), &out, sizeof(T));
            std::reverse(data.begin(), data.end());
            std::memcpy(&out, data.data(), sizeof(T));
        }
    }

    // Used to grab the byte-data of any T element. Defaults to Big Endian, however can be configured to use little endian
    template <bool little_endian = false, typename BYTE_TYPE, typename T>
    inline int toBytes(const T& in, BYTE_TYPE* out)
    {
        if constexpr (!(std::is_same_v<BYTE_TYPE, std::int8_t> || std::is_same_v<BYTE_TYPE, std::uint8_t>))
            static_assert("Must provide a signed/unsigned int8 type");
        std::memcpy(out, &in, sizeof(T));

        if constexpr (ENDIAN_LOOKUP(little_endian))
        {
            // TODO: this but better.
            for (size_t i = 0; i < sizeof(T) / 2; i++)
                std::swap(out[i], out[sizeof(T) - 1 - i]);
        }

        return 0;
    }

    // Used to cast the binary data of any T object, into a T object. Assumes data is in big ending (configurable)
    template <bool little_endian = false, typename BYTE_TYPE, typename T>
    inline int fromBytes(const BYTE_TYPE* in, T& out)
    {
        if constexpr (!(std::is_same_v<BYTE_TYPE, std::int8_t> || std::is_same_v<BYTE_TYPE, std::uint8_t>))
            static_assert("Must provide a signed/unsigned int8 type");

        std::memcpy(&out, in, sizeof(T));
        if constexpr (ENDIAN_LOOKUP(little_endian))
        {
            reverse(out);
        }

        return 0;
    }

    template <bool little_endian = false, typename BYTE_TYPE, typename T>
    inline static int fromBytes(const BYTE_TYPE* in, T* out)
    {
        return fromBytes(in, *out);
    }

    inline static size_t next_byte_allocation(size_t prev_size, size_t default_allocation_block = 8192, size_t default_size = 16)
    {
        if (prev_size < default_size)
            return default_size;
        if (prev_size < default_allocation_block)
            return prev_size * 2;
        return prev_size + default_allocation_block;
    }
}

namespace blt
{
    template <typename V>
    struct ptr_iterator
    {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = blt::ptrdiff_t;
        using value_type = V;
        using pointer = value_type*;
        using reference = value_type&;
        using iter_reference = ptr_iterator&;

        explicit ptr_iterator(V* v): _v(v)
        {
        }

        reference operator*() const
        {
            return *_v;
        }

        pointer operator->()
        {
            return _v;
        }

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

        iter_reference operator+=(difference_type amount)
        {
            _v += amount;
            return *this;
        }

        iter_reference operator-=(difference_type amount)
        {
            _v -= amount;
            return *this;
        }

        reference operator[](difference_type index)
        {
            return *(_v + index);
        }

        reference operator[](blt::size_t index)
        {
            return *(_v + index);
        }

        friend bool operator<(const ptr_iterator& a, const ptr_iterator& b)
        {
            return b._v - a._v > 0;
        }

        friend bool operator>(const ptr_iterator& a, const ptr_iterator& b)
        {
            return a._v - b._v > 0;
        }

        friend bool operator<=(const ptr_iterator& a, const ptr_iterator& b)
        {
            return b._v - a._v >= 0;
        }

        friend bool operator>=(const ptr_iterator& a, const ptr_iterator& b)
        {
            return a._v - b._v >= 0;
        }

        friend difference_type operator-(const ptr_iterator& a, const ptr_iterator& b)
        {
            return a._v - b._v;
        }

        friend ptr_iterator operator+(const ptr_iterator& a, difference_type n)
        {
            return ptr_iterator(a._v + n);
        }

        friend ptr_iterator operator+(difference_type n, const ptr_iterator& a)
        {
            return ptr_iterator(a._v + n);
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
}

#endif //BLT_MEMORY_UTIL_H
