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
    
    static inline blt::u32 pcg_hash32(blt::u32 input)
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
    
    static inline double pcg_double32(blt::u32& seed)
    {
        seed = pcg_hash32(seed);
        return static_cast<double>(seed) / static_cast<double>(std::numeric_limits<blt::u32>::max());
    }
    
    static inline float pcg_float32(blt::u32& seed)
    {
        return static_cast<float>(pcg_double32(seed));
    }
    
    /**
     * @return random float without changing seed
     */
    static inline float pcg_float32c(blt::u32 seed)
    {
        return pcg_float32(seed);
    }
    
    static inline double pcg_double32c(blt::u32 seed)
    {
        return pcg_double32(seed);
    }
    
    /**
     * @param seed seed for random
     * @param min inclusive min
     * @param max exclusive max
     * @return random int between min (inclusive) and max (exclusive)
     */
    template<typename T = blt::i32>
    static inline T pcg_random32(blt::u32& seed, T min = 0, T max = 2)
    {
        return static_cast<T>((pcg_double32(seed) * static_cast<double>(max - min)) + static_cast<double>(min));
    }
    
    template<typename T = blt::i32>
    static inline T pcg_random32c(blt::u32 seed, T min = 0, T max = 2)
    {
        return pcg_int(seed, min, max);
    }
    
    
    static inline double murmur_double64(blt::u64& seed)
    {
        seed = murmur64(seed);
        return static_cast<double>(seed) / static_cast<double>(std::numeric_limits<blt::u64>::max());
    }
    
    static inline float murmur_float64(blt::u64& seed)
    {
        return static_cast<float>(murmur_double64(seed));
    }
    
    static inline float murmur_float64c(blt::u64 seed)
    {
        return murmur_float64(seed);
    }
    
    static inline double murmur_double64c(blt::u64 seed)
    {
        return murmur_double64(seed);
    }
    
    template<typename T = blt::i32>
    static inline T murmur_random64(blt::u64& seed, T min = 0, T max = 2)
    {
        return static_cast<T>((murmur_double64(seed) * static_cast<double>(max - min)) + static_cast<double>(min));
    }
    
    template<typename T = blt::i32>
    static inline T murmur_random64c(blt::u64 seed, T min = 0, T max = 2)
    {
        return murmur_integral_64(seed, min, max);
    }
    
}

#endif //BLT_RANDOM_H
