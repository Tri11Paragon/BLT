/*
 * Created by Brett on 09/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_MATH_H
#define BLT_MATH_H

#include <initializer_list>
#include <cmath>

namespace blt {
    
    template<unsigned long size>
    struct vec {
        private:
            float elements[size]{};
        public:
            
            vec() {
                for (int i = 0; i < size; i++)
                    elements[i] = 0;
            }
            
            vec(std::initializer_list<float> args): vec() {
                for (int i = 0; i < args.size(); i++) {
                    elements[i] = *(args.begin() + i);
                }
            }
            
            explicit vec(const float elem[size]) {
                for (int i = 0; i < size; i++) {
                    elements[i] = elem[i];
                }
            }
            
            vec(const vec<size>& copy): vec(copy.elements) {}
            
            [[nodiscard]] inline float x() const {return elements[0];}
            [[nodiscard]] inline float y() const {return elements[1];}
            [[nodiscard]] inline float z() const {return elements[2];}
            
            inline float& operator[](int index) {
                return elements[index];
            }
            
            inline float operator[](int index) const {
                return elements[index];
            }
            
            inline vec<size>& operator=(float f) {
                for (int i = 0; i < size; i++)
                    elements[i] = f;
                return *this;
            }
            
            inline vec<size>& operator=(int i) {
                for (int _ = 0; _ < size; _++)
                    elements[_] = i;
                return *this;
            }
            
            inline vec<size> operator-() {
                float negativeCopy[size];
                for (int i = 0; i < size; i++)
                    negativeCopy[i] = -elements[i];
                return vec<size>{negativeCopy};
            }
            
            inline vec<size>& operator+=(const vec<size>& other) {
                for (int i = 0; i < size; i++)
                    elements[i] += other[i];
                return *this;
            }
            
            inline vec<size>& operator*=(const vec<size>& other) {
                for (int i = 0; i < size; i++)
                    elements[i] *= other[i];
                return *this;
            }
            
            inline vec<size>& operator+=(float f) {
                for (int i = 0; i < size; i++)
                    elements[i] += f;
                return *this;
            }
            
            inline vec<size>& operator*=(float f) {
                for (int i = 0; i < size; i++)
                    elements[i] *= f;
                return *this;
            }
            
            inline vec<size>& operator-=(const vec<size>& other) {
                for (int i = 0; i < size; i++)
                    elements[i] -= other[i];
                return *this;
            }
            
            inline vec<size>& operator-=(float f) {
                for (int i = 0; i < size; i++)
                    elements[i] -= f;
                return *this;
            }
    };
    
    template<unsigned long size>
    inline vec<size> operator+(const vec<size>& left, const vec<size>& right) {
        float initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] + right[i];
        return vec<size>{initializer};
    }
    
    template<unsigned long size>
    inline vec<size> operator-(const vec<size>& left, const vec<size>& right) {
        float initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] - right[i];
        return vec<size>{initializer};
    }
    
    template<unsigned long size>
    inline vec<size> operator+(const vec<size>& left, float f) {
        float initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] + f;
        return vec<size>{initializer};
    }
    
    template<unsigned long size>
    inline vec<size> operator-(const vec<size>& left, float f) {
        float initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] + f;
        return vec<size>{initializer};
    }
    
    template<unsigned long size>
    inline vec<size> operator+(float f, const vec<size>& right) {
        float initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = f + right[i];
        return vec<size>{initializer};
    }
    
    template<unsigned long size>
    inline vec<size> operator-(float f, const vec<size>& right) {
        float initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = f - right[i];
        return vec<size>{initializer};
    }
    
    template<unsigned long size>
    inline vec<size> operator*(const vec<size>& left, const vec<size>& right) {
        float initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] * right[i];
        return vec<size>{initializer};
    }
    
    template<unsigned long size>
    inline vec<size> operator*(const vec<size>& left, float f) {
        float initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] * f;
        return vec<size>{initializer};
    }
    
    template<unsigned long size>
    inline vec<size> operator*(float f, const vec<size>& right) {
        float initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = f * right[i];
        return vec<size>{initializer};
    }
    
    template<unsigned long size>
    inline vec<size> operator/(const vec<size>& left, float f) {
        float initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] / f;
        return vec<size>{initializer};
    }
    
    typedef vec<2> vec2;
    typedef vec<3> vec3;
    typedef vec<4> vec4;
    
    class mat4x4 {
        protected:
            // 4x4 = 16
            union dataType {
                float single[16];
                float dim[4][4];
            };
            dataType data{};
            
            friend mat4x4 operator+(const mat4x4& left, const mat4x4& right);
            
            friend mat4x4 operator-(const mat4x4& left, const mat4x4& right);
            
            friend mat4x4 operator*(const mat4x4& left, const mat4x4& right);
            
            friend mat4x4 operator*(float c, const mat4x4& v);
            
            friend mat4x4 operator*(const mat4x4& v, float c);
            
            friend mat4x4 operator/(const mat4x4& v, float c);
            
            friend mat4x4 operator/(float c, const mat4x4& v);
        
        public:
            mat4x4() {
                for (float& i : data.single) {
                    i = 0;
                }
                // set identity matrix default
                m00(1);
                m11(1);
                m22(1);
                m33(1);
            }
            
            /*explicit mat4x4(glm::mat4x4 mat) {
                m00(mat[0][0]);
                m01(mat[1][0]);
                m02(mat[2][0]);
                m03(mat[3][0]);
    
                m10(mat[0][1]);
                m11(mat[1][1]);
                m12(mat[2][1]);
                m13(mat[3][1]);
    
                m20(mat[0][2]);
                m21(mat[1][2]);
                m22(mat[2][2]);
                m23(mat[3][2]);
    
                m30(mat[0][3]);
                m31(mat[1][3]);
                m32(mat[2][3]);
                m33(mat[3][3]);
            }*/
            mat4x4(const mat4x4& mat) {
                for (int i = 0; i < 16; i++) {
                    data.single[i] = mat.data.single[i];
                }
            }
            
            explicit mat4x4(const float dat[16]) {
                for (int i = 0; i < 16; i++) {
                    data.single[i] = dat[i];
                }
            }
            
            inline mat4x4& translate(float x, float y, float z) {
                m03(x);
                m13(y);
                m23(z);
                return *this;
            }
            
            inline mat4x4& translate(const vec4& vec) { return translate(vec[0], vec[1], vec[2]); }
        
            inline mat4x4& translate(const vec3& vec) { return translate(vec[0], vec[1], vec[2]); }
            
            inline mat4x4& scale(float x, float y, float z) {
                m00(x);
                m11(y);
                m22(z);
                return *this;
            }
            
            inline mat4x4& scale(const vec4& vec) { return scale(vec[0], vec[1], vec[2]); }
            inline mat4x4& scale(const vec3& vec) { return scale(vec[0], vec[1], vec[2]); }
            
            inline float* ptr() { return data.single; }
            
            mat4x4& transpose() {
                mat4x4 copy{*this};
                
                m00(copy.m00());
                m01(copy.m10());
                m02(copy.m20());
                m03(copy.m30());
                
                m10(copy.m01());
                m11(copy.m11());
                m12(copy.m21());
                m13(copy.m31());
                
                m20(copy.m02());
                m21(copy.m12());
                m22(copy.m22());
                m23(copy.m32());
                
                m30(copy.m03());
                m31(copy.m13());
                m32(copy.m23());
                m33(copy.m33());
                
                return *this;
            }
            
            // Due to the conversion between the 2d array -> 1d array we must transpose the values.
            // the old system has been archived (commented) for future debugging
