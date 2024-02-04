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
#include <variant>
#include <utility>

#if defined(__GNUC__)
    
    #include <cxxabi.h>

namespace blt
{
    static inline std::string demangle(const std::string& str)
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
    static inline std::string demangle(const std::string& str)
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
    
    struct unexpect_t
    {
        explicit unexpect_t() = default;
    };
    
    inline constexpr unexpect_t unexpect{};
    
    template<typename T>
    using remove_cvref_t = std::remove_reference_t<std::remove_cv_t<T>>;
    
    template<class E>
    class unexpected
    {
        private:
            E e;
        public:
            constexpr unexpected(const unexpected&) = default;
            
            constexpr unexpected(unexpected&&) = default;
            
            template<class Err = E, std::enable_if_t<
                    !std::is_same_v<remove_cvref_t<Err>, unexpected> && !std::is_same_v<remove_cvref_t<Err>, std::in_place_t> &&
                    std::is_constructible_v<E, Err>, bool> = true>
            constexpr explicit unexpected(Err&& e): e(std::forward<Err>(e))
            {}
            
            template<class... Args, std::enable_if_t<std::is_constructible_v<E, Args...>, bool> = true>
            constexpr explicit unexpected(std::in_place_t, Args&& ... args): e(std::forward<Args>(args)...)
            {}
            
            template<class U, class... Args, std::enable_if_t<std::is_constructible_v<E, std::initializer_list<U>&, Args...>, bool> = true>
            constexpr explicit unexpected(std::in_place_t, std::initializer_list<U> il, Args&& ... args): e(il, std::forward<Args>(args)...)
            {}
            
            constexpr const E& error() const& noexcept
            {
                return e;
            }
            
            constexpr E& error()& noexcept
            {
                return e;
            }
            
            constexpr const E&& error() const&& noexcept
            {
                return e;
            }
            
            constexpr E&& error()&& noexcept
            {
                return e;
            }
            
            constexpr void swap(unexpected& other) noexcept(std::is_nothrow_swappable_v<E>)
            {
                std::swap(error(), other.error());
            }
            
            template<typename E2>
            inline friend constexpr bool operator==(const unexpected& x, const unexpected <E2>& y)
            {
                return x.error() == y.error();
            }
            
            friend constexpr void swap(unexpected& x, unexpected& y) noexcept(noexcept(x.swap(y)))
            {}
    };
    
    template<class E>
    unexpected(E) -> unexpected<E>;
    
    template<class E>
    class bad_expected_access : public std::exception
    {
        private:
            E e;
        public:
            explicit bad_expected_access(E e): e(std::move(e))
            {}
            
            const E& error() const& noexcept
            { return e; }
            
            E& error()& noexcept
            { return e; }
            
            const E&& error() const&& noexcept
            { return e; }
            
            E&& error()&& noexcept
            { return e; }
            
            [[nodiscard]] const char* what() const noexcept override
            { return "blt::expected does not contain a value!"; }
        
    };
    
    template<typename T, typename E, bool = std::is_copy_constructible_v<T>>
    class expected
    {
        protected:
            std::variant<T, E> v;
            
            template<typename U, typename G>
            inline static constexpr bool eight_insanity_v =
                    std::is_constructible_v<T, expected<U, G>&> || std::is_constructible_v<T, expected<U, G>> ||
                    std::is_constructible_v<T, const expected<U, G>&> || std::is_constructible_v<T, const expected<U, G>> ||
                    std::is_convertible_v<expected<U, G>&, T> || std::is_convertible_v<expected<U, G>, T> ||
                    std::is_convertible_v<const expected<U, G>&, T> || std::is_convertible_v<const expected<U, G>, T>;
            
            template<typename U, typename G>
            inline static constexpr bool four_insanity_v =
                    std::is_constructible_v<unexpected<E>, expected<U, G>&> || std::is_constructible_v<unexpected<E>, expected<U, G>> ||
                    std::is_constructible_v<unexpected<E>, const expected<U, G>&> || std::is_constructible_v<unexpected<E>, const expected<U, G>>;
        
        public:
            template<typename std::enable_if_t<std::is_default_constructible_v<T>, bool> = true>
            constexpr expected() noexcept: v(T())
            {}
            
            constexpr expected(const expected& copy) = delete;
            
            constexpr expected(expected&& move) noexcept: v(move ? std::move(*move) : std::move(move.error()))
            {}
            
