/*
 * Created by Brett on 23/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_PROFILER_H
#define BLT_PROFILER_H

#include <string>
#include <blt/std/queue.h>
#include <vector>
#include <unordered_map>
#include <blt/std/logging.h>

/**
 * Defines several disableable macros (#define BLT_DISABLE_PROFILING). If you do not use these macros profiling will not be disableable
 */

namespace blt::profiling {
    struct CapturePoint {
        std::string name;
        long point {};
    };
    
    struct CaptureInterval {
        long start;
        long end;
    };
    
    struct Profile {
        std::unordered_map<std::string, CaptureInterval> intervals;
        std::unordered_map<std::string, std::vector<CaptureInterval>> historicalIntervals;
        blt::flat_queue<CapturePoint> points;
    };
    
    void startInterval(const std::string& profileName, const std::string& intervalName);
    void endInterval(const std::string& profileName, const std::string& intervalName);
    
    void point(const std::string& profileName, const std::string& pointName);
    
    CaptureInterval getInterval(const std::string& profileName, const std::string& intervalName);
    std::vector<CaptureInterval> getAllIntervals(const std::string& profileName, const std::string& intervalName);
    Profile getProfile(const std::string& profileName);
    
    void printProfile(const std::string& profileName, blt::logging::LOG_LEVEL loggingLevel = logging::NONE);
    void printOrderedProfile(const std::string& profileName, logging::LOG_LEVEL loggingLevel = logging::NONE);
    
    void discardProfiles();
    void discardIntervals(const std::string& profileName);
    void discardPoints(const std::string& profileName);
}

#endif //BLT_PROFILER_H

#ifdef BLT_DISABLE_PROFILING
    #define BLT_START_INTERVAL(profileName, intervalName)
    #define BLT_END_INTERVAL(profileName, intervalName)
    #define BLT_POINT(profileName, pointName)
    #define BLT_PRINT_PROFILE(profileName, ...)
    #define BLT_PRINT_ORDERED(profileName, ...)
#else
    #define BLT_START_INTERVAL(profileName, intervalName) blt::profiling::startInterval(profileName, intervalName);
    #define BLT_END_INTERVAL(profileName, intervalName) blt::profiling::endInterval(profileName, intervalName);
    #define BLT_POINT(profileName, pointName) blt::profiling::point(profileName, pointName);
    #define BLT_PRINT_PROFILE(profileName, ...) blt::profiling::printProfile(profileName, ##__VA_ARGS__);
    #define BLT_PRINT_ORDERED(profileName, ...) blt::profiling::printOrderedProfile(profileName, ##__VA_ARGS__);
#endif

#define BLT_INTERVAL_START(profileName, intervalName) BLT_START_INTERVAL(profileName, intervalName)
#define BLT_INTERVAL_END(profileName, intervalName) BLT_END_INTERVAL(profileName, intervalName)