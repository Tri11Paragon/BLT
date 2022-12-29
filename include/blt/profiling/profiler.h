/*
 * Created by Brett on 23/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_PROFILER_H
#define BLT_PROFILER_H

#include <string>
#include <string_view>
#include <blt/config.h>
#include <mutex>

#ifdef PHMAP_ENABLED
    #include <parallel_hashmap/phmap.h>
#else
    #include <unordered_map>
#endif


namespace BLT {
    struct CapturePoint {
        long point;
    };
    
    struct CaptureInterval {
        CapturePoint start;
        CapturePoint end;
    };
#ifdef PHMAP_ENABLED
    typedef phmap::parallel_flat_hash_map<std::string_view, CaptureInterval> INTERVAL_MAP;
    typedef phmap::parallel_flat_hash_map<std::string_view, CapturePoint> POINT_MAP;
    typedef phmap::parallel_flat_hash_map<int, std::string_view> ORDER_MAP;
#else
    typedef std::unordered_map<std::string_view, CaptureInterval> INTERVAL_MAP;
    typedef std::unordered_map<std::string_view, CapturePoint> POINT_MAP;
    typedef std::unordered_map<int, std::string_view> ORDER_MAP;
#endif
    
    class Profiler {
        private:
            INTERVAL_MAP intervals{};
            POINT_MAP points{};
            ORDER_MAP order{};
            
            std::mutex timerLock{};
            int lastOrder = 0;
        public:
            Profiler() = default;
            void finishCycle();
            void startInterval(const std::string_view& name);
            void endInterval(const std::string_view& name);
    };
}

#endif //BLT_PROFILER_H
