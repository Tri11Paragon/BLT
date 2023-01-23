/*
 * Created by Brett on 23/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/std/logging.h>
#include <cstdarg>

// https://en.cppreference.com/w/cpp/utility/variadic
// https://medium.com/swlh/variadic-functions-3419c287a0d2
// https://publications.gbdirect.co.uk//c_book/chapter9/stdarg.html
// https://cplusplus.com/reference/cstdio/printf/

void blt::logToConsoleAndFile(const char* prefix, const char* format, ...) {

}

void blt::logToConsole(const char* prefix, const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    va_end(args);
}

void blt::logToFile(const char* prefix, const char* format, ...) {

}

void blt::logToConsoleAndFile(const char* format, ...) {

}

void blt::logToConsole(const char* format, ...) {

}

void blt::logToFile(const char* format, ...) {

}