            /*
             * (4)...(5)
             */
            template<class U, class G, class UF = std::add_lvalue_reference_t<const U>, class GF = const G&, std::enable_if_t<
                    (!std::is_convertible_v<UF, T> || !std::is_convertible_v<GF, E>) && (std::is_constructible_v<T, UF> || std::is_void_v<U>) &&
                    std::is_constructible_v<E, GF> && !eight_insanity_v < U, G>&& !four_insanity_v<U, G>, bool> = true>
            
            constexpr explicit expected(const expected<U, G>& other):
                    v(other.has_value() ? std::forward<UF>(*other) : std::forward<GF>(other.error()))
            {}
            
            template<class U, class G, class UF = U, class GF = G, std::enable_if_t<
                    (!std::is_convertible_v<UF, T> || !std::is_convertible_v<GF, E>) && (std::is_constructible_v<T, UF> || std::is_void_v<U>) &&
                    std::is_constructible_v<E, GF> && !eight_insanity_v < U, G>&& !four_insanity_v<U, G>, bool> = true>
            
            constexpr explicit expected(expected<U, G>&& other):
                    v(other.has_value() ? std::forward<UF>(*other) : std::forward<GF>(other.error()))
            {}
            
            template<class U, class G, class UF = std::add_lvalue_reference_t<const U>, class GF = const G&, std::enable_if_t<
                    (std::is_convertible_v<UF, T> && std::is_convertible_v<GF, E>) && (std::is_constructible_v<T, UF> || std::is_void_v<U>) &&
                    std::is_constructible_v<E, GF> && !eight_insanity_v < U, G>&& !four_insanity_v<U, G>, bool> = true>
            
            constexpr expected(const expected<U, G>& other):
                    v(other.has_value() ? std::forward<UF>(*other) : std::forward<GF>(other.error()))
            {}
            
            template<class U, class G, class UF = U, class GF = G, std::enable_if_t<
                    (std::is_convertible_v<UF, T> && std::is_convertible_v<GF, E>) && (std::is_constructible_v<T, UF> || std::is_void_v<U>) &&
                    std::is_constructible_v<E, GF> && !eight_insanity_v < U, G>&& !four_insanity_v<U, G>, bool> = true>
            
            constexpr expected(expected<U, G>&& other):
                    v(other.has_value() ? std::forward<UF>(*other) : std::forward<GF>(other.error()))
            {}
            
            
            /*
             * (6)
             */
            
            template<class U = T, std::enable_if_t<!std::is_convertible_v<U, T> &&
                                                   !std::is_same_v<remove_cvref_t<T>, void> &&
                                                   !std::is_same_v<remove_cvref_t<U>, std::in_place_t> &&
                                                   !std::is_same_v<expected, remove_cvref_t<U>> &&
                                                   std::is_constructible_v<T, U> &&
                                                   !std::is_same_v<remove_cvref_t<U>, unexpected<U>> &&
                                                   !std::is_same_v<remove_cvref_t<U>, expected<T, E>>, bool> = true>
            constexpr explicit expected(U&& v): v(T(std::forward<U>(v)))
            {}
            
            template<class U = T, std::enable_if_t<std::is_convertible_v<U, T> &&
                                                   !std::is_same_v<remove_cvref_t<T>, void> &&
                                                   !std::is_same_v<remove_cvref_t<U>, std::in_place_t> &&
                                                   !std::is_same_v<expected, remove_cvref_t<U>> &&
                                                   std::is_constructible_v<T, U> &&
                                                   !std::is_same_v<remove_cvref_t<U>, unexpected<U>> &&
                                                   !std::is_same_v<remove_cvref_t<U>, expected<T, E>>, bool> = true>
            constexpr expected(U&& v): v(T(std::forward<U>(v)))
            {}
            
            /*
             * (7)
             */
            
            template<class G, class GF = std::add_const_t<std::add_lvalue_reference_t<G>>, std::enable_if_t<
                    !std::is_convertible_v<const G&, E> && std::is_constructible_v<E, GF>, bool> = true>
            constexpr explicit expected(const unexpected<G>& e): v(std::forward<GF>(e.error()))
            {}
            
            template<class G, class GF = std::add_const_t<std::add_lvalue_reference_t<G>>, std::enable_if_t<
                    std::is_convertible_v<const G&, E> && std::is_constructible_v<E, GF>, bool> = true>
            constexpr expected(const unexpected<G>& e): v(std::forward<GF>(e.error()))
            {}
            
