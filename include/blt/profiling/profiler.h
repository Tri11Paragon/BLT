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
    struct capture_point {
        std::string name;
        long point {};
    };
    
    struct capture_interval {
        long start;
        long end;
    };
    
    struct profile {
        std::unordered_map<std::string, capture_interval> intervals;
        std::unordered_map<std::string, std::vector<capture_interval>> historicalIntervals;
        blt::flat_queue<capture_point> points;
    };
    
    void startInterval(const std::string& profileName, const std::string& intervalName);
    void endInterval(const std::string& profileName, const std::string& intervalName);
    
    void point(const std::string& profileName, const std::string& pointName);
    
    capture_interval getInterval(const std::string& profileName, const std::string& intervalName);
    std::vector<capture_interval> getAllIntervals(const std::string& profileName, const std::string& intervalName);
    profile getProfile(const std::string& profileName);
    
    void printProfile(const std::string& profileName, blt::logging::LOG_LEVEL loggingLevel = logging::NONE, bool averageHistory = false);
    void printOrderedProfile(const std::string& profileName, logging::LOG_LEVEL loggingLevel = logging::NONE, bool averageHistory = false);
    
    void discardProfiles();
    void discardIntervals(const std::string& profileName);
    void discardPoints(const std::string& profileName);
    
    class scoped_interval {
        private:
            std::string m_Profile;
            std::string m_Interval;
        public:
            scoped_interval(std::string profile, std::string interval): m_Profile(std::move(profile)), m_Interval(std::move(interval)) {
                blt::profiling::startInterval(m_Profile, m_Interval);
            }
            ~scoped_interval() {
                blt::profiling::endInterval(m_Profile, m_Interval);
            }
    };
}

#endif //BLT_PROFILER_H

#ifdef BLT_DISABLE_PROFILING
    #define BLT_START_INTERVAL(profileName, intervalName)
    #define BLT_END_INTERVAL(profileName, intervalName)
    #define BLT_POINT(profileName, pointName)
    #define BLT_PRINT_PROFILE(profileName, ...)
    #define BLT_PRINT_ORDERED(profileName, ...)
#else
    #define BLT_START_INTERVAL(profileName, intervalName) blt::profiling::startInterval(profileName, intervalName)
    #define BLT_END_INTERVAL(profileName, intervalName) blt::profiling::endInterval(profileName, intervalName)
    #define BLT_POINT(profileName, pointName) blt::profiling::point(profileName, pointName)
    #define BLT_PRINT_PROFILE(profileName, ...) blt::profiling::printProfile(profileName, ##__VA_ARGS__)
    #define BLT_PRINT_ORDERED(profileName, ...) blt::profiling::printOrderedProfile(profileName, ##__VA_ARGS__)
#endif

#define BLT_INTERVAL_START(profileName, intervalName) BLT_START_INTERVAL(profileName, intervalName)
#define BLT_INTERVAL_END(profileName, intervalName) BLT_END_INTERVAL(profileName, intervalName)