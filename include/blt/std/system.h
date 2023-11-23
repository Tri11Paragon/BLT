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

#if defined(__GNUC__) || defined(__llvm__)
    #define BLT_ATTRIB_NO_INLINE __attribute__ ((noinline))
#else
    #if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
        #define BLT_ATTRIB_NO_INLINE __declspec(noinline)
    #else
        #define BLT_ATTRIB_NO_INLINE
    #endif
#endif
    
    template<typename T>
    void BLT_ATTRIB_NO_INLINE black_box_ref(const T& val){
        volatile void* hell;
        hell = (void*)&val;
    }
    
    template<typename T>
    void BLT_ATTRIB_NO_INLINE black_box(T val){
        volatile void* hell2;
        hell2 = (void*)&val;
    }
    
    template<typename T>
    const T& BLT_ATTRIB_NO_INLINE black_box_ref_ret(const T& val){
        volatile void* hell;
        hell = (void*)&val;
        return val;
    }
    
    template<typename T>
    T BLT_ATTRIB_NO_INLINE black_box_ret(T val){
        volatile void* hell2;
        hell2 = (void*)&val;
        return val;
    }
}

#endif //BLT_SYSTEM_H
