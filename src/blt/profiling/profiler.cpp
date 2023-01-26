/*
 * Created by Brett on 23/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/profiling/profiler.h>
#include <mutex>
#include <vector>
#include <blt/std/time.h>
#include <blt/std/logging.h>
#include <iostream>

namespace blt::profiling {
    
    // TODO: a better way
    std::mutex profileLock{};
    std::unordered_map<std::string, Profile> profiles;
    
    void startInterval(const std::string& profileName, const std::string& intervalName) {
        std::scoped_lock lock(profileLock);
        CaptureInterval interval{};
        interval.start = System::getCurrentTimeNanoseconds();
        profiles[profileName].intervals[intervalName] = interval;
    }
    
    void endInterval(const std::string& profileName, const std::string& intervalName) {
        std::scoped_lock lock(profileLock);
        profiles[profileName].intervals[intervalName].end = System::getCurrentTimeNanoseconds();
    }
    
    void point(const std::string& profileName, const std::string& pointName) {
    
    }
    
    CaptureInterval getInterval(const std::string& profileName, const std::string& intervalName) {
        return profiles[profileName].intervals[intervalName];
    }
    
    Profile getProfile(const std::string& profileName) {
        return profiles[profileName];
    }
    
    void printProfile(const std::string& profileName, int loggingLevel) {
        //auto& out = loggingLevel < 0 ? std::cout : logging::loggerLevelDecode[loggingLevel];
        
    }
    
    void printOrderedProfile(const std::string& profileName, int loggingLevel) {
    
    }
    
    void discardProfiles() {
        profiles = {};
    }
    
    void discardIntervals(const std::string& profileName) {
        profiles[profileName].intervals = {};
    }
    
    void discardPoints(const std::string& profileName) {
        profiles[profileName].points = {};
    }
}