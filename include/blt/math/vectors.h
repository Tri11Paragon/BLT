/*
 * Created by Brett on 28/02/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_VECTORS_H
#define BLT_TESTS_VECTORS_H

#include <initializer_list>
#include <cmath>

template<typename T, unsigned long size>
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
        
        [[nodiscard]] inline T x() const {return elements[0];}
        [[nodiscard]] inline T y() const {return elements[1];}
        [[nodiscard]] inline T z() const {return elements[2];}
        [[nodiscard]] inline T w() const {return elements[3];}
        
        inline T& operator[](int index) {
            return elements[index];
        }
        
        inline T operator[](int index) const {
            return elements[index];
        }
        
        inline vec<T, size>& operator=(T f) {
            for (int i = 0; i < size; i++)
                elements[i] = f;
            return *this;
        }
        
        inline vec<T, size>& operator=(int i) {
            for (int _ = 0; _ < size; _++)
                elements[_] = i;
            return *this;
        }
        
        inline vec<T, size> operator-() {
            T negativeCopy[size];
            for (int i = 0; i < size; i++)
                negativeCopy[i] = -elements[i];
            return vec<size>{negativeCopy};
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
        static inline T dot(const vec<T, size>& left, const vec<T, size>& right) {
            T dot = 0;
            for (int i = 0; i < size; i++)
                dot += left[i] * right[i];
            return dot;
        }
};

template<unsigned long size>
inline vec<size> operator+(const vec<size>& left, const vec<size>& right) {
    float initializer[size];
    for (int i = 0; i < size; i++)
        initializer[i] = left[i] + right[i];
    return vec<size>{initializer};
}

template<typename T, unsigned long size>
inline vec<T, size> operator-(const vec<T, size>& left, const vec<T, size>& right) {
    float initializer[size];
    for (int i = 0; i < size; i++)
        initializer[i] = left[i] - right[i];
    return vec<T, size>{initializer};
}

template<typename T, unsigned long size>
inline vec<T, size> operator+(const vec<T, size>& left, float f) {
    float initializer[size];
    for (int i = 0; i < size; i++)
        initializer[i] = left[i] + f;
    return vec<T, size>{initializer};
}

template<typename T, unsigned long size>
inline vec<T, size> operator-(const vec<T, size>& left, float f) {
    float initializer[size];
    for (int i = 0; i < size; i++)
        initializer[i] = left[i] + f;
    return vec<T, size>{initializer};
}

template<typename T, unsigned long size>
inline vec<T, size> operator+(float f, const vec<T, size>& right) {
    float initializer[size];
    for (int i = 0; i < size; i++)
        initializer[i] = f + right[i];
    return vec<T, size>{initializer};
}

template<typename T, unsigned long size>
inline vec<T, size> operator-(float f, const vec<T, size>& right) {
    float initializer[size];
    for (int i = 0; i < size; i++)
        initializer[i] = f - right[i];
    return vec<T, size>{initializer};
}

template<typename T, unsigned long size>
inline vec<T, size> operator*(const vec<T, size>& left, const vec<T, size>& right) {
    float initializer[size];
    for (int i = 0; i < size; i++)
        initializer[i] = left[i] * right[i];
    return vec<T, size>{initializer};
}

template<typename T, unsigned long size>
inline vec<T, size> operator*(const vec<T, size>& left, float f) {
    float initializer[size];
    for (int i = 0; i < size; i++)
        initializer[i] = left[i] * f;
    return vec<T, size>{initializer};
}

template<typename T, unsigned long size>
inline vec<T, size> operator*(float f, const vec<T, size>& right) {
    float initializer[size];
    for (int i = 0; i < size; i++)
        initializer[i] = f * right[i];
    return vec<T, size>{initializer};
}

template<typename T, unsigned long size>
inline vec<T, size> operator/(const vec<T, size>& left, float f) {
    float initializer[size];
    for (int i = 0; i < size; i++)
        initializer[i] = left[i] / f;
    return vec<T, size>{initializer};
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

#endif //BLT_TESTS_VECTORS_H
