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
#include <cstdint>
#include <array>
#include <type_traits>
#include <blt/std/types.h>

namespace blt
{

#define MSVC_COMPILER (!defined(__GNUC__) && !defined(__clang__))
    
    constexpr float EPSILON = 0.0001f;
    
    static inline constexpr bool f_equal(float v1, float v2)
    {
        return v1 >= v2 - EPSILON && v1 <= v2 + EPSILON;
    }
    
    template<typename T, blt::u32 size>
    struct vec
    {
            static_assert(std::is_arithmetic_v<T> && "blt::vec must be created using an arithmetic type!");
        private:
            std::array<T, size> elements;
        public:
            vec()
            {
                for (auto& v : elements)
                    v = static_cast<T>(0);
            }
            
            /**
             * Create a vector with initializer list, if the initializer list doesn't contain enough values to fill this vec, it will use t
             * @param t default value to fill with
             * @param args list of args
             */
            template<typename U, std::enable_if_t<std::is_same_v<T, U> || std::is_convertible_v<U, T>, bool> = true>
            vec(U t, std::initializer_list<U> args)
            {
                auto b = args.begin();
                for (auto& v : elements)
                {
                    if (b == args.end())
                    {
                        v = t;
                        continue;
                    }
                    v = *b;
                    ++b;
                }
            }
            
            /**
             * Create a vector from an initializer list, if the list doesn't have enough elements it will be filled with the default value (0)
             * @param args
             */
            template<typename U, std::enable_if_t<std::is_same_v<T, U> || std::is_convertible_v<U, T>, bool> = true>
            vec(std::initializer_list<U> args): vec(U(), args)
            {}
            
            template<typename... Args>
            explicit vec(Args... args): vec(std::array<T, size>{static_cast<T>(args)...})
            {}
            
            explicit vec(T t)
            {
                for (auto& v : elements)
                    v = t;
            }
            
            explicit vec(const T elem[size])
            {
                for (size_t i = 0; i < size; i++)
                    elements[i] = elem[i];
            }
            
            explicit vec(std::array<T, size> elem)
            {
                auto b = elem.begin();
                for (auto& v : elements)
                {
                    v = *b;
                    ++b;
                }
            }
            
            [[nodiscard]] inline T x() const
            {
                return elements[0];
            }
            
            [[nodiscard]] inline T y() const
            {
                static_assert(size > 1);
                return elements[1];
            }
            
            [[nodiscard]] inline T z() const
            {
                static_assert(size > 2);
                return elements[2];
            }
            
            [[nodiscard]] inline T w() const
            {
                static_assert(size > 3);
                return elements[3];
            }
            
            [[nodiscard]] inline T magnitude() const
            {
                T total = 0;
                for (blt::u32 i = 0; i < size; i++)
                    total += elements[i] * elements[i];
                return std::sqrt(total);
            }
            
            [[nodiscard]] inline vec<T, size> normalize() const
            {
                T mag = this->magnitude();
                if (mag == 0)
                    return vec<T, size>(*this);
                return *this / mag;
            }
            
            inline T& operator[](int index)
            {
                return elements[index];
            }
            
            inline T operator[](int index) const
            {
                return elements[index];
            }
            
            inline vec<T, size>& operator=(T v)
            {
                for (blt::u32 i = 0; i < size; i++)
                    elements[i] = v;
                return *this;
            }
            
            inline vec<T, size> operator-()
            {
                vec<T, size> initializer{};
                for (blt::u32 i = 0; i < size; i++)
                    initializer[i] = -elements[i];
                return vec<T, size>{initializer};
            }
            
            inline vec<T, size>& operator+=(const vec<T, size>& other)
            {
                for (blt::u32 i = 0; i < size; i++)
                    elements[i] += other[i];
                return *this;
            }
            
            inline vec<T, size>& operator*=(const vec<T, size>& other)
            {
                for (blt::u32 i = 0; i < size; i++)
                    elements[i] *= other[i];
                return *this;
            }
            
            inline vec<T, size>& operator+=(T f)
            {
                for (blt::u32 i = 0; i < size; i++)
                    elements[i] += f;
                return *this;
            }
            
            inline vec<T, size>& operator*=(T f)
            {
                for (blt::u32 i = 0; i < size; i++)
                    elements[i] *= f;
                return *this;
            }
            
            inline vec<T, size>& operator-=(const vec<T, size>& other)
            {
                for (blt::u32 i = 0; i < size; i++)
                    elements[i] -= other[i];
                return *this;
            }
            
            inline vec<T, size>& operator-=(T f)
            {
                for (blt::u32 i = 0; i < size; i++)
                    elements[i] -= f;
                return *this;
            }
            
            /**
             * performs the dot product of left * right
             */
            static inline constexpr T dot(const vec<T, size>& left, const vec<T, size>& right)
            {
                T dot = 0;
                for (blt::u32 i = 0; i < size; i++)
                    dot += left[i] * right[i];
                return dot;
            }
            
            static inline constexpr vec<T, size> cross(
                    const vec<T, size>& left, const vec<T, size>& right
                                                      )
            {
                // cross is only defined on vectors of size 3. 2D could be implemented, which is a TODO
                static_assert(size == 3);
                return {left.y() * right.z() - left.z() * right.y(),
                        left.z() * right.x() - left.x() * right.z(),
                        left.x() * right.y() - left.y() * right.x()};
            }
            
            static inline constexpr vec<T, size> project(
                    const vec<T, size>& u, const vec<T, size>& v
                                                        )
            {
                T du = dot(u);
                T dv = dot(v);
                return (du / dv) * v;
            }
            
            inline auto* data()
            {
                return elements;
            }
            
            [[nodiscard]] inline const auto* data() const
            {
                return elements;
            }
            
            auto begin()
            {
                return elements.begin();
            }
            
            auto end()
            {
                return elements.end();
            }
            
            auto rbegin()
            {
                return elements.rbegin();
            }
            
            auto rend()
            {
                return elements.rend();
            }
            
            [[nodiscard]] auto cbegin() const
            {
                return elements.cbegin();
            }
            
            [[nodiscard]] auto cend() const
            {
                return elements.cend();
            }
    };
    
    template<typename T, blt::u32 size>
    inline constexpr vec<T, size> operator+(const vec<T, size>& left, const vec<T, size>& right)
    {
        vec<T, size> initializer{};
        for (blt::u32 i = 0; i < size; i++)
            initializer[i] = left[i] + right[i];
        return initializer;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr vec<T, size> operator-(const vec<T, size>& left, const vec<T, size>& right)
    {
        vec<T, size> initializer{};
        for (blt::u32 i = 0; i < size; i++)
            initializer[i] = left[i] - right[i];
        return initializer;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr vec<T, size> operator+(const vec<T, size>& left, T f)
    {
        vec<T, size> initializer{};
        for (blt::u32 i = 0; i < size; i++)
            initializer[i] = left[i] + f;
        return initializer;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr vec<T, size> operator-(const vec<T, size>& left, T f)
    {
        vec<T, size> initializer{};
        for (blt::u32 i = 0; i < size; i++)
            initializer[i] = left[i] + f;
        return initializer;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr vec<T, size> operator+(T f, const vec<T, size>& right)
    {
        vec<T, size> initializer{};
        for (blt::u32 i = 0; i < size; i++)
            initializer[i] = f + right[i];
        return initializer;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr vec<T, size> operator-(T f, const vec<T, size>& right)
    {
        vec<T, size> initializer{};
        for (blt::u32 i = 0; i < size; i++)
            initializer[i] = f - right[i];
        return initializer;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr vec<T, size> operator*(const vec<T, size>& left, const vec<T, size>& right)
    {
        vec<T, size> initializer{};
        for (blt::u32 i = 0; i < size; i++)
            initializer[i] = left[i] * right[i];
        return initializer;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr vec<T, size> operator*(const vec<T, size>& left, T f)
    {
        vec<T, size> initializer{};
        for (blt::u32 i = 0; i < size; i++)
            initializer[i] = left[i] * f;
        return initializer;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr vec<T, size> operator*(T f, const vec<T, size>& right)
    {
        vec<T, size> initializer{};
        for (blt::u32 i = 0; i < size; i++)
            initializer[i] = f * right[i];
        return initializer;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr vec<T, size> operator/(const vec<T, size>& left, T f)
    {
        vec<T, size> initializer{};
        for (blt::u32 i = 0; i < size; i++)
            initializer[i] = left[i] / f;
        return initializer;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr bool operator==(const vec<T, size>& left, const vec<T, size>& right)
    {
        for (blt::u32 i = 0; i < size; i++)
            if (left[i] != right[i])
                return false;
        return true;
    }
    
    template<typename T, blt::u32 size>
    inline constexpr bool operator!=(const vec<T, size>& left, const vec<T, size>& right)
    {
        return !(left == right);
    }
    
    template<typename T, blt::u32 size>
    inline constexpr bool operator&&(const vec<T, size>& left, const vec<T, size>& right)
    {
        for (blt::u32 i = 0; i < size; i++)
            if (!f_equal(left[i], right[i]))
                return false;
        return true;
    }
    
    using vec2f = vec<float, 2>;
    using vec3f = vec<float, 3>;
    using vec4f = vec<float, 4>;
    
    using vec2d = vec<double, 2>;
    using vec3d = vec<double, 3>;
    using vec4d = vec<double, 4>;
    
    using vec2i = vec<blt::i32, 2>;
    using vec3i = vec<blt::i32, 3>;
    using vec4i = vec<blt::i32, 4>;
    
    using vec2l = vec<blt::i64, 2>;
    using vec3l = vec<blt::i64, 3>;
    using vec4l = vec<blt::i64, 4>;
    
    using vec2ui = vec<blt::u32, 2>;
    using vec3ui = vec<blt::u32, 3>;
    using vec4ui = vec<blt::u32, 4>;
    
    using vec2ul = vec<blt::u64, 2>;
    using vec3ul = vec<blt::u64, 3>;
    using vec4ul = vec<blt::u64, 4>;
    
    using vec2 = vec2f;
    using vec3 = vec3f;
    using vec4 = vec4f;
    
    namespace vec_algorithm
    {
        static inline void findOrthogonalBasis(const vec3& v, vec3& v1, vec3& v2, vec3& v3)
        {
            v1 = v.normalize();
            
            vec3 arbitraryVector{1, 0, 0};
            if (std::abs(vec3::dot(v, arbitraryVector)) > 0.9)
            {
                arbitraryVector = vec3{0, 1, 0};
            }
            
            v2 = vec3::cross(v, arbitraryVector).normalize();
            v3 = vec3::cross(v1, v2);
        }
        
        // Gram-Schmidt orthonormalization algorithm
        static inline void gramSchmidt(std::vector<vec3>& vectors)
        {
            int n = (int) vectors.size();
            std::vector<vec3> basis;
            
            // normalize first vector
            basis.push_back(vectors[0]);
            basis[0] = basis[0].normalize();
            
            // iterate over the rest of the vectors
            for (int i = 1; i < n; ++i)
            {
                // subtract the projections of the vector onto the previous basis vectors
                vec3 new_vector = vectors[i];
                for (int j = 0; j < i; ++j)
                {
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
