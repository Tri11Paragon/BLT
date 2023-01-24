/*
 * Created by Brett on 23/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_LOGGING_H
#define BLT_TESTS_LOGGING_H

namespace blt::logging {
    
    enum LOG_LEVEL {
        TRACE = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4, FATAL = 5
    };
    
    struct LOG_PROPERTIES {
        bool m_useColor = true;
        bool m_logToConsole = true;
        bool m_logToFile = true;
        const char* m_directory = "./";
        
        explicit constexpr LOG_PROPERTIES(bool useColor, bool logToConsole, bool logToFile, const char* directory):
                m_useColor(useColor), m_logToConsole(logToConsole), m_logToFile(logToFile), m_directory(directory) {}
        
        explicit constexpr LOG_PROPERTIES() = default;
    };
    
    void init(LOG_PROPERTIES properties);
    void log(LOG_LEVEL level, bool auto_line, const char* format, ...);
}

#ifdef BLT_DISABLE_LOGGING
    #define BLT_TRACE(format)
    #define BLT_DEBUG(format)
    #define BLT_INFO(format)
    #define BLT_WARN(format)
    #define BLT_ERROR(format)
    #define BLT_FATAL(format)
    
    #define BLT_TRACE_FMT(format, args...)
    #define BLT_DEBUG_FMT(format, args...)
    #define BLT_INFO_FMT(format, args...)
    #define BLT_WARN_FMT(format, args...)
    #define BLT_ERROR_FMT(format, args...)
    #define BLT_FATAL_FMT(format, args...)
#else
    #define BLT_TRACE(format) log(blt::logging::TRACE, false, format);
    #define BLT_DEBUG(format) log(blt::logging::DEBUG, false, format);
    #define BLT_INFO(format) log(blt::logging::INFO, false, format);
    #define BLT_WARN(format) log(blt::logging::WARN, false, format);
    #define BLT_ERROR(format) log(blt::logging::ERROR, false, format);
    #define BLT_FATAL(format) log(blt::logging::FATAL, false, format);
    
    #define BLT_TRACE_FMT(format, args...) log(blt::logging::TRACE, false, format, args);
    #define BLT_DEBUG_FMT(format, args...) log(blt::logging::DEBUG, false, format, args);
    #define BLT_INFO_FMT(format, args...) log(blt::logging::INFO, false, format, args);
    #define BLT_WARN_FMT(format, args...) log(blt::logging::WARN, false, format, args);
    #define BLT_ERROR_FMT(format, args...) log(blt::logging::ERROR, false, format, args);
    #define BLT_FATAL_FMT(format, args...) log(blt::logging::FATAL, false, format, args);
    
    #define BLT_TRACE_LN(format) log(blt::logging::TRACE, true, format);
    #define BLT_DEBUG_LN(format) log(blt::logging::DEBUG, true, format);
    #define BLT_INFO_LN(format) log(blt::logging::INFO, true, format);
    #define BLT_WARN_LN(format) log(blt::logging::WARN, true, format);
    #define BLT_ERROR_LN(format) log(blt::logging::ERROR, true, format);
    #define BLT_FATAL_LN(format) log(blt::logging::FATAL, true, format);
    
    #define BLT_TRACE_FMT_LN(format, args...) log(blt::logging::TRACE, true, format, args);
    #define BLT_DEBUG_FMT_LN(format, args...) log(blt::logging::DEBUG, true, format, args);
    #define BLT_INFO_FMT_LN(format, args...) log(blt::logging::INFO, true, format, args);
    #define BLT_WARN_FMT_LN(format, args...) log(blt::logging::WARN, true, format, args);
    #define BLT_ERROR_FMT_LN(format, args...) log(blt::logging::ERROR, true, format, args);
    #define BLT_FATAL_FMT_LN(format, args...) log(blt::logging::FATAL, true, format, args);
#endif

#endif //BLT_TESTS_LOGGING_H
