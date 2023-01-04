/*
 * Created by Brett on 23/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/profiling/profiler.h>
#include <iostream>
#include <utility>

namespace BLT {
    
    void Profiler::finishCycle() {
    
    }
    
    static inline auto getCurrentTimeNanoseconds() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    void Profiler::startInterval(const std::string_view& name) {
        std::scoped_lock lock(timerLock);
        CaptureInterval interval{};
        interval.start = {getCurrentTimeNanoseconds()};
        intervals[name] = interval;
    }
    
    void Profiler::endInterval(const std::string_view& name) {
        std::scoped_lock lock(timerLock);
        intervals[name].end = {getCurrentTimeNanoseconds()};
        order[lastOrder++] = name;
    }
    
    void Profiler::profilerPoint() {
    
    }
    
    void Profiler::profilerPointCyclic() {
    
    }
    
}