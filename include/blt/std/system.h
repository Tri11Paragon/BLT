/*
 * Created by Brett on 04/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_SYSTEM_H
#define BLT_SYSTEM_H

#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#include <cstdint>

namespace blt::system {
//#ifdef __GNUC__
//    #define GNU_INLINE __attribute__((__gnu_inline__, __always_inline__))
//#else
//    #define GNU_INLINE
//#endif
    inline std::uint64_t rdtsc(){
        return __rdtsc();
    }
    // TODO: system memory and current CPU usage. (Linux Only currently)
}

#endif //BLT_SYSTEM_H
