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

#include <string>
#include <blt/compatibility.h>
#include <optional>

#if defined(__GNUC__)
    
    #include <cxxabi.h>

namespace blt
{
    static BLT_CPP20_CONSTEXPR inline std::string demangle(const std::string& str)
    {
        int status;
        // only defined for GNU C++11?
        char* demangled_name = abi::__cxa_demangle(str.c_str(), nullptr, nullptr, &status);
        if (demangled_name == nullptr)
            return str;
        std::string ret_name = demangled_name;
        std::free(demangled_name);
        return ret_name;
    }
}
#else
namespace blt
{
    static BLT_CPP20_CONSTEXPR inline std::string demangle(const std::string& str)
    {
        return str;
    }
}
#endif

namespace blt
{
    template<typename T>
    static BLT_CPP20_CONSTEXPR inline std::string type_string()
    {
        return demangle(typeid(T).name());
    }
    
    template<typename TYPE_ITR>
    class enumerator
    {
        public:
            class iterator
            {
                public:
                    using iterator_category = std::input_iterator_tag;
                    using value_type = typename TYPE_ITR::value_type;
                    using difference_type = typename TYPE_ITR::difference_type;
                    using pointer = typename TYPE_ITR::pointer;
                    using reference = typename TYPE_ITR::reference;
                private:
                    size_t index = 0;
                    TYPE_ITR current;
                public:
                    explicit iterator(TYPE_ITR current): current(std::move(current))
                    {}
                    
                    iterator& operator++()
                    {
                        ++index;
                        ++current;
                        return *this;
                    }
                    
                    bool operator==(iterator other) const
                    {
                        return current == other.current;
                    }
                    
                    bool operator!=(iterator other) const
                    {
                        return current != other.current;
                    }
                    
                    std::pair<size_t, const reference> operator*() const
                    {
                        return {index, *current};
                    };
                    
                    std::pair<size_t, reference> operator*()
                    {
                        return {index, *current};
                    };
            };
            
            explicit enumerator(TYPE_ITR begin, TYPE_ITR end): begin_(std::move(begin)), end_(std::move(end))
            {}
            
            iterator begin()
            {
                return begin_;
            }
            
            iterator end()
            {
                return end_;
            }
        
        private:
            iterator begin_;
            iterator end_;
    };
    
    template<typename T>
    static inline auto enumerate(const T& container)
    {
        return enumerator{container.begin(), container.end()};
    }
    
    template<typename T>
    static inline auto enumerate(T& container)
    {
        return enumerator{container.begin(), container.end()};
    }
    
    template<typename T>
    struct range
    {
        public:
            struct range_itr
            {
                public:
                    using iterator_category = std::bidirectional_iterator_tag;
                    using difference_type = T;
                    using value_type = T;
                    using pointer = T*;
                    using reference = T&;
                private:
                    T current;
                    bool forward;
                public:
                    
                    explicit range_itr(T current, bool forward): current(current), forward(forward)
                    {}
                    
                    value_type operator*() const
                    { return current; }
                    
                    value_type operator->()
                    { return current; }
                    
                    range_itr& operator++()
                    {
                        if (forward)
                            current++;
                        else
                            current--;
                        return *this;
                    }
                    
                    range_itr& operator--()
                    {
                        if (forward)
                            current--;
                        else
                            current++;
                        return *this;
                    }
                    
                    friend bool operator==(const range_itr& a, const range_itr& b)
                    {
                        return a.current == b.current;
                    }
                    
                    friend bool operator!=(const range_itr& a, const range_itr& b)
                    {
                        return a.current != b.current;
                    }
            };
        
        private:
            T _begin;
            T _end;
            T offset = 0;
        public:
            range(T begin, T end): _begin(begin), _end(end), offset(end < begin ? 1 : 0)
            {}
            
            range_itr begin()
            {
                return range_itr(_begin - offset, offset == 0);
            }
            
            range_itr end()
            {
                // not sure if i like this
                return range_itr(_end - offset, offset == 0);
            }
    };
    
    template<typename T, typename E>
    class expected
    {
        private:
            std::optional<T> t;
            std::optional<E> e;
        public:
            constexpr expected() noexcept: t(T())
            {}
            
            template<typename U, std::enable_if_t<std::is_convertible_v<U, T>, bool> = true>
            constexpr explicit expected(U&& t): t(std::forward<U>(t))
            {}
            
