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
#include <blt/std/string.h>
#include <iostream>
#include <algorithm>

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
    
    inline void print(const std::vector<std::string>& lines, int level) {
        if (level < 0) {
            for (const auto& line : lines)
                std::cout << line;
        } else {
            auto& logger = logging::loggerLevelDecode[level];
            for (const auto& line : lines)
                logger << line;
        }
    }
    
    void printProfile(const std::string& profileName, int loggingLevel) {
        std::vector<std::string> lines;
        const auto& profile = profiles[profileName];
        const auto& intervals = profile.intervals;
        const auto& points = profile.points;
        
        {
            std::string profileNameString = "Profile ";
            profileNameString += profileName;
            profileNameString += " Recorded {\n";
            lines.emplace_back(profileNameString);
            
            for (const auto& interval : intervals) {
                const auto difference = interval.second.end - interval.second.start;
                std::string currentIntervalString = "\t";
                currentIntervalString += interval.first;
                currentIntervalString += " ";
                currentIntervalString += std::to_string(difference);
                currentIntervalString += "ns (";
                currentIntervalString += std::to_string((double) difference / 1000000.0);
                currentIntervalString += "ms);\n";
                
                lines.emplace_back(currentIntervalString);
            }
            lines.emplace_back("}\n");
        }
        print(lines, loggingLevel);
    }
    
    struct timeOrderContainer {
        long difference;
        std::string name;
        
        timeOrderContainer(long difference, std::string name): difference(difference), name(std::move(name)) {}
    };
    
    inline bool timeCompare(const timeOrderContainer& container1, const timeOrderContainer& container2) {
        return container1.difference < container2.difference;
    }
    
    inline std::pair<size_t, size_t> longestInterval(const std::vector<timeOrderContainer>& intervalDifferences) {
        size_t longestName = 0;
        size_t longestIntervalTime = 0;
        for (const auto& intervalDifference : intervalDifferences) {
            longestName = std::max(longestName, intervalDifference.name.length() - 1);
            longestIntervalTime = std::max(longestIntervalTime, std::to_string(intervalDifference.difference).length() - 1);
        }
        return {longestName, longestIntervalTime};
    }
    
    void printOrderedProfile(const std::string& profileName, int loggingLevel) {
        std::vector<std::string> lines;
        const auto& profile = profiles[profileName];
        const auto& intervals = profile.intervals;
        const auto& points = profile.points;
        
        {
            std::string profileNameString = "Profile ";
            profileNameString += profileName;
            profileNameString += " Recorded {\n";
            lines.emplace_back(profileNameString);
            
            std::vector<timeOrderContainer> unorderedIntervalVector;
            
            for (const auto& interval : intervals) {
                const auto difference = interval.second.end - interval.second.start;
                unorderedIntervalVector.emplace_back(difference, interval.first);
            }
            
            std::sort(unorderedIntervalVector.begin(), unorderedIntervalVector.end(), timeCompare);
            
            auto longestName = longestInterval(unorderedIntervalVector);
            
            
            int count = 1;
            for (const auto& interval : unorderedIntervalVector) {
                const auto difference = interval.difference;
                std::string currentIntervalString = "\t";
                std::string countStr = std::to_string(count++);
                countStr += ". ";
                auto countMaxStrSize = std::to_string(unorderedIntervalVector.size() + 1).length() + countStr.length();
                currentIntervalString += blt::string::postPadWithSpaces(countStr, countMaxStrSize);
                currentIntervalString += blt::string::postPadWithSpaces(interval.name, longestName.first);
                currentIntervalString += "\t";
                currentIntervalString += blt::string::postPadWithSpaces(std::to_string(difference), longestName.second);
                currentIntervalString += "ns (";
                currentIntervalString += std::to_string((double) difference / 1000000.0);
                currentIntervalString += "ms);\n";
                
                lines.emplace_back(currentIntervalString);
            }
            lines.emplace_back("}\n");
        }
        print(lines, loggingLevel);
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