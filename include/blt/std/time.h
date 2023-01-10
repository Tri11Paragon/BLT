/*
 * Created by Brett on 04/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TIME_H
#define BLT_TIME_H

#include <chrono>
#include <ctime>
#include <sstream>

namespace blt::System {
    static inline auto getCurrentTimeNanoseconds() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }
    
    /**
     * Standard time string is formatted as follows:
     * Year-Month-Date Hour:Min:Second
     * If you do not want a space in the string use getTimeStringFS(); (Time String for easy filesystem)
     * @return the BLT standard string of time.now
     */
    static inline std::string getTimeString() {
        auto t = std::time(nullptr);
        auto now = std::localtime(&t);
        std::stringstream timeString;
        timeString << (1900 + now->tm_year);
        timeString << "-";
        timeString << (1 + now->tm_mon);
        timeString << "-";
        timeString << now->tm_mday;
        timeString << " ";
        timeString << now->tm_hour;
        timeString << ":";
        timeString << now->tm_min;
        timeString << ":";
        timeString << now->tm_sec;
        return timeString.str();
    }
    /**
     * @return the BLT standard string of time.now (See getTimeString()) that is filesystem friendly (FAT compatible).
     */
    static inline std::string getTimeStringFS() {
        auto t = std::time(nullptr);
        auto now = std::localtime(&t);
        std::stringstream timeString;
        timeString << (1900 + now->tm_year);
        timeString << "-";
        timeString << (1 + now->tm_mon);
        timeString << "-";
        timeString << now->tm_mday;
        timeString << "_";
        timeString << now->tm_hour;
        timeString << "-";
        timeString << now->tm_min;
        timeString << "-";
        timeString << now->tm_sec;
        return timeString.str();
    }
}

#endif //BLT_TIME_H