            template<typename U, std::enable_if_t<std::is_convertible_v<U, E>, bool> = true>
            constexpr explicit expected(U&& e): e(std::forward<U>(e))
            {}
            
//            template<typename U, std::enable_if_t<std::is_convertible_v<U, T>, bool> = true>
//            constexpr expected(std::initializer_list<U> t): t(std::move(*t.begin()))
//            {}
//
//            template<typename U, std::enable_if_t<std::is_convertible_v<U, E>, bool> = true>
//            constexpr expected(std::initializer_list<U> e): e(std::move(*e.begin()))
//            {}
            
            template<class U, class G, std::enable_if_t<std::is_convertible_v<U, T> && std::is_convertible_v<G, E>, bool> = true>
            constexpr explicit expected(const expected<U, G>& other)
            {
                if (other.has_value())
                    t = other.value();
                else
                    e = other.error();
            }
            
            template<class U, class G, std::enable_if_t<std::is_convertible_v<U, T> && std::is_convertible_v<G, E>, bool> = true>
            constexpr explicit expected(expected<U, G>&& other)
            {
                if (other.has_value())
                    t = other.value();
                else
                    e = other.error();
            }
            
            constexpr expected(const T& t): t(t)
            {}
            
            constexpr expected(T&& t): t(std::move(t))
            {}
            
            constexpr expected(const E& e): e(e)
            {}
            
            constexpr expected(E&& e): e(std::move(e))
            {}
            
            constexpr expected(const expected& copy) = default;
            
            constexpr expected(expected&& move) = default;
            
            expected& operator=(const expected& copy) = default;
            
            expected& operator=(expected&& move) = default;
            
            [[nodiscard]] constexpr explicit operator bool() const noexcept
            {
                return t.has_value();
            }
            
            [[nodiscard]] constexpr inline bool has_value() const noexcept
            {
                return t.has_value();
            }
            
            constexpr T& value()&
            {
                return t.value();
            }
            
            constexpr const T& value() const&
            {
                return t.value();
            }
            
            constexpr T&& value()&&
            {
                return t.value();
            }
            
            constexpr const T&& value() const&&
            {
                return t.value();
            }
            
            constexpr const E& error() const& noexcept
            {
                return e.value();
            }
            
            constexpr E& error()& noexcept
            {
                return e.value();
            }
            
            constexpr const E&& error() const&& noexcept
            {
                return e.value();
            }
            
            constexpr E&& error()&& noexcept
            {
                return e.value();
            }
            
            template<class U, std::enable_if_t<std::is_convertible_v<U, T> && std::is_copy_constructible_v<T>, bool> = true>
            constexpr T value_or(U&& default_value) const&
            {
                return bool(*this) ? **this : static_cast<T>(std::forward<U>(default_value));
            }
            
            template<class U, std::enable_if_t<std::is_convertible_v<U, T> && std::is_move_constructible_v<T>, bool> = true>
            constexpr T value_or(U&& default_value)&&
            {
                return bool(*this) ? std::move(**this) : static_cast<T>(std::forward<U>(default_value));
            }
            
            constexpr inline const T* operator->() const noexcept
            {
                return &t.value();
            }
            
            constexpr inline T* operator->() noexcept
            {
                return &t.value();
            }
            
            constexpr inline const T& operator*() const& noexcept
            {
                return t.value();
            }
            
            constexpr inline T& operator*()& noexcept
            {
                return t.value();
            }
            
            constexpr inline const T&& operator*() const&& noexcept
            {
                return t.value();
            }
            
            constexpr inline T&& operator*()&& noexcept
            {
                return std::move(t.value());
            }
    };

//#define BLT_LAMBDA(type, var, code) [](const type& var) -> auto { return code; }
//#define BLT_LAMBDA(var, code) [](var) -> auto { return code; }

/*
 * std::visit(blt::lambda_visitor{
 *      lambdas...
 * }, data_variant);
 */

// TODO: WTF
    template<class... TLambdas>
    struct lambda_visitor : TLambdas ...
    {
        using TLambdas::operator()...;
    };

#if __cplusplus < 202002L
    
    // explicit deduction guide (not needed as of C++20)
    template<class... TLambdas>
    lambda_visitor(TLambdas...) -> lambda_visitor<TLambdas...>;

#endif


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
    BLT_ATTRIB_NO_INLINE void black_box(const T& val)
    {
        static volatile void* hell;
        hell = (void*) &val;
        (void) hell;
    }
    
    template<typename T>
    BLT_ATTRIB_NO_INLINE const T& black_box_ret(const T& val)
    {
        static volatile void* hell;
        hell = (void*) &val;
        (void) hell;
        return val;
    }
    
}

#endif //BLT_UTILITY_H