//            [[nodiscard]] inline float m00() const { return data.dim[0][0]; }
//            [[nodiscard]] inline float m10() const { return data.dim[1][0]; }
//            [[nodiscard]] inline float m20() const { return data.dim[2][0]; }
//            [[nodiscard]] inline float m30() const { return data.dim[3][0]; }
//            [[nodiscard]] inline float m01() const { return data.dim[0][1]; }
//            [[nodiscard]] inline float m11() const { return data.dim[1][1]; }
//            [[nodiscard]] inline float m21() const { return data.dim[2][1]; }
//            [[nodiscard]] inline float m31() const { return data.dim[3][1]; }
//            [[nodiscard]] inline float m02() const { return data.dim[0][2]; }
//            [[nodiscard]] inline float m12() const { return data.dim[1][2]; }
//            [[nodiscard]] inline float m22() const { return data.dim[2][2]; }
//            [[nodiscard]] inline float m32() const { return data.dim[3][2]; }
//            [[nodiscard]] inline float m03() const { return data.dim[0][3]; }
//            [[nodiscard]] inline float m13() const { return data.dim[1][3]; }
//            [[nodiscard]] inline float m23() const { return data.dim[2][3]; }
//            [[nodiscard]] inline float m33() const { return data.dim[3][3]; }
//            [[nodiscard]] inline float m(int i, int j) const { return data.dim[i][j]; };
//            inline float m00(float d) { return data.dim[0][0] = d; }
//            inline float m10(float d) { return data.dim[1][0] = d; }
//            inline float m20(float d) { return data.dim[2][0] = d; }
//            inline float m30(float d) { return data.dim[3][0] = d; }
//            inline float m01(float d) { return data.dim[0][1] = d; }
//            inline float m11(float d) { return data.dim[1][1] = d; }
//            inline float m21(float d) { return data.dim[2][1] = d; }
//            inline float m31(float d) { return data.dim[3][1] = d; }
//            inline float m02(float d) { return data.dim[0][2] = d; }
//            inline float m12(float d) { return data.dim[1][2] = d; }
//            inline float m22(float d) { return data.dim[2][2] = d; }
//            inline float m32(float d) { return data.dim[3][2] = d; }
//            inline float m03(float d) { return data.dim[0][3] = d; }
//            inline float m13(float d) { return data.dim[1][3] = d; }
//            inline float m23(float d) { return data.dim[2][3] = d; }
//            inline float m33(float d) { return data.dim[3][3] = d; }
            
            [[nodiscard]] inline float m00() const { return data.dim[0][0]; }
            
            [[nodiscard]] inline float m10() const { return data.dim[0][1]; }
            
            [[nodiscard]] inline float m20() const { return data.dim[0][2]; }
            
            [[nodiscard]] inline float m30() const { return data.dim[0][3]; }
            
            [[nodiscard]] inline float m01() const { return data.dim[1][0]; }
            
            [[nodiscard]] inline float m11() const { return data.dim[1][1]; }
            
            [[nodiscard]] inline float m21() const { return data.dim[1][2]; }
            
            [[nodiscard]] inline float m31() const { return data.dim[1][3]; }
            
            [[nodiscard]] inline float m02() const { return data.dim[2][0]; }
            
            [[nodiscard]] inline float m12() const { return data.dim[2][1]; }
            
            [[nodiscard]] inline float m22() const { return data.dim[2][2]; }
            
            [[nodiscard]] inline float m32() const { return data.dim[2][3]; }
            
            [[nodiscard]] inline float m03() const { return data.dim[3][0]; }
            
            [[nodiscard]] inline float m13() const { return data.dim[3][1]; }
            
            [[nodiscard]] inline float m23() const { return data.dim[3][2]; }
            
            [[nodiscard]] inline float m33() const { return data.dim[3][3]; }
            
            [[nodiscard]] inline float m(int i, int j) const { return data.dim[i][j]; };
            
            inline float m00(float d) { return data.dim[0][0] = d; }
            
            inline float m10(float d) { return data.dim[0][1] = d; }
            
            inline float m20(float d) { return data.dim[0][2] = d; }
            
            inline float m30(float d) { return data.dim[0][3] = d; }
            
            inline float m01(float d) { return data.dim[1][0] = d; }
            
            inline float m11(float d) { return data.dim[1][1] = d; }
            
            inline float m21(float d) { return data.dim[1][2] = d; }
            
            inline float m31(float d) { return data.dim[1][3] = d; }
            
            inline float m02(float d) { return data.dim[2][0] = d; }
            
            inline float m12(float d) { return data.dim[2][1] = d; }
            
            inline float m22(float d) { return data.dim[2][2] = d; }
            
            inline float m32(float d) { return data.dim[2][3] = d; }
            
            inline float m03(float d) { return data.dim[3][0] = d; }
            
            inline float m13(float d) { return data.dim[3][1] = d; }
            
            inline float m23(float d) { return data.dim[3][2] = d; }
            
            inline float m33(float d) { return data.dim[3][3] = d; }
            
            inline float m(int i, int j, float d) { return data.dim[i][j] = d; };
