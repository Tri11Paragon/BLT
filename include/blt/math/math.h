/*
 * Created by Brett on 09/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_MATH_H
#define BLT_MATH_H

#include <blt/math/vectors.h>
#include <blt/math/matrix.h>

namespace blt {
    
    /**
     * fast number integer
     */
    static inline unsigned int f_randi(unsigned int seed) {
        seed = (seed << 13) ^ seed;
        return ((seed * (seed * seed * 15731 + 789221) + 1376312589) & 0x7fffffff);
    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
    /**
     * fast inverse sqrt
     */
    static inline float fsqrt(float n){
        int i;
        float x, y;
        x = n * 0.5f;
        y = n;
        i = *reinterpret_cast<int*>(&y);
        i = 0x5f3759df - (i >> 1);
        y = *reinterpret_cast<float*>(&i);
        y = y * (1.5f - (x * y * y));
        y = y * (1.5f - (x * y * y));
        return y;
    }

#pragma GCC diagnostic pop
    
    
    static inline constexpr double pow(int b, int p) {
        int collection = 1;
        for (int i = 0; i < p; i++)
            collection *= b;
        return collection;
    }
    
    /**
     * This is a fast rounding function and is not guaranteed to be 100% correct
     * @tparam decimal_places
     * @param value
     * @return
     */
    template<int decimal_places>
    static inline double round_up(double value) {
        constexpr double multiplier = pow(10, decimal_places);
        return ((int)(value * multiplier) + 1) / multiplier;
    }
    
    /*inline std::ostream& operator<<(std::ostream& out, const mat4x4& v) {
        return out << "\rMatrix4x4{" << v.m00() << ", " << v.m01() << ", " << v.m02() << ", " << v.m03() << "} \n"\
 << "         {" << v.m10() << ", " << v.m11() << ", " << v.m12() << ", " << v.m13() << "} \n"\
 << "         {" << v.m20() << ", " << v.m21() << ", " << v.m22() << ", " << v.m23() << "} \n"\
 << "         {" << v.m30() << ", " << v.m31() << ", " << v.m32() << ", " << v.m33() << "} \n";
    }*/

}

#endif //BLT_MATH_H
