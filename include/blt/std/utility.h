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

#ifndef BLT_UTILITY_H
#define BLT_UTILITY_H

#include <optional>

namespace blt
{
    
    template<typename BEGIN, typename END>
    class enumerate
    {
        private:
        
    };

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
    void BLT_ATTRIB_NO_INLINE black_box_ref(const T& val)
    {
        volatile void* hell;
        hell = (void*) &val;
    }
    
    template<typename T>
    void BLT_ATTRIB_NO_INLINE black_box(T val)
    {
        volatile void* hell2;
        hell2 = (void*) &val;
    }
    
    template<typename T>
    const T& BLT_ATTRIB_NO_INLINE black_box_ref_ret(const T& val)
    {
        volatile void* hell;
        hell = (void*) &val;
        return val;
    }
    
    template<typename T>
    T BLT_ATTRIB_NO_INLINE black_box_ret(T val)
    {
        volatile void* hell2;
        hell2 = (void*) &val;
        return val;
    }
    
}

#endif //BLT_UTILITY_H
