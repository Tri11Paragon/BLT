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
    
    struct IntervalComparable {
        long difference;
        std::string name;
        
        IntervalComparable(long difference, std::string name):
                difference(difference), name(std::move(name)) {}
    };
    
    inline void println(const std::vector<std::string>&& lines, logging::LOG_LEVEL level) {
        auto& logger = logging::getLoggerFromLevel(level);
        for (const auto& line : lines)
            logger << line << "\n";
    }
    
    /**
     * Copy-sorts the unordered vector into the ordered vector, min -> max
     */
    inline void orderIntervals(
            const std::unordered_map<std::string, capture_interval>& unordered,
            std::vector<IntervalComparable>& ordered
    ) {
        // copy
        for (const auto& i : unordered)
            ordered.emplace_back(i.second.end - i.second.start, i.first);
        
        // sort
        std::sort(
                ordered.begin(), ordered.end(),
                [&](const IntervalComparable& c1, const IntervalComparable& c2) -> bool {
                    return c1.difference < c2.difference;
                }
        );
    }
    
    inline void averageIntervals(
            const std::unordered_map<std::string, std::vector<capture_interval>>& intervals,
            std::unordered_map<std::string, capture_interval>& averagedIntervals
    ) {
        // average all intervals
        for (const auto& i : intervals) {
            const auto& name = i.first;
            const auto& interval_vec = i.second;
            long total_difference = 0;
            
            // calculate total difference
            for (const auto& value : interval_vec) {
                auto difference = value.end - value.start;
                // negatives make no sense. remove them to prevent errors
                if (difference > 0)
                    total_difference += difference;
            }
    
            total_difference /= (long)interval_vec.size();
            
            // create a new name for the interval that includes the sample size
            std::string new_name = "(";
            new_name += std::to_string(interval_vec.size());
            new_name += ") ";
            new_name += name;
            
            // we can exploit how the order func works by supplying the difference into end,
            // which sorts correctly despite not being a true interval.
            averagedIntervals.insert({new_name, capture_interval{0, total_difference}});
        }
    }
    
    void writeProfile(std::ofstream& out, const std::string& profileName, bool averageHistory) {
        auto& profile = profiles[profileName];
        const auto& intervals = profile.intervals;
        const auto& points = profile.points;
    
        std::vector<IntervalComparable> ordered_vector;
        std::unordered_map<std::string, capture_interval> averaged_intervals;
    
        if (averageHistory)
            averageIntervals(profile.historicalIntervals, averaged_intervals);
    
        orderIntervals(averageHistory ? averaged_intervals : intervals, ordered_vector);
    }
    
    void printProfile(
            const std::string& profileName, logging::LOG_LEVEL loggingLevel, bool averageHistory
    ) {
        auto& profile = profiles[profileName];
        const auto& intervals = profile.intervals;
        const auto& points = profile.points;
        
        std::vector<IntervalComparable> ordered_vector;
        std::unordered_map<std::string, capture_interval> averaged_intervals;
        
        if (averageHistory)
            averageIntervals(profile.historicalIntervals, averaged_intervals);
        
        orderIntervals(averageHistory ? averaged_intervals : intervals, ordered_vector);
        
        string::TableFormatter formatter{profileName};
        formatter.addColumn({"Order"});
        formatter.addColumn({"Interval"});
        formatter.addColumn({"Time (ns)"});
        formatter.addColumn({"Time (ms)"});
        
        int index = 1;
        for (const auto& interval : ordered_vector) {
            formatter.addRow(
                    {std::to_string(index++), interval.name, std::to_string(interval.difference),
                     std::to_string((double)interval.difference / 1000000.0)}
            );
        }
    
        println(formatter.createTable(true, true), loggingLevel);
    }

// --- small helper functions below ---
    
    void startInterval(const std::string& profileName, const std::string& intervalName) {
        std::scoped_lock lock(profileLock);
        capture_interval interval{};
        interval.start = system::getCurrentTimeNanoseconds();
        profiles[profileName].intervals[intervalName] = interval;
    }
    
    void endInterval(const std::string& profileName, const std::string& intervalName) {
        std::scoped_lock lock(profileLock);
        profiles[profileName].intervals[intervalName].end = system::getCurrentTimeNanoseconds();
        profiles[profileName].historicalIntervals[intervalName].push_back(
                profiles[profileName].intervals[intervalName]
        );
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
    
    std::vector<capture_interval> getAllIntervals(
            const std::string& profileName, const std::string& intervalName
    ) {
        return profiles[profileName].historicalIntervals[intervalName];
    }
    
}