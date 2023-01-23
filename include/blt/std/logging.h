/*
 * Created by Brett on 23/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_LOGGING_H
#define BLT_TESTS_LOGGING_H

namespace blt {
    void logToConsoleAndFile(const char* format, ...);
    void logToConsole(const char* format, ...);
    void logToFile(const char* format, ...);
    void logToConsoleAndFile(const char* prefix, const char* format, ...);
    void logToConsole(const char* prefix, const char* format, ...);
    void logToFile(const char* prefix, const char* format, ...);
}

#define BLT

#endif //BLT_TESTS_LOGGING_H
