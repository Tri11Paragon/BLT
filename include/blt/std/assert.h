/*
 * Created by Brett on 23/08/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_ASSERT_H
#define BLT_ASSERT_H

#include <cstdlib>

namespace blt
{
    void printStacktrace(char** messages, int size, const char* path, int line);
    
    void b_assert_failed(const char* expression, const char* msg, const char* path, int line);
    
    void b_throw(const char* what, const char* path, int line);
    
    void b_abort(const char* what, const char* path, int line);
}

/**
 * Prints error with stack trace if assertion fails.  Does not stop execution.
 */
#define blt_assert(expr) do {static_cast<bool>(expr) ? void(0) : blt::b_assert_failed(#expr, nullptr, __FILE__, __LINE__); } while (0)
/**
* Prints error with stack trace if assertion fails. Will print fail_message after
* the assertion expression but before the stack trace. Does not stop execution.
*/
#define blt_assert_msg(expr, fail_message) do {static_cast<bool>(expr) ? void(0) : blt::b_assert_failed(#expr, fail_message, __FILE__, __LINE__) } while (0)
/**
 * Prints error with stack trace then exits with failure.
 */
#define BLT_ASSERT(expr) do {                                                   \
        if (!static_cast<bool>(expr)) {                                         \
            blt::b_assert_failed(#expr, nullptr, __FILE__, __LINE__);           \
            std::exit(EXIT_FAILURE);                                            \
        }                                                                       \
    } while (0)

/**
 *  Prints the error with stack trace if the assertion fails and stops execution with EXIT_FAILURE. Will print fail_message after
 *  the assertion expression but before the stack trace.
 */
#define BLT_ASSERT_MSG(expr, fail_message) do {                                 \
        if (!static_cast<bool>(expr)) {                                         \
            blt::b_assert_failed(#expr, fail_message, __FILE__, __LINE__);      \
            std::exit(EXIT_FAILURE);                                            \
        }                                                                       \
    } while (0)

// prints as error but does not throw the exception.
#define blt_throw(throwable) do {blt::b_throw(throwable.what(), __FILE__, __LINE__);} while (0)
// prints as error with stack trace and throws the exception.
#define BLT_THROW(throwable) do {blt::b_throw(throwable.what(), __FILE__, __LINE__); throw throwable;} while(0)


#define BLT_ABORT(message) do {blt::b_abort(message, __FILE__, __LINE__); std::exit(1); } while (0)

#endif //BLT_ASSERT_H
