/*
 * Created by Brett on 04/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_RANDOM_H
#define BLT_RANDOM_H

#include <blt/std/types.h>
#include <random>

namespace blt::random
{
    // https://github.com/avaneev/komihash/tree/main
    
    static inline blt::u32 PCG_Hash(blt::u32 input)
    {
        blt::u32 state = input * 747796405u + 2891336453u;
        blt::u32 word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }
    
    //https://lemire.me/blog/2018/08/15/fast-strongly-universal-64-bit-hashing-everywhere/
    static inline blt::u64 murmur64(blt::u64 h)
    {
        h ^= h >> 33;
        h *= 0xff51afd7ed558ccdL;
        h ^= h >> 33;
        h *= 0xc4ceb9fe1a85ec53L;
        h ^= h >> 33;
        return h;
    }
    
    static inline double pcg_double(blt::u32& seed)
    {
        seed = PCG_Hash(seed);
        return static_cast<double>(seed) / static_cast<double>(std::numeric_limits<blt::u32>::max());
    }
    
    static inline float pcg_float(blt::u32& seed)
    {
        return static_cast<float>(pcg_double(seed));
    }
    
    /**
     * @return random float without changing seed
     */
    static inline float pcg_float_c(blt::u32 seed)
    {
        return pcg_float(seed);
    }
    
    static inline double pcg_double_c(blt::u32 seed)
    {
        return pcg_double(seed);
    }
    
    /**
     * @param seed seed for random
     * @param min inclusive min
     * @param max exclusive max
     * @return random int between min (inclusive) and max (exclusive)
     */
    static inline int pcg_int(blt::u32& seed, int min = 0, int max = 2)
    {
        return static_cast<int>((pcg_double(seed) * static_cast<double>(max - min)) + static_cast<double>(min));
    }
    
    static inline int pcg_int_c(blt::u32 seed, int min = 0, int max = 2)
    {
        return pcg_int(seed, min, max);
    }
    
    
    static inline double murmur_double(blt::u64& seed)
    {
        seed = murmur64(seed);
        return static_cast<double>(seed) / static_cast<double>(std::numeric_limits<blt::u64>::max());
    }
    
    static inline float murmur_float(blt::u64& seed)
    {
        return static_cast<float>(murmur_double(seed));
    }
    
    static inline float murmur_float_c(blt::u64 seed)
    {
        return murmur_float(seed);
    }
    
    static inline double murmur_double_c(blt::u64 seed)
    {
        return murmur_double(seed);
    }
    
    template<typename T>
    static inline T murmur_integral_64(blt::u64& seed, T min = 0, T max = 2)
    {
        return static_cast<T>((murmur_double(seed) * static_cast<double>(max - min)) + static_cast<double>(min));
    }
    
    template<typename T>
    static inline T murmur_integral_64c(blt::u64 seed, T min = 0, T max = 2)
    {
        return murmur_integral_64(seed, min, max);
    }
    
}

#endif //BLT_RANDOM_H
