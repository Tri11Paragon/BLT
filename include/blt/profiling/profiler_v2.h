#pragma once
/*
 * Created by Brett on 20/09/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_PROFILER_V2_H
#define BLT_PROFILER_V2_H

#include <cstdint>
#include <string>
#include <vector>
#include <blt/std/hashmap.h>

namespace blt
{
    // 32 bit currently not supported
    typedef std::int64_t pf_time_t;
    
    struct interval_t
    {
        pf_time_t start = 0;
        pf_time_t end = 0;
        pf_time_t total = 0;
        pf_time_t count = 0;
    };
    
    struct profile_t
    {
        std::vector<interval_t> intervals;
    };
    
    void startInterval(interval_t& interval);
    
    void endInterval(profile_t& profiler, interval_t& interval);
    
    void printProfile(const profile_t& profiler);
    
    void writeProfile(std::ifstream& stream, const profile_t& profiler);
    
    namespace _internal
    {
        void startInterval(const std::string& profile_name, const std::string& interval_name);
        void endInterval(const std::string& profile_name, const std::string& interval_name);
        void printProfile(const std::string& profile_name);
        void writeProfile(const std::string& profile_name);
    }
    
}

#endif //BLT_PROFILER_V2_H
