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
    
    /**
     * fast inverse sqrt
     */
    static inline float fsqrt(float n){
        int i;
        float x, y;
        x = n * 0.5f;
        y = n;
        i = * (int*) &y;
        i = 0x5f3759df - (i >> 1);
        y = * (float*) &i;
        y = y * (1.5f - (x * y * y));
        y = y * (1.5f - (x * y * y));
        return y;
    }
    
//    inline std::ostream& operator<<(std::ostream& out, const mat4x4& v) {
//        return out << "\rMatrix4x4{" << v.m00() << ", " << v.m01() << ", " << v.m02() << ", " << v.m03() << "} \n"\
// << "         {" << v.m10() << ", " << v.m11() << ", " << v.m12() << ", " << v.m13() << "} \n"\
// << "         {" << v.m20() << ", " << v.m21() << ", " << v.m22() << ", " << v.m23() << "} \n"\
// << "         {" << v.m30() << ", " << v.m31() << ", " << v.m32() << ", " << v.m33() << "} \n";
//    }

}

#endif //BLT_MATH_H
