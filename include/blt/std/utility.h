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

#include <blt/compatibility.h>
#include <blt/std/ranges.h>
#include <blt/std/expected.h>
#include <string>
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
    
    template<typename T>
    static BLT_CPP20_CONSTEXPR inline std::string type_string_raw()
    {
        return typeid(T).name();
    }

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
    /**
     * means that the return value is solely a function of the arguments,
     * and if any of the arguments are pointers, then the pointers must not be dereferenced.
     */
    #define BLT_ATTRIB_NO_SIDE_EFFECTS __attribute__((const))
    /**
     * the function has no side effects and the value returned depends on the arguments and the state of global variables.
     * Therefore it is safe for the optimizer to elide some calls to it, if the arguments are the same,
     * and the caller did not do anything to change the state of the globals in between the calls.
     */
    #define BLT_ATTRIB_GLOBAL_READ_ONLY __attribute__((pure))
#else
    #if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
        #define BLT_ATTRIB_NO_INLINE __declspec(noinline)
    #else
        #define BLT_ATTRIB_NO_INLINE
    #endif
    #define BLT_ATTRIB_CONST
    #define BLT_ATTRIB_PURE
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
