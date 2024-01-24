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

//#define BLT_LAMBDA(type, var, code) [](const type& var) -> auto { return code; }
//#define BLT_LAMBDA(var, code) [](var) -> auto { return code; }

/*
 * std::visit(blt::lambda_visitor{
 *      lambdas...
 * }, data_variant);
 */

// TODO: WTF
    template<class... TLambdas>
    struct lambda_visitor : TLambdas... {
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
