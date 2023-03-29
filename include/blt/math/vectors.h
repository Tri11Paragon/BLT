/*
 * Created by Brett on 28/02/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_VECTORS_H
#define BLT_TESTS_VECTORS_H

#include <initializer_list>
#include <cmath>
#include <vector>

namespace blt {
    
    constexpr float EPSILON = 0.0001f;
    
    static inline constexpr bool f_equal(float v1, float v2) {
        return v1 >= v2 - EPSILON && v1 <= v2 + EPSILON;
    }
    
    template<typename T, unsigned long size, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
    struct vec {
        private:
            T elements[size]{};
        public:
            
            vec() {
                for (int i = 0; i < size; i++)
                    elements[i] = 0;
            }
            
            vec(std::initializer_list<T> args): vec() {
                for (int i = 0; i < args.size(); i++) {
                    elements[i] = *(args.begin() + i);
                }
            }
            
            explicit vec(const T elem[size]) {
                for (int i = 0; i < size; i++) {
                    elements[i] = elem[i];
                }
            }
            
            vec(const vec<T, size>& copy): vec(copy.elements) {}
            
            [[nodiscard]] inline T x() const {
                return elements[0];
            }
            
            [[nodiscard]] inline T y() const {
                static_assert(size > 1);
                return elements[1];
            }
            
            [[nodiscard]] inline T z() const {
                static_assert(size > 2);
                return elements[2];
            }
            
            [[nodiscard]] inline T w() const {
                static_assert(size > 3);
                return elements[3];
            }
            
            [[nodiscard]] inline T magnitude() const {
                T total = 0;
                for (int i = 0; i < size; i++)
                    total += elements[i] * elements[i];
                return std::sqrt(total);
            }
            
            [[nodiscard]] inline vec<T, size> normalize() const {
                auto mag = this->magnitude();
                if (mag == 0)
                    return *this;
                return *this / mag;
            }
            
            inline T& operator[](int index) {
                return elements[index];
            }
            
            inline T operator[](int index) const {
                return elements[index];
            }
            
            inline vec<T, size>& operator=(T v) {
                for (int i = 0; i < size; i++)
                    elements[i] = v;
                return *this;
            }
            
            inline vec<T, size> operator-() {
                T negativeCopy[size];
                for (int i = 0; i < size; i++)
                    negativeCopy[i] = -elements[i];
                return vec<T, size>{negativeCopy};
            }
            
            inline vec<T, size>& operator+=(const vec<T, size>& other) {
                for (int i = 0; i < size; i++)
                    elements[i] += other[i];
                return *this;
            }
            
            inline vec<T, size>& operator*=(const vec<T, size>& other) {
                for (int i = 0; i < size; i++)
                    elements[i] *= other[i];
                return *this;
            }
            
            inline vec<T, size>& operator+=(T f) {
                for (int i = 0; i < size; i++)
                    elements[i] += f;
                return *this;
            }
            
            inline vec<T, size>& operator*=(T f) {
                for (int i = 0; i < size; i++)
                    elements[i] *= f;
                return *this;
            }
            
            inline vec<T, size>& operator-=(const vec<T, size>& other) {
                for (int i = 0; i < size; i++)
                    elements[i] -= other[i];
                return *this;
            }
            
            inline vec<T, size>& operator-=(T f) {
                for (int i = 0; i < size; i++)
                    elements[i] -= f;
                return *this;
            }
            
            /**
             * performs the dot product of left * right
             */
            static inline constexpr T dot(const vec<T, size>& left, const vec<T, size>& right) {
                T dot = 0;
                for (int i = 0; i < size; i++)
                    dot += left[i] * right[i];
                return dot;
            }
            
            static inline constexpr vec<T, size> cross(const vec<T, size>& left, const vec<T, size>& right) {
                // cross is only defined on vectors of size 3. 2D could be implemented, which is a TODO
                static_assert(size == 3);
                return {left.y() * right.z() - left.z() * right.y(),
                        left.z() * right.x() - left.x() * right.z(),
                        left.x() * right.y() - left.y() * right.x()};
            }
            
            static inline constexpr vec<T, size> project(const vec<T, size>& u, const vec<T, size>& v){
                float du = dot(u);
                float dv = dot(v);
                return (du / dv) * v;
            }
    };
    
    template<typename T, unsigned long size>
    inline constexpr vec<T, size> operator+(const vec<T, size>& left, const vec<T, size>& right) {
        T initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] + right[i];
        return vec<T, size>{initializer};
    }
    
    template<typename T, unsigned long size>
    inline constexpr vec<T, size> operator-(const vec<T, size>& left, const vec<T, size>& right) {
        T initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] - right[i];
        return vec<T, size>{initializer};
    }
    
    template<typename T, unsigned long size>
    inline constexpr vec<T, size> operator+(const vec<T, size>& left, float f) {
        T initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] + f;
        return vec<T, size>{initializer};
    }
    
    template<typename T, unsigned long size>
    inline constexpr vec<T, size> operator-(const vec<T, size>& left, float f) {
        T initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] + f;
        return vec<T, size>{initializer};
    }
    
    template<typename T, unsigned long size>
    inline constexpr vec<T, size> operator+(float f, const vec<T, size>& right) {
        T initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = f + right[i];
        return vec<T, size>{initializer};
    }
    
    template<typename T, unsigned long size>
    inline constexpr vec<T, size> operator-(float f, const vec<T, size>& right) {
        T initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = f - right[i];
        return vec<T, size>{initializer};
    }
    
    template<typename T, unsigned long size>
    inline constexpr vec<T, size> operator*(const vec<T, size>& left, const vec<T, size>& right) {
        T initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] * right[i];
        return vec<T, size>{initializer};
    }
    
    template<typename T, unsigned long size>
    inline constexpr vec<T, size> operator*(const vec<T, size>& left, float f) {
        T initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] * f;
        return vec<T, size>{initializer};
    }
    
    template<typename T, unsigned long size>
    inline constexpr vec<T, size> operator*(float f, const vec<T, size>& right) {
        T initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = f * right[i];
        return vec<T, size>{initializer};
    }
    
    template<typename T, unsigned long size>
    inline constexpr vec<T, size> operator/(const vec<T, size>& left, float f) {
        T initializer[size];
        for (int i = 0; i < size; i++)
            initializer[i] = left[i] / f;
        return vec<T, size>{initializer};
    }
    
    template<typename T, unsigned long size>
    inline constexpr bool operator==(const vec<T, size>& left, const vec<T, size>& right) {
        for (int i = 0; i < size; i++)
            if (left[i] != right[i])
                return false;
        return true;
    }
    
    template<typename T, unsigned long size>
    inline constexpr bool operator&&(const vec<T, size>& left, const vec<T, size>& right) {
        for (int i = 0; i < size; i++)
            if (!f_equal(left[i], right[i]))
                return false;
        return true;
    }
    
    typedef vec<float, 2> vec2f;
    typedef vec<float, 3> vec3f;
    typedef vec<float, 4> vec4f;
    
    typedef vec<double, 2> vec2d;
    typedef vec<double, 3> vec3d;
    typedef vec<double, 4> vec4d;
    
    typedef vec<int, 2> vec2i;
    typedef vec<int, 3> vec3i;
    typedef vec<int, 4> vec4i;
    
    typedef vec<long long, 2> vec2l;
    typedef vec<long long, 3> vec3l;
    typedef vec<long long, 4> vec4l;
    
    typedef vec<unsigned int, 2> vec2ui;
    typedef vec<unsigned int, 3> vec3ui;
    typedef vec<unsigned int, 4> vec4ui;
    
    typedef vec<unsigned long long, 2> vec2ul;
    typedef vec<unsigned long long, 3> vec3ul;
    typedef vec<unsigned long long, 4> vec4ul;
    
    typedef vec2f vec2;
    typedef vec3f vec3;
    typedef vec4f vec4;
    
    namespace vec_algorithm {
        static inline void findOrthogonalBasis(const vec3& v, vec3& v1, vec3& v2, vec3& v3) {
            v1 = v.normalize();
        
            vec3 arbitraryVector{1, 0, 0};
            if (std::abs(vec3::dot(v, arbitraryVector)) > 0.9) {
                arbitraryVector = vec3{0, 1, 0};
            }
        
            v2 = vec3::cross(v, arbitraryVector).normalize();
            v3 = vec3::cross(v1, v2);
        }
    
        // Gram-Schmidt orthonormalization algorithm
        static inline void gramSchmidt(std::vector<vec3>& vectors) {
            int n = (int)vectors.size();
            std::vector<vec3> basis;
        
            // normalize first vector
            basis.push_back(vectors[0]);
            basis[0] = basis[0].normalize();
        
            // iterate over the rest of the vectors
            for (int i = 1; i < n; ++i) {
                // subtract the projections of the vector onto the previous basis vectors
                vec3 new_vector = vectors[i];
                for (int j = 0; j < i; ++j) {
                    float projection = vec3::dot(vectors[i], basis[j]);
                    new_vector[0] -= projection * basis[j].x();
                    new_vector[1] -= projection * basis[j].y();
                    new_vector[2] -= projection * basis[j].z();
                }
                // normalize the new basis vector
                new_vector = new_vector.normalize();
                basis.push_back(new_vector);
            }
        
            vectors = basis;
        }
    }
}

#endif //BLT_TESTS_VECTORS_H
