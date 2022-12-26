/*
 * Created by Brett on 23/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_PROFILER_H
#define BLT_PROFILER_H

#include <string>
#include <string_view>

namespace BLT {
    struct CapturePoint {
        long point;
    };
    
    struct CaptureInterval {
        CapturePoint start;
        CapturePoint end;
    };
    
    template<class MAP_TYPE>
    class Profiler {
        private:
            MAP_TYPE intervals;
            MAP_TYPE points;
    };
}

#endif //BLT_PROFILER_H
