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
#include <blt/std/logging.h>

namespace blt
{
    // use the historical values (average) instead of the latest values
    static inline constexpr std::uint32_t PRINT_HISTORY = 0x1;
    // print out the cycles
    static inline constexpr std::uint32_t PRINT_CYCLES = 0x2;
    // print out the wall time
    static inline constexpr std::uint32_t PRINT_WALL = 0x4;
    // print out the thread CPU time
    static inline constexpr std::uint32_t PRINT_THREAD = 0x8;
    
    enum class sort_by
    {
        CYCLES,
        WALL,
        THREAD
    };
    
    // 32 bit currently not supported
    typedef std::int64_t pf_time_t;
    typedef std::uint64_t pf_cycle_t;
    
    struct interval_t
    {
        pf_time_t wall_start = 0;
        pf_time_t wall_end = 0;
        pf_time_t wall_total = 0;
        
        pf_time_t thread_start = 0;
        pf_time_t thread_end = 0;
        pf_time_t thread_total = 0;
        
        pf_cycle_t cycles_start = 0;
        pf_cycle_t cycles_end = 0;
        pf_cycle_t cycles_total = 0;
        
        std::uint64_t count = 0;
        std::string interval_name;
    };
    
    struct cycle_interval_t
    {
        pf_cycle_t cycles_start = 0;
        pf_cycle_t cycles_end = 0;
        pf_cycle_t cycles_total = 0;
        std::uint64_t count = 0;
        std::string interval_name;
    };
    
    struct profile_t
    {
        std::vector<interval_t*> intervals;
        std::vector<cycle_interval_t*> cycle_intervals;
        std::string name;
        
        explicit profile_t(std::string name): name(std::move(name))
        {}
        
        profile_t(const profile_t& p) = delete;
        
        profile_t& operator=(const profile_t& p) = delete;
        
        ~profile_t();
    };
    
    interval_t* createInterval(profile_t& profiler, std::string interval_name);
    
    void startInterval(interval_t* interval);
    
    void endInterval(interval_t* interval);
    
    void printProfile(profile_t& profiler, std::uint32_t flags = PRINT_HISTORY | PRINT_CYCLES | PRINT_THREAD | PRINT_WALL,
                      sort_by sort = sort_by::CYCLES, blt::logging::log_level log_level = blt::logging::log_level::NONE);
    
    void writeProfile(std::ifstream& stream, const profile_t& profiler);
    
    void clearProfile(profile_t& profiler);
    
    namespace _internal
    {
        void startInterval(const std::string& profile_name, const std::string& interval_name);
        
        void endInterval(const std::string& profile_name, const std::string& interval_name);
        
        void printProfile(const std::string& profile_name);
        
        void writeProfile(const std::string& profile_name);
    }
    
    class auto_interval
    {
        private:
            interval_t* iv;
        public:
            auto_interval(std::string interval_name, profile_t& profiler)
            {
                iv = blt::createInterval(profiler, std::move(interval_name));
                blt::startInterval(iv);
            }
            
            explicit auto_interval(interval_t* iv): iv(iv)
            {
                blt::startInterval(iv);
            }
            
            auto_interval(const auto_interval& i) = delete;
            
            auto_interval& operator=(const auto_interval& i) = delete;
            
            ~auto_interval()
            {
                blt::endInterval(iv);
            }
    };
    
}

#endif //BLT_PROFILER_V2_H