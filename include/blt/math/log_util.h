/*
 * Created by Brett on 14/03/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_LOG_UTIL_H
#define BLT_TESTS_LOG_UTIL_H

#include <blt/math/vectors.h>
#include <blt/math/matrix.h>
#include <blt/std/logging_old.h>

namespace blt {

    template<typename T, unsigned long size>
    static inline logging::logger& operator<<(logging::logger& log, blt::vec<T, size> vec){
        log << "(";
        for (int i = 0; i < size; i++)
            log << vec[i] << ((i == size-1) ? ")" : ", ");
        return log;
    }

}

#endif //BLT_TESTS_LOG_UTIL_H
