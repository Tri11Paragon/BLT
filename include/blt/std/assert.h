/*
 * Created by Brett on 23/08/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_ASSERT_H
#define BLT_ASSERT_H

namespace blt
{
    void printStacktrace(char** messages, int size, const char* path, int line);
    
    void b_assert_failed(const char* expression, const char* path, int line);
    
    void b_throw(const char* what, const char* path, int line);

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
    void BLT_ATTRIB_NO_INLINE black_box_ref(const T& val){
        volatile void* hell;
        hell = (void*)&val;
    }
    
    template<typename T>
    void BLT_ATTRIB_NO_INLINE black_box(T val){
        volatile void* hell2;
        hell2 = (void*)&val;
    }
    
    template<typename T>
    const T& BLT_ATTRIB_NO_INLINE black_box_ref_ret(const T& val){
        volatile void* hell;
        hell = (void*)&val;
        return val;
    }
    
    template<typename T>
    T BLT_ATTRIB_NO_INLINE black_box_ret(T val){
        volatile void* hell2;
        hell2 = (void*)&val;
        return val;
    }
}

// prints error with stack trace if assertion fails. Does not stop execution.
#define blt_assert(expr) do {static_cast<bool>(expr) ? void(0) : blt::b_assert_failed(#expr, __FILE__, __LINE__) } while (0)
// prints error with stack trace then exits with failure.
#define BLT_ASSERT(expr) do {                                   \
        if (!static_cast<bool>(expr)) {                         \
            blt::b_assert_failed(#expr, __FILE__, __LINE__);    \
            std::exit(EXIT_FAILURE);                            \
        }                                                       \
    } while (0)
// prints as error but does not throw the exception.
#define blt_throw(throwable) do {blt::b_throw(throwable.what(), __FILE__, __LINE__);} while (0)
// prints as error with stack trace and throws the exception.
#define BLT_THROW(throwable) do {blt::b_throw(throwable.what(), __FILE__, __LINE__); throw throwable;} while(0)


#endif //BLT_ASSERT_H
