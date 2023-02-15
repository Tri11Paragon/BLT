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
#include <algorithm>
#include <blt/std/format.h>

namespace blt::profiling {
    
    // TODO: a better way
    std::mutex profileLock{};
    std::unordered_map<std::string, profile> profiles;
    
    void startInterval(const std::string& profileName, const std::string& intervalName) {
        std::scoped_lock lock(profileLock);
        capture_interval interval{};
        interval.start = system::getCurrentTimeNanoseconds();
        profiles[profileName].intervals[intervalName] = interval;
    }
    
    void endInterval(const std::string& profileName, const std::string& intervalName) {
        std::scoped_lock lock(profileLock);
        profiles[profileName].intervals[intervalName].end = system::getCurrentTimeNanoseconds();
        profiles[profileName].historicalIntervals[intervalName].push_back(profiles[profileName].intervals[intervalName]);
    }
    
    void point(const std::string& profileName, const std::string& pointName) {
        std::scoped_lock lock(profileLock);
        capture_point point{};
        point.point = system::getCurrentTimeNanoseconds();
        point.name = pointName;
        profiles[profileName].points.push(point);
    }
    
    capture_interval getInterval(const std::string& profileName, const std::string& intervalName) {
        return profiles[profileName].intervals[intervalName];
    }
    
    profile getProfile(const std::string& profileName) {
        return profiles[profileName];
    }
    
    inline void print(const std::vector<std::string>& lines, logging::LOG_LEVEL level) {
        auto& logger = logging::getLoggerFromLevel(level);
        for (const auto& line : lines)
            logger << line;
    }
    
    void printProfile(const std::string& profileName, blt::logging::LOG_LEVEL loggingLevel, bool averageHistory) {
        string::TableFormatter formatter {profileName};
        formatter.addColumn({"Interval"});
        formatter.addColumn({"Time (ns)"});
        formatter.addColumn({"Time (ms)"});
    
        auto& profile = profiles[profileName];
        const auto& intervals = profile.intervals;
        const auto& points = profile.points;
        
        for (const auto& interval : intervals) {
            if (averageHistory) {
                const auto& history = profile.historicalIntervals[interval.first];
                long total_difference = 0;
                for (const auto& h_interval : history) {
                    const auto difference = h_interval.end - h_interval.start;
                    total_difference += difference;
                }
                total_difference /= (long) history.size();
                formatter.addRow({interval.first, std::to_string(total_difference), std::to_string((double) total_difference / 1000000.0)});
            } else {
                const auto difference = interval.second.end - interval.second.start;
                formatter.addRow({interval.first, std::to_string(difference), std::to_string((double) difference / 1000000.0)});
            }
        }
        
        std::vector<std::string> updatedLines;
        const auto& lines = formatter.createTable(true, true);
        for (const auto& line : lines)
            updatedLines.emplace_back(line + "\n");
        
        print(updatedLines, loggingLevel);
    }
    
    struct timeOrderContainer {
        long difference;
        std::string name;
        
        timeOrderContainer(long difference, std::string name): difference(difference), name(std::move(name)) {}
    };
    
    inline bool timeCompare(const timeOrderContainer& container1, const timeOrderContainer& container2) {
        return container1.difference < container2.difference;
    }
    
    void printOrderedProfile(const std::string& profileName, logging::LOG_LEVEL loggingLevel, bool averageHistory) {
        auto& profile = profiles[profileName];
        const auto& intervals = profile.intervals;
        const auto& points = profile.points;
    
        std::vector<timeOrderContainer> unorderedIntervalVector;
    
        for (const auto& interval : intervals) {
            if (averageHistory) {
                const auto& history = profile.historicalIntervals[interval.first];
                long total_difference = 0;
                for (const auto& h_interval : history) {
                    const auto difference = h_interval.end - h_interval.start;
                    total_difference += difference;
                }
                total_difference /= (long) history.size();
                unorderedIntervalVector.emplace_back(total_difference, std::string("(H) ") += interval.first);
            } else {
                const auto difference = interval.second.end - interval.second.start;
                unorderedIntervalVector.emplace_back(difference, interval.first);
            }
        }
    
        std::sort(unorderedIntervalVector.begin(), unorderedIntervalVector.end(), timeCompare);
    
        string::TableFormatter formatter {profileName};
        formatter.addColumn({"Order"});
        formatter.addColumn({"Interval"});
        formatter.addColumn({"Time (ns)"});
        formatter.addColumn({"Time (ms)"});
    
        int index = 1;
        for (const auto& interval : unorderedIntervalVector) {
            formatter.addRow({std::to_string(index++), interval.name, std::to_string(interval.difference), std::to_string(interval.difference / 1000000.0)});
        }
    
        std::vector<std::string> updatedLines;
        const auto& lines = formatter.createTable(true, true);
        for (const auto& line : lines)
            updatedLines.emplace_back(line + "\n");
    
        print(updatedLines, loggingLevel);
    }
    
    void discardProfiles() {
        profiles = {};
    }
    
    void discardIntervals(const std::string& profileName) {
        profiles[profileName].intervals = {};
        profiles[profileName].historicalIntervals = {};
    }
    
    void discardPoints(const std::string& profileName) {
        profiles[profileName].points = {};
    }
    
    std::vector<capture_interval> getAllIntervals(const std::string& profileName, const std::string& intervalName) {
        return profiles[profileName].historicalIntervals[intervalName];
    }
}