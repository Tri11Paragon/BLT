/*
 * Created by Brett on 20/07/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_LOGGING2_H
#define BLT_TESTS_LOGGING2_H

#include <string>
#include <type_traits>
#include <functional>
#include <blt/config.h>

namespace blt::logging {
    
    enum class log_level {
        // low level
        TRACE0, TRACE1, TRACE2, TRACE3,
        // normal
        TRACE, DEBUG, INFO,
        // errors
        WARN, ERROR, FATAL,
        // default
        NONE
    };
    
    struct log_tag {
        // tag without the ${{ or }}
        std::string tag;
        // function to run, log level and raw user input string are provided
        std::function<std::string(blt::logging::log_level, std::string)> func;
    };
    
    struct log_format {
        /**
         * the log output format is the string which will be used to create the log output string
         *
         * Available tags:
         *  - ${{YEAR}}         // current year
         *  - ${{MONTH}}        // current month
         *  - ${{DAY}}          // current day
         *  - ${{HOUR}}         // current hour
         *  - ${{MINUTE}}       // current minute
         *  - ${{SECOND}}       // current second
         *  - ${{MS}}           // current millisecond
         *  - ${{LF}}            // log level color (ASCII color code)
         *  - ${{R}}            // ASCII color reset
         *  - ${{LOG_LEVEL}}    // current log level
         *  - ${{THREAD_NAME}}  // current thread name, NOTE: thread names must be set by calling "setThreadName()" from the thread in question!
         *  - ${{FILE}}         // file from which the macro is invoked
         *  - ${{LINE}}         // line in the from which the macro is invoked
         *  - ${{RAW_STR}}      // raw user string without formatting applied (NOTE: format args are not provided!)
         *  - ${{STR}}          // the user supplied string (format applied!)
         */
        std::string logOutputFormat = "[${{HOUR}}:${{MINUTE}}:${{SECOND}] ${{LF}}[${{LOG_LEVEL}}]${{R}} ${{STR}}";
    };
    
    void log(log_level level, const std::string& format, const char* file, int line, ...);
    
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
    inline void log(T t, log_level level, const char* file, int line) {
        log(std::to_string(t), level, file, line);
    }
    
    void flush();
    
    void setThreadName(const std::string& name);
    
}

#ifdef BLT_LOGGING_IMPLEMENTATION
    
    #include <iostream>

namespace blt::logging {
    
    void log(const std::string& format, log_level level, const char* file, int line, ...) {
        
    }
    
    void setThreadName(const std::string& name) {
        
    }
    
    void flush() {
        std::cerr.flush();
        std::cout.flush();
    }
    
}

#endif

#if !defined(BLT_ENABLE_LOGGING) || defined(BLT_DISABLE_LOGGING)
    #define BLT_LOG(format, level, ...)
    #define BLT_TRACE(format, ...)
    #define BLT_DEBUG(format, ...)
    #define BLT_INFO(format, ...)
    #define BLT_WARN(format, ...)
    #define BLT_ERROR(format, ...)
    #define BLT_FATAL(format, ...)
#else
    #define BLT_LOG(format, level, ...) log(format, level, __FILE__, __LINE__, ##__VA_ARGS__)
    #ifndef BLT_ENABLE_TRACE
        #define BLT_TRACE(format, ...)
    #else
        #define BLT_TRACE(format, ...) BLT_LOG(format, blt::logging::log_level::TRACE, ##__VA_ARGS__)
    #endif
    
    #ifndef BLT_ENABLE_DEBUG
        #define BLT_DEBUG(format, ...)
    #else
        #define BLT_DEBUG(format, ...) BLT_LOG(format, blt::logging::log_level::DEBUG, ##__VA_ARGS__)
    #endif
    
    #ifndef BLT_ENABLE_INFO
        #define BLT_INFO(format, ...)
    #else
        #define BLT_INFO(format, ...) BLT_LOG(format, blt::logging::log_level::INFO, ##__VA_ARGS__)
    #endif
    
    #ifndef BLT_ENABLE_WARN
        #define BLT_WARN(format, ...)
    #else
        #define BLT_WARN(format, ...) BLT_LOG(format, blt::logging::log_level::WARN, ##__VA_ARGS__)
    #endif
    
    #ifndef BLT_ENABLE_ERROR
        #define BLT_ERROR(format, ...)
    #else
        #define BLT_ERROR(format, ...) BLT_LOG(format, blt::logging::log_level::ERROR, ##__VA_ARGS__)
    #endif
    
    #ifndef BLT_ENABLE_FATAL
        #define BLT_FATAL(format, ...)
    #else
        #define BLT_FATAL(format, ...) BLT_LOG(format, blt::logging::log_level::FATAL, ##__VA_ARGS__)
    #endif
#endif

#endif //BLT_TESTS_LOGGING2_H
