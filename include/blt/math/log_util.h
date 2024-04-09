/*
 * Created by Brett on 14/03/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_LOG_UTIL_H
#define BLT_TESTS_LOG_UTIL_H

#include <blt/math/vectors.h>
#include <blt/math/matrix.h>
#include <blt/std/logging.h>
#include <blt/std/utility.h>
#include "blt/std/string.h"

namespace blt
{
    
    template<typename Writer = blt::logging::logger, typename T, blt::u32 size>
    static inline Writer& operator<<(Writer& log, const blt::vec<T, size>& vec)
    {
        std::string type_string;
        const auto tstr = blt::type_string<T>();
        
        if constexpr (std::is_arithmetic_v<T>)
        {
            type_string += tstr[0];
            // for unsigned long / unsigned int
            auto split = blt::string::split_sv(tstr, ' ');
            if (tstr[0] == 'u'){
                if (split.size() > 1){
                    type_string += split[1][0];
                } else
                    type_string += tstr[1];
            }
            
        } else
            type_string = tstr;
        
        log << "Vec" << size << type_string << "(";
        for (blt::u32 i = 0; i < size; i++)
            log << vec[i] << ((i == size - 1) ? ")" : ", ");
        return log;
    }
    
    template<typename Writer = std::ostream>
    inline Writer& operator<<(Writer& out, const mat4x4& v)
    {
        out << "Mat4x4(\n";
        out << "\t{" << v.m00() << ", " << v.m01() << ", " << v.m02() << ", " << v.m03() << "},\n";
        out << "\t{" << v.m10() << ", " << v.m11() << ", " << v.m12() << ", " << v.m13() << "},\n";
        out << "\t{" << v.m20() << ", " << v.m21() << ", " << v.m22() << ", " << v.m23() << "},\n";
        out << "\t{" << v.m30() << ", " << v.m31() << ", " << v.m32() << ", " << v.m33() << "})";
        return out;
    }
    
}

#endif //BLT_TESTS_LOG_UTIL_H
