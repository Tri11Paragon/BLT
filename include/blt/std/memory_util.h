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

#include <type_traits>
#include <array>
#include <cstring>
#include <algorithm>

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

namespace blt::mem
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
    
    inline static size_t next_byte_allocation(size_t prev_size, size_t default_allocation_block = 8192)
    {
        if (prev_size < default_allocation_block)
            return prev_size * 2;
        return prev_size + default_allocation_block;
    }
}

#endif //BLT_MEMORY_UTIL_H
