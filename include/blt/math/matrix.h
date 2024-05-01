/*
 * Created by Brett on 28/02/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_MATRIX_H
#define BLT_TESTS_MATRIX_H

#include <blt/math/vectors.h>
#include <cstring>

#ifndef M_PI
// MSVC does not have M_PI
# define M_PI		3.14159265358979323846
#endif

namespace blt
{
    
    class mat4x4
    {
        protected:
            // 4x4 = 16
            union dataType
            {
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
            mat4x4()
            {
                for (float& i : data.single)
                    i = 0;
                // set identity matrix default
                m00(1);
                m11(1);
                m22(1);
                m33(1);
            }
            
            mat4x4(const blt::vec4& c1, const blt::vec4& c2, const blt::vec4& c3, const blt::vec4& c4)
            {
                // dangerous?
                std::memcpy(data.dim[0], c1.data(), 4 * sizeof(float));
                std::memcpy(data.dim[1], c2.data(), 4 * sizeof(float));
                std::memcpy(data.dim[2], c3.data(), 4 * sizeof(float));
                std::memcpy(data.dim[3], c4.data(), 4 * sizeof(float));
            }
            
            mat4x4(const mat4x4& mat)
            {
                for (int i = 0; i < 16; i++)
                {
                    data.single[i] = mat.data.single[i];
                }
            }
            
            mat4x4& operator=(const mat4x4& copy)
            {
                if (&copy == this)
                    return *this;
                for (int i = 0; i < 16; i++)
                {
                    data.single[i] = copy.data.single[i];
                }
                return *this;
            }
            
            explicit mat4x4(const float dat[16])
            {
                for (int i = 0; i < 16; i++)
                {
                    data.single[i] = dat[i];
                }
            }
            
            inline mat4x4& translate(float x, float y, float z)
            {
                mat4x4 translation_mat{};
                /**
                 * 9.005 Are OpenGL matrices column-major or row-major?
                 * For programming purposes, OpenGL matrices are 16-value arrays with base vectors laid out contiguously in memory.
                 * The translation components occupy the 13th, 14th, and 15th elements of the 16-element matrix,
                 * where indices are numbered from 1 to 16 as described in section 2.11.2 of the OpenGL 2.1 Specification.
                 */
                translation_mat.m03(x);
                translation_mat.m13(y);
                translation_mat.m23(z);
                
                *this = *this * translation_mat;
                
                return *this;
            }
            
            inline mat4x4& translate(const vec4& vec)
            { return translate(vec[0], vec[1], vec[2]); }
            
            inline mat4x4& translate(const vec3& vec)
            { return translate(vec[0], vec[1], vec[2]); }
            
            inline mat4x4& scale(float x, float y, float z)
            {
                mat4x4 scale_mat{};
                
                m00(m00() * x);
                m11(m11() * y);
                m22(m22() * z);
                
                *this = *this * scale_mat;
                
                return *this;
            }
            
            inline mat4x4& scale(const vec4& vec)
            { return scale(vec[0], vec[1], vec[2]); }
            
            inline mat4x4& scale(const vec3& vec)
            { return scale(vec[0], vec[1], vec[2]); }
            
            // angle in radians
            inline mat4x4& rotateX(float angle)
            {
                mat4x4 rotationMatrix{};
                rotationMatrix.m(1, 1, std::cos(angle));
                rotationMatrix.m(1, 2, -std::sin(angle));
                rotationMatrix.m(2, 1, std::sin(angle));
                rotationMatrix.m(2, 2, std::cos(angle));
                
                *this = *this * rotationMatrix;
                
                return *this;
            }
            
            inline mat4x4& rotateY(float angle)
            {
                mat4x4 rotationMatrix{};
                rotationMatrix.m(0, 0, std::cos(angle));
                rotationMatrix.m(0, 2, std::sin(angle));
                rotationMatrix.m(2, 0, -std::sin(angle));
                rotationMatrix.m(2, 2, std::cos(angle));
                
                *this = *this * rotationMatrix;
                
                return *this;
            }
            
            inline mat4x4& rotateZ(float angle)
            {
                mat4x4 rotationMatrix{};
                rotationMatrix.m(0, 0, std::cos(angle));
                rotationMatrix.m(0, 1, -std::sin(angle));
                rotationMatrix.m(1, 0, std::sin(angle));
                rotationMatrix.m(1, 1, std::cos(angle));
                
                *this = *this * rotationMatrix;
                
                return *this;
            }
            
            [[nodiscard]] mat4x4 transpose() const
            {
                mat4x4 copy{*this};
                
                for (int i = 0; i < 4; i++)
                {
                    for (int j = 0; j < 4; j++)
                    {
                        copy.m(j, i, m(i, j));
                    }
                }
                
                return copy;
            }
            
            [[nodiscard]] float determinant() const
            {
                return m00() * (m11() * m22() * m33() + m12() * m23() * m31() + m13() * m21() * m32()
                                - m31() * m22() * m13() - m32() * m23() * m11() - m33() * m21() * m12())
                       - m10() * (m01() * m22() * m33() + m02() * m23() * m31() + m03() * m21() * m32()
                                  - m31() * m32() * m03() - m32() * m23() * m01() - m33() * m21() * m02())
                       + m20() * (m01() * m12() * m33() + m02() * m13() * m31() + m03() * m11() * m32()
                                  - m31() * m12() * m03() - m32() * m13() * m01() - m33() * m11() * m02())
                       - m30() * (m01() * m12() * m23() + m02() * m13() * m21() + m03() * m11() * m22()
                                  - m21() * m12() * m03() - m22() * m13() * m01() - m23() * m11() * m02());
            }
            
            [[nodiscard]] mat4x4 adjugate() const
            {
                mat4x4 ad;
                ad.w11(w22() * w33() * w44() + w23() * w34() * w42() + w24() * w32() * w43()
                       - w24() * w33() * w42() - w23() * w32() * w44() - w22() * w34() * w43());
                ad.w12(w21() * w33() * w44() + w23() * w34() * w41() + w24() * w31() * w43()
                       - w24() * w33() * w41() - w23() * w31() * w44() - w21() * w34() * w43());
                ad.w13(w21() * w32() * w44() + w22() * w34() * w41() + w24() * w31() * w42()
                       - w24() * w32() * w41() - w22() * w31() * w44() - w21() * w34() * w42());
                ad.w14(w21() * w32() * w43() + w22() * w33() * w41() + w23() * w31() * w42()
                       - w23() * w32() * w41() - w22() * w31() * w43() - w21() * w33() * w42());
                
                ad.w21(w12() * w33() * w44() + w13() * w34() * w42() + w14() * w32() * w43()
                       - w14() * w33() * w42() - w13() * w32() * w44() - w12() * w34() * w43());
                ad.w22(w11() * w33() * w44() + w13() * w34() * w41() + w14() * w31() * w43()
                       - w14() * w33() * w41() - w13() * w31() * w44() - w11() * w34() * w43());
                ad.w23(w11() * w32() * w44() + w12() * w34() * w41() + w14() * w31() * w42()
                       - w14() * w32() * w41() - w12() * w31() * w44() - w11() * w34() * w42());
                ad.w24(w11() * w32() * w43() + w12() * w33() * w41() + w13() * w31() * w42()
                       - w13() * w32() * w41() - w12() * w31() * w43() - w11() * w33() * w42());
                
                ad.w31(w12() * w23() * w44() + w13() * w24() * w42() + w14() * w22() * w43()
                       - w14() * w23() * w42() - w13() * w22() * w44() - w12() * w24() * w43());
                ad.w32(w11() * w23() * w44() + w13() * w24() * w41() + w14() * w21() * w43()
                       - w14() * w23() * w41() - w13() * w21() * w44() - w11() * w24() * w43());
                ad.w33(w11() * w22() * w44() + w12() * w24() * w41() + w14() * w21() * w42()
                       - w14() * w22() * w41() - w12() * w21() * w44() - w11() * w24() * w42());
                ad.w34(w11() * w22() * w43() + w12() * w23() * w41() + w13() * w21() * w42()
                       - w13() * w22() * w41() - w12() * w21() * w43() - w11() * w23() * w42());
                
                ad.w41(w12() * w23() * w34() + w13() * w24() * w32() + w14() * w22() * w33()
                       - w14() * w23() * w32() - w13() * w22() * w34() - w12() * w24() * w33());
                ad.w42(w11() * w23() * w34() + w13() * w24() * w31() + w14() * w21() * w33()
                       - w14() * w23() * w31() - w13() * w21() * w34() - w11() * w24() * w33());
                ad.w43(w11() * w22() * w34() + w12() * w24() * w31() + w14() * w21() * w32()
                       - w14() * w22() * w31() - w12() * w21() * w34() - w11() * w24() * w32());
                ad.w44(w11() * w22() * w33() + w12() * w23() * w31() + w13() * w21() * w32()
                       - w13() * w22() * w31() - w12() * w21() * w33() - w11() * w23() * w32());
                
                for (int i = 1; i <= 4; i++)
                {
                    for (int j = 1; j <= 4; j++)
                    {
                        auto v = static_cast<float>(std::pow(-1, j + i));
                        ad.w(j, i, v * ad.w(j, i));
                    }
                }
                return ad;
            }
            
            [[nodiscard]] mat4x4 inverse() const
            {
                auto ad = adjugate();
                auto d = 1 / determinant();
                return d * ad;
            }
            
            [[nodiscard]] inline float m(int row, int column) const
            { return data.single[row + column * 4]; };
            
            [[nodiscard]] inline float m00() const
            { return m(0, 0); }
            
            [[nodiscard]] inline float m10() const
            { return m(1, 0); }
            
            [[nodiscard]] inline float m20() const
            { return m(2, 0); }
            
            [[nodiscard]] inline float m30() const
            { return m(3, 0); }
            
            [[nodiscard]] inline float m01() const
            { return m(0, 1); }
            
            [[nodiscard]] inline float m11() const
            { return m(1, 1); }
            
            [[nodiscard]] inline float m21() const
            { return m(2, 1); }
            
            [[nodiscard]] inline float m31() const
            { return m(3, 1); }
            
            [[nodiscard]] inline float m02() const
            { return m(0, 2); }
            
            [[nodiscard]] inline float m12() const
            { return m(1, 2); }
            
            [[nodiscard]] inline float m22() const
            { return m(2, 2); }
            
            [[nodiscard]] inline float m32() const
            { return m(3, 2); }
            
            [[nodiscard]] inline float m03() const
            { return m(0, 3); }
            
            [[nodiscard]] inline float m13() const
            { return m(1, 3); }
            
            [[nodiscard]] inline float m23() const
            { return m(2, 3); }
            
            [[nodiscard]] inline float m33() const
            { return m(3, 3); }
            
            inline float m(int row, int column, float value)
            { return data.single[row + column * 4] = value; };
            
            inline float m00(float d)
            { return m(0, 0, d); }
            
            inline float m10(float d)
            { return m(1, 0, d); }
            
            inline float m20(float d)
            { return m(2, 0, d); }
            
            inline float m30(float d)
            { return m(3, 0, d); }
            
            inline float m01(float d)
            { return m(0, 1, d); }
            
            inline float m11(float d)
            { return m(1, 1, d); }
            
            inline float m21(float d)
            { return m(2, 1, d); }
            
            inline float m31(float d)
            { return m(3, 1, d); }
            
            inline float m02(float d)
            { return m(0, 2, d); }
            
            inline float m12(float d)
            { return m(1, 2, d); }
            
            inline float m22(float d)
            { return m(2, 2, d); }
            
            inline float m32(float d)
            { return m(3, 2, d); }
            
            inline float m03(float d)
            { return m(0, 3, d); }
            
            inline float m13(float d)
            { return m(1, 3, d); }
            
            inline float m23(float d)
            { return m(2, 3, d); }
            
            inline float m33(float d)
            { return m(3, 3, d); }
            
            [[nodiscard]] inline float w(int row, int column) const
            { return data.single[(row - 1) + (column - 1) * 4]; };
            
            [[nodiscard]] inline float w11() const
            { return m(0, 0); }
            
            [[nodiscard]] inline float w21() const
            { return m(1, 0); }
            
            [[nodiscard]] inline float w31() const
            { return m(2, 0); }
            
            [[nodiscard]] inline float w41() const
            { return m(3, 0); }
            
            [[nodiscard]] inline float w12() const
            { return m(0, 1); }
            
            [[nodiscard]] inline float w22() const
            { return m(1, 1); }
            
            [[nodiscard]] inline float w32() const
            { return m(2, 1); }
            
            [[nodiscard]] inline float w42() const
            { return m(3, 1); }
            
            [[nodiscard]] inline float w13() const
            { return m(0, 2); }
            
            [[nodiscard]] inline float w23() const
            { return m(1, 2); }
            
            [[nodiscard]] inline float w33() const
            { return m(2, 2); }
            
            [[nodiscard]] inline float w43() const
            { return m(3, 2); }
            
            [[nodiscard]] inline float w14() const
            { return m(0, 3); }
            
            [[nodiscard]] inline float w24() const
            { return m(1, 3); }
            
            [[nodiscard]] inline float w34() const
            { return m(2, 3); }
            
            [[nodiscard]] inline float w44() const
            { return m(3, 3); }
            
            inline float w(int row, int column, float value)
            { return data.single[(row - 1) + (column - 1) * 4] = value; };
            
            inline float w11(float d)
            { return m(0, 0, d); }
            
            inline float w21(float d)
            { return m(1, 0, d); }
            
            inline float w31(float d)
            { return m(2, 0, d); }
            
            inline float w41(float d)
            { return m(3, 0, d); }
            
            inline float w12(float d)
            { return m(0, 1, d); }
            
            inline float w22(float d)
            { return m(1, 1, d); }
            
            inline float w32(float d)
            { return m(2, 1, d); }
            
            inline float w42(float d)
            { return m(3, 1, d); }
            
            inline float w13(float d)
            { return m(0, 2, d); }
            
            inline float w23(float d)
            { return m(1, 2, d); }
            
            inline float w33(float d)
            { return m(2, 2, d); }
            
            inline float w43(float d)
            { return m(3, 2, d); }
            
            inline float w14(float d)
            { return m(0, 3, d); }
            
            inline float w24(float d)
            { return m(1, 3, d); }
            
            inline float w34(float d)
            { return m(2, 3, d); }
            
            inline float w44(float d)
            { return m(3, 3, d); }
            
            inline float* ptr()
            { return data.single; }
    };
    
    // adds the two mat4x4 left and right
    inline mat4x4 operator+(const mat4x4& left, const mat4x4& right)
    {
        float data[16];
        for (int i = 0; i < 16; i++)
            data[i] = left.data.single[i] + right.data.single[i];
        return mat4x4{data};
    }
    
    // subtracts the right mat4x4 from the left.
    inline mat4x4 operator-(const mat4x4& left, const mat4x4& right)
    {
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
    inline mat4x4 operator*(const mat4x4& left, const mat4x4& right)
    {
        mat4x4 mat{emptyMatrix};
        
        // TODO: check avx with this??
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                for (int k = 0; k < 4; k++)
                {
                    mat.m(i, j, mat.m(i, j) + left.m(i, k) * right.m(k, j));
                }
            }
        }
        
        return mat;
    }
    
    inline vec4 operator*(const mat4x4& left, const vec4& right)
    {
        vec4 ret{0, 0, 0, 0};
        
        for (int m = 0; m < 4; m++)
        {
            for (int n = 0; n < 4; n++)
            {
                ret[m] = ret[m] + left.m(m, n) * right[n];
            }
        }
        
        return ret;
    }
    
    template<typename T, unsigned long size>
    inline vec4 operator*(const mat4x4& left, const vec<T, size>& right)
    {
        vec4 ret{0, 0, 0, 0};
        
        for (int i = 0; i < size; i++)
            ret[i] = (float) right[i];
        
        return left * ret;
    }
    
    // multiplies the const c with each element in the mat4x4 v
    inline mat4x4 operator*(float c, const mat4x4& v)
    {
        mat4x4 mat{};
        
        for (int i = 0; i < 16; i++)
        {
            mat.data.single[i] = c * v.data.single[i];
        }
        
        return mat;
    }
    
    // same as above but for right sided constants
    inline mat4x4 operator*(const mat4x4& v, float c)
    {
        mat4x4 mat{};
        
        for (int i = 0; i < 16; i++)
        {
            mat.data.single[i] = v.data.single[i] * c;
        }
        
        return mat;
    }
    
    // divides the mat4x4 by the constant c
    inline mat4x4 operator/(const mat4x4& v, float c)
    {
        mat4x4 mat{};
        
        for (int i = 0; i < 16; i++)
        {
            mat.data.single[i] = v.data.single[i] / c;
        }
        
        return mat;
    }
    
    // divides each element in the mat4x4 by over the constant
    inline mat4x4 operator/(float c, const mat4x4& v)
    {
        mat4x4 mat{};
        
        for (int i = 0; i < 16; i++)
        {
            mat.data.single[i] = c / v.data.single[i];
        }
        
        return mat;
    }
    
    // https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix/building-basic-perspective-projection-matrix.html
    // https://ogldev.org/www/tutorial12/tutorial12.html
    // http://www.songho.ca/opengl/gl_projectionmatrix.html
    static inline mat4x4 perspective(float fov, float aspect_ratio, float near, float far)
    {
        mat4x4 perspectiveMat4x4{emptyMatrix};
        
        float halfTan = tanf(fov * 0.5f * (float) M_PI / 180.0f);
        perspectiveMat4x4.m00(float(1.0 / (aspect_ratio * halfTan)));
        perspectiveMat4x4.m11(float(1.0 / halfTan));
        perspectiveMat4x4.m22(float(-((far + near) / (far - near))));
        perspectiveMat4x4.m32(-1);
        perspectiveMat4x4.m23(float(-((2 * near * far) / (far - near))));
        
        return perspectiveMat4x4;
    }
    
    static inline mat4x4 ortho(float left, float right, float top, float bottom, float near, float far)
    {
        mat4x4 perspectiveMat4x4{emptyMatrix};
        
        perspectiveMat4x4.m00(2 / (right - left));
        perspectiveMat4x4.m11(2 / (top - bottom));
        perspectiveMat4x4.m22(2 / (far - near));
        perspectiveMat4x4.m33(1);
        perspectiveMat4x4.m03(-(right + left) / (right - left));
        perspectiveMat4x4.m13(-(top + bottom) / (top - bottom));
        perspectiveMat4x4.m23(-(far + near) / (far - near));
        
        return perspectiveMat4x4;
    }
    
}

#endif //BLT_TESTS_MATRIX_H