//            inline float* operator [](int _i) {
//                return data.dim[_i];
//            }
            
            [[nodiscard]] float determinant() const {
                return m00() * (m11() * m22() * m33() + m12() * m23() * m31() + m13() * m21() * m32()
                                - m31() * m22() * m13() - m32() * m23() * m11() - m33() * m21() * m12())
                       - m10() * (m01() * m22() * m33() + m02() * m23() * m31() + m03() * m21() * m32()
                                  - m31() * m32() * m03() - m32() * m23() * m01() - m33() * m21() * m02())
                       + m20() * (m01() * m12() * m33() + m02() * m13() * m31() + m03() * m11() * m32()
                                  - m31() * m12() * m03() - m32() * m13() * m01() - m33() * m11() * m02())
                       - m30() * (m01() * m12() * m23() + m02() * m13() * m21() + m03() * m11() * m22()
                                  - m21() * m12() * m03() - m22() * m13() * m01() - m23() * m11() * m02());
            }
    };
    
    // adds the two mat4x4 left and right
    inline mat4x4 operator+(const mat4x4& left, const mat4x4& right) {
        float data[16];
        for (int i = 0; i < 16; i++)
            data[i] = left.data.single[i] + right.data.single[i];
        return mat4x4{data};
    }
    
    // subtracts the right mat4x4 from the left.
    inline mat4x4 operator-(const mat4x4& left, const mat4x4& right) {
        float data[16];
        for (int i = 0; i < 16; i++)
            data[i] = left.data.single[i] - right.data.single[i];
        return mat4x4{data};
    }
    
    // since matrices are made identity by default, we need to create the result collector matrix without identity
    // otherwise the diagonal will be 1 off and cause weird results (see black screen issue)
    constexpr float emptyMatrix[16] = {0, 0, 0, 0,
                                       0, 0, 0, 0,
                                       0, 0, 0, 0,
                                       0, 0, 0, 0};
    
    // multiples the left with the right
    inline mat4x4 operator*(const mat4x4& left, const mat4x4& right) {
        mat4x4 mat{emptyMatrix};
        
        // TODO: check avx with this??
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < 4; k++) {
                    mat.m(i, j, mat.m(i, j) + left.m(i, k) * right.m(k, j));
                }
            }
        }
        
        return mat;
    }
    
    // multiplies the const c with each element in the mat4x4 v
    inline mat4x4 operator*(float c, const mat4x4& v) {
        mat4x4 mat{};
        
        for (int i = 0; i < 16; i++) {
            mat.data.single[i] = c * v.data.single[i];
        }
        
        return mat;
    }
    
    // same as above but for right sided constants
    inline mat4x4 operator*(const mat4x4& v, float c) {
        mat4x4 mat{};
        
        for (int i = 0; i < 16; i++) {
            mat.data.single[i] = v.data.single[i] * c;
        }
        
        return mat;
    }
    
    // divides the mat4x4 by the constant c
    inline mat4x4 operator/(const mat4x4& v, float c) {
        mat4x4 mat{};
        
        for (int i = 0; i < 16; i++) {
            mat.data.single[i] = v.data.single[i] / c;
        }
        
        return mat;
    }
    
    // divides each element in the mat4x4 by over the constant
    inline mat4x4 operator/(float c, const mat4x4& v) {
        mat4x4 mat{};
        
        for (int i = 0; i < 16; i++) {
            mat.data.single[i] = c / v.data.single[i];
        }
        
        return mat;
    }
    
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix.html
    static inline mat4x4 perspective(float fov, float near, float far){
        mat4x4 perspectiveMat4x4;
        
        float scale = 1.0f / (float)tan(fov * 0.5f * M_PI / 180.0f);
        perspectiveMat4x4.m00(scale);
        perspectiveMat4x4.m11(scale);
        perspectiveMat4x4.m22(-far / (far - near));
        perspectiveMat4x4.m32(-far * near / (far - near));
        perspectiveMat4x4.m23(-1);
        perspectiveMat4x4.m33(0);
        
        return perspectiveMat4x4;
    }
    
//    inline std::ostream& operator<<(std::ostream& out, const mat4x4& v) {
//        return out << "\rMatrix4x4{" << v.m00() << ", " << v.m01() << ", " << v.m02() << ", " << v.m03() << "} \n"\
// << "         {" << v.m10() << ", " << v.m11() << ", " << v.m12() << ", " << v.m13() << "} \n"\
// << "         {" << v.m20() << ", " << v.m21() << ", " << v.m22() << ", " << v.m23() << "} \n"\
// << "         {" << v.m30() << ", " << v.m31() << ", " << v.m32() << ", " << v.m33() << "} \n";
//    }

}

#endif //BLT_MATH_H
