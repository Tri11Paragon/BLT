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

#include <optional>
#include <blt/std/string.h>

#if defined(__GNUC__)
    
    #include <cxxabi.h>
    #include <blt/compatibility.h>
    #include <string>

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
    void BLT_ATTRIB_NO_INLINE black_box(const T& val)
    {
        static volatile void* hell;
        hell = (void*) &val;
        (void) hell;
    }
    
    template<typename T>
    const T& BLT_ATTRIB_NO_INLINE black_box_ret(const T& val)
    {
        static volatile void* hell;
        hell = (void*) &val;
        (void) hell;
        return val;
    }
    
}

#endif //BLT_UTILITY_H
