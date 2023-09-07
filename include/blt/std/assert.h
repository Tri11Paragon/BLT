/*
 * Created by Brett on 23/08/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_ASSERT_H
#define BLT_ASSERT_H

#ifdef __GNUC__
    
    #include <execinfo.h>
    #include <cstdlib>

#endif

#ifdef __GNUC__
    #define BLT_STACK_TRACE(number) void* ptrs[number]; \
            int size = backtrace(ptrs, number);         \
            char** messages = backtrace_symbols(ptrs, size);
    
    #define BLT_FREE_STACK_TRACE() free(messages);
#else
    #define BLT_STACK_TRACE(number) void();
    #define BLT_FREE_STACK_TRACE() void();
#endif

namespace blt
{
    void printStacktrace(char** messages, int size, const char* path, int line);
    
    void b_assert_failed(const char* expression, const char* path, int line);
    
    void b_throw(const char* what, const char* path, int line);
}

// prints error with stack trace if assertion fails. Does not stop execution.
#define blt_assert(expr) do {static_cast<bool>(expr) ? void(0) : blt::b_assert_failed(#expr, __FILE__, __LINE__) } while (0)
// prints error with stack trace then exits with failure.
#define BLT_ASSERT(expr) do {static_cast<bool>(expr) ? void(0) : blt::b_assert_failed(#expr, __FILE__, __LINE__); std::exit(EXIT_FAILURE); } while (0)
// prints as error but does not throw the exception.
#define blt_throw(throwable) do {blt::b_throw(throwable.what(), __FILE__, __LINE__);} while (0)
// prints as error with stack trace and throws the exception.
#define BLT_THROW(throwable) do {blt::b_throw(throwable.what(), __FILE__, __LINE__); throw throwable;} while(0)


#endif //BLT_ASSERT_H
