/*
 * Created by Brett on 28/02/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_MATRIX_H
#define BLT_TESTS_MATRIX_H

#include <blt/math/vectors.h>

namespace blt {
    
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
                for (float& i : data.single)
                    i = 0;
                // set identity matrix default
                m00(1);
                m11(1);
                m22(1);
                m33(1);
            }
            
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
                /**
                 * 9.005 Are OpenGL matrices column-major or row-major?
                 * For programming purposes, OpenGL matrices are 16-value arrays with base vectors laid out contiguously in memory.
                 * The translation components occupy the 13th, 14th, and 15th elements of the 16-element matrix,
                 * where indices are numbered from 1 to 16 as described in section 2.11.2 of the OpenGL 2.1 Specification.
                 */
                m03(x);
                m13(y);
                m23(z);
                return *this;
            }
            
            inline mat4x4& translate(const vec4& vec) { return translate(vec[0], vec[1], vec[2]); }
            inline mat4x4& translate(const vec3& vec) { return translate(vec[0], vec[1], vec[2]); }
            
            inline mat4x4& scale(float x, float y, float z) {
                m00(m00() * x);
                m11(m11() * y);
                m22(m11() * z);
                return *this;
            }
            
            inline mat4x4& scale(const vec4& vec) { return scale(vec[0], vec[1], vec[2]); }
            inline mat4x4& scale(const vec3& vec) { return scale(vec[0], vec[1], vec[2]); }
            
            [[nodiscard]] mat4x4 transpose() const {
                mat4x4 copy{*this};
                
                for (int i = 0; i < 4; i++){
                    for (int j = 0; j < 4; j++) {
                        copy.m(j, i, m(i, j));
                    }
                }
                
                return copy;
            }
            
            [[nodiscard]] inline float m(int row, int column) const { return data.single[row + column * 4]; };

            [[nodiscard]] inline float m00() const { return m(0, 0); }
            
            [[nodiscard]] inline float m10() const { return m(1, 0); }
            
            [[nodiscard]] inline float m20() const { return m(2, 0); }
            
            [[nodiscard]] inline float m30() const { return m(3, 0); }
            
            [[nodiscard]] inline float m01() const { return m(0, 1); }
            
            [[nodiscard]] inline float m11() const { return m(1, 1); }
            
            [[nodiscard]] inline float m21() const { return m(2, 1); }
            
            [[nodiscard]] inline float m31() const { return m(3, 1); }
            
            [[nodiscard]] inline float m02() const { return m(0, 2); }
            
            [[nodiscard]] inline float m12() const { return m(1, 2); }
            
            [[nodiscard]] inline float m22() const { return m(2, 2); }
            
            [[nodiscard]] inline float m32() const { return m(3, 2); }
            
            [[nodiscard]] inline float m03() const { return m(0, 3); }
            
            [[nodiscard]] inline float m13() const { return m(1, 3); }
            
            [[nodiscard]] inline float m23() const { return m(2, 3); }
            
            [[nodiscard]] inline float m33() const { return m(3, 3); }
            
            inline float m(int row, int column, float value) { return data.single[row + column * 4] = value; };
            
            inline float m00(float d) { return m(0, 0, d); }
            
            inline float m10(float d) { return m(1, 0, d); }
            
            inline float m20(float d) { return m(2, 0, d); }
            
            inline float m30(float d) { return m(3, 0, d); }
            
            inline float m01(float d) { return m(0, 1, d); }
            
            inline float m11(float d) { return m(1, 1, d); }
            
            inline float m21(float d) { return m(2, 1, d); }
            
            inline float m31(float d) { return m(3, 1, d); }
            
            inline float m02(float d) { return m(0, 2, d); }
            
            inline float m12(float d) { return m(1, 2, d); }
            
            inline float m22(float d) { return m(2, 2, d); }
            
            inline float m32(float d) { return m(3, 2, d); }
            
            inline float m03(float d) { return m(0, 3, d); }
            
            inline float m13(float d) { return m(1, 3, d); }
            
            inline float m23(float d) { return m(2, 3, d); }
            
            inline float m33(float d) { return m(3, 3, d); }
            
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
            
            inline float* ptr() { return data.single; }
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
    // https://ogldev.org/www/tutorial12/tutorial12.html
    // http://www.songho.ca/opengl/gl_projectionmatrix.html
    static inline mat4x4 perspective(float fov, float aspect_ratio, float near, float far){
        mat4x4 perspectiveMat4x4 {emptyMatrix};
        
        float oneOverNearMFar = 1.0f / (near - far);
        
        float halfTan = tanf(fov * 0.5f * (float)M_PI / 180.0f);
        perspectiveMat4x4.m00(float(1.0 / (aspect_ratio * halfTan)));
        perspectiveMat4x4.m11(float(1.0 / halfTan));
        perspectiveMat4x4.m22(float(-((far + near) / (far - near))));
        perspectiveMat4x4.m32(-1);
        perspectiveMat4x4.m23(float(-((2 * near * far) / (far - near))));
        
        return perspectiveMat4x4;
    }
    
    static inline mat4x4 ortho(float left, float right, float top, float bottom, float near, float far){
        mat4x4 perspectiveMat4x4 {emptyMatrix};
    
        perspectiveMat4x4.m00(2/(right - left));
        perspectiveMat4x4.m11(2/(top-bottom));
        perspectiveMat4x4.m22(2/(far-near));
        perspectiveMat4x4.m33(1);
        perspectiveMat4x4.m03(-(right + left) / (right - left));
        perspectiveMat4x4.m13(-(top + bottom) / (top - bottom));
        perspectiveMat4x4.m23(-(far + near) / (far - near));
        
        return perspectiveMat4x4;
    }

}

#endif //BLT_TESTS_MATRIX_H
