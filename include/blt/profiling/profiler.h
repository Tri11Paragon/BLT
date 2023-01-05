/*
 * Created by Brett on 23/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_PROFILER_H
#define BLT_PROFILER_H

#include <string>
#include <string_view>
#include <mutex>
#include <queue>
#include <blt/std/time.h>

namespace BLT {
    struct CapturePoint {
        std::string_view name;
        long point;
    };
    
    struct CaptureInterval {
        long start;
        long end;
    };
    
    /**
     * @tparam HASHMAP_TYPE
     */
    template <template<typename, typename> class HASHMAP_TYPE>
    class profile {
        private:
            // profiling intervals.
            HASHMAP_TYPE<std::string_view, CaptureInterval> intervals{};
            
            // profiling points
            std::vector<std::queue<CapturePoint>> cyclicPointsHistory{};
            std::queue<CapturePoint> points{};
            std::queue<CapturePoint> cyclicPoints{};
            
            std::mutex timerLock{};
        public:
            profile() = default;
            void finishCycle() {
                cyclicPointsHistory.push_back(cyclicPoints);
                // im not sure if this is the correct way to clear a queue, there is no function to do so.
                cyclicPoints = {};
            }
            void startInterval(const std::string_view& name) {
                std::scoped_lock lock(timerLock);
                CaptureInterval interval{};
                interval.start = System::getCurrentTimeNanoseconds();
                intervals[name] = interval;
            }
            void endInterval(const std::string_view& name) {
                std::scoped_lock lock(timerLock);
                intervals[name].end = System::getCurrentTimeNanoseconds();
            }
            /**
             * Records the current time for the purpose of reconstructing the execution time between points, in order to find the most common cause for performance issues.
             * @param name a common name for the point which you are trying to profile. This name should be meaningful as it will be displayed in the output.
             */
            void profilerPoint(const std::string_view& name) {
                points.push(CapturePoint{name, System::getCurrentTimeNanoseconds()});
            }
            /**
            * Records the current time for the purpose of reconstructing the execution time between points, in order to find the most common cause for performance issues.
            * Uses a separate tracking device that will be reset when finishCycle(); is called.
            * @param name a common name for the point which you are trying to profile. This name should be meaningful as it will be displayed in the output.
            */
            void profilerPointCyclic(const std::string_view& name) {
                cyclicPoints.push(CapturePoint{name, System::getCurrentTimeNanoseconds()});
            }
    };
}

#endif //BLT_PROFILER_H
