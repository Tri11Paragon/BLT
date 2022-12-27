/*
 * Created by Brett on 23/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_PROFILER_H
#define BLT_PROFILER_H

#include <string>
#include <string_view>
#include <unordered_map>


namespace BLT {
    struct CapturePoint {
        long point;
    };
    
    struct CaptureInterval {
        CapturePoint start;
        CapturePoint end;
    };
    
    class Profiler {
        private:
            std::map<std::string_view, CaptureInterval>* intervals;
            std::map<std::string_view, CapturePoint>* points;
        public:
            Profiler(std::map<std::string_view, CaptureInterval>* test) {
                intervals = test;
            }
    };
}

#endif //BLT_PROFILER_H