            /*
             * (8)
             */
            
            template<class G, class GF = std::add_const_t<std::add_lvalue_reference_t<G>>, std::enable_if_t<
                    !std::is_convertible_v<G, E> && std::is_constructible_v<E, GF>, bool> = true>
            constexpr explicit expected(unexpected<G>&& e): v(std::forward<GF>(e.error()))
            {}
            
            template<class G, class GF = std::add_const_t<std::add_lvalue_reference_t<G>>, std::enable_if_t<
                    std::is_convertible_v<G, E> && std::is_constructible_v<E, GF>, bool> = true>
            constexpr expected(unexpected<G>&& e): v(std::forward<GF>(e.error()))
            {}
            
            /*
             * (9)...(13)
             */
            template<class... Args, std::enable_if_t<std::is_constructible_v<T, Args...>, bool> = true>
            constexpr explicit expected(std::in_place_t, Args&& ... args): v(T(std::forward<Args>(args)...))
            {}
            
            template<class U, class... Args, std::enable_if_t<std::is_constructible_v<T, std::initializer_list<U>&, Args...>, bool> = true>
            constexpr explicit expected(std::in_place_t, std::initializer_list<U> il, Args&& ... args): v(T(il, std::forward<Args>(args)...))
            {}
            
//            template<class... Args, std::enable_if_t<std::is_same_v<remove_cvref_t<T>, void>, bool> = true>
//            constexpr explicit expected(std::in_place_t) noexcept: v(T())
//            {}
            
            template<class... Args, std::enable_if_t<std::is_constructible_v<E, Args...>, bool> = true>
            constexpr explicit expected(unexpect_t, Args&& ... args): v(E(std::forward<Args>(args)...))
            {}
            
            template<class U, class... Args, std::enable_if_t<std::is_constructible_v<E, std::initializer_list<U>&, Args...>, bool> = true>
            constexpr explicit expected(unexpect_t, std::initializer_list<U> il, Args&& ... args): v(E(il, std::forward<Args>(args)...))
            {}
            
            expected& operator=(const expected& copy) = delete;
            
            expected& operator=(expected&& move) = default;
            
            [[nodiscard]] constexpr explicit operator bool() const noexcept
            {
                return std::holds_alternative<T>(v);
            }
            
            [[nodiscard]] constexpr inline bool has_value() const noexcept
            {
                return std::holds_alternative<T>(v);
            }
            
            constexpr T& value()&
            {
                if (*this)
                    return std::get<T>(v);
                else
                    throw bad_expected_access(std::as_const(error()));
            }
            
            constexpr const T& value() const&
            {
                if (*this)
                    return std::get<T>(v);
                else
                    throw bad_expected_access(std::as_const(error()));
            }
            
            constexpr T&& value()&&
            {
                if (*this)
                    return std::get<T>(v);
                else
                    throw bad_expected_access(std::move(error()));
            }
            
            constexpr const T&& value() const&&
            {
                if (*this)
                    return std::get<T>(v);
                else
                    throw bad_expected_access(std::move(error()));
            }
            
            constexpr const E& error() const& noexcept
            {
                return std::get<E>(v);
            }
            
            constexpr E& error()& noexcept
            {
                return std::get<E>(v);
            }
            
            constexpr const E&& error() const&& noexcept
            {
                return std::get<E>(v);
            }
            
            constexpr E&& error()&& noexcept
            {
                return std::get<E>(v);
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
                return &std::get<T>(v);
            }
            
            constexpr inline T* operator->() noexcept
            {
                return &std::get<T>(v);
            }
            
            constexpr inline const T& operator*() const& noexcept
            {
                return std::get<T>(v);
            }
            
            constexpr inline T& operator*()& noexcept
            {
                return std::get<T>(v);
            }
            
            constexpr inline const T&& operator*() const&& noexcept
            {
                return std::move(std::get<T>(v));
            }
            
            constexpr inline T&& operator*()&& noexcept
            {
                return std::move(std::get<T>(v));
            }
    };
    
    template<typename T, typename E>
    class expected<T, E, true> : expected<T, E, false>
    {
        public:
            using expected<T, E, false>::expected;
            
            constexpr expected(const expected& copy): expected<T, E, false>::v(copy ? *copy : copy.error())
            {}
            
            expected& operator=(const expected& copy) = default;
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
