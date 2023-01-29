/*
 * Created by Brett on 23/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_LOGGING_H
#define BLT_LOGGING_H

#include <string>

namespace blt::logging {
    
    enum LOG_LEVEL {
        TRACE = 0, DEBUG = 1, INFO = 2, WARN = 3, ERROR = 4, FATAL = 5
    };
    
    struct LOG_PROPERTIES {
        bool m_useColor = true;
        bool m_logToConsole = true;
        bool m_logToFile = true;
        const char* m_directory = "./log/";
        LOG_LEVEL minLevel = TRACE;
        
        explicit constexpr LOG_PROPERTIES(bool useColor, bool logToConsole, bool logToFile, const char* directory):
                m_useColor(useColor), m_logToConsole(logToConsole), m_logToFile(logToFile), m_directory(directory) {}
        
        explicit constexpr LOG_PROPERTIES() = default;
    };
    
    struct logger {
        LOG_LEVEL level;
        void logi(const std::string& str) const;
        // evil hack, todo: better way
        #ifdef BLT_DISABLE_LOGGING
            void log(const std::string& str) const {
            
            }
        #else
            void log(const std::string& str) const {
                logi(str);
            }
        #endif
    };
    
    static logger tlog{TRACE};
    static logger dlog{DEBUG};
    static logger ilog{INFO};
    static logger wlog{WARN};
    static logger elog{ERROR};
    static logger flog{FATAL};
    
    /**
     * Always in order of the enum!
     */
    static logger loggerLevelDecode[6]{tlog, dlog, ilog, wlog, elog, flog};

    static logger trace{TRACE};
    static logger debug{DEBUG};
    static logger info{INFO};
    static logger warn{WARN};
    static logger error{ERROR};
    static logger fatal{FATAL};
    
    static inline logger& operator<<(logger& out, const std::string& str) {
        out.log(str);
        return out;
    }
    
    static inline logger& operator<<(logger& out, const char chr) {
        out.log(std::to_string(chr));
        return out;
    }
    
    static inline logger& operator<<(logger& out, const unsigned char i) {
        out.log(std::to_string(i));
        return out;
    }
    
    static inline logger& operator<<(logger& out, const short i) {
        out.log(std::to_string(i));
        return out;
    }
    
    static inline logger& operator<<(logger& out, const unsigned short i) {
        out.log(std::to_string(i));
        return out;
    }
    
    static inline logger& operator<<(logger& out, const int i) {
        out.log(std::to_string(i));
        return out;
    }
    
    static inline logger& operator<<(logger& out, const unsigned int i) {
        out.log(std::to_string(i));
        return out;
    }
    
    static inline logger& operator<<(logger& out, const long i) {
        out.log(std::to_string(i));
        return out;
    }
    
    static inline logger& operator<<(logger& out, const unsigned long i) {
        out.log(std::to_string(i));
        return out;
    }
    
    static inline logger& operator<<(logger& out, const float f) {
        out.log(std::to_string(f));
        return out;
    }
    
    static inline logger& operator<<(logger& out, const double f) {
        out.log(std::to_string(f));
        return out;
    }
    
    void init(LOG_PROPERTIES properties);
    void log(const std::string& format, LOG_LEVEL level, int auto_line, ...);
    void log(int i, LOG_LEVEL level, int auto_line);
    void log(long i, LOG_LEVEL level, int auto_line);
    void log(unsigned int i, LOG_LEVEL level, int auto_line);
    void log(unsigned long i, LOG_LEVEL level, int auto_line);
    void log(char i, LOG_LEVEL level, int auto_line);
    void log(unsigned char i, LOG_LEVEL level, int auto_line);
    void log(short i, LOG_LEVEL level, int auto_line);
    void log(unsigned short i, LOG_LEVEL level, int auto_line);
    void log(float f, LOG_LEVEL level, int auto_line);
    void log(double f, LOG_LEVEL level, int auto_line);
}

#ifdef BLT_DISABLE_LOGGING
    #define BLT_TRACE(format, ...)
    #define BLT_DEBUG(format, ...)
    #define BLT_INFO(format, ...)
    #define BLT_WARN(format, ...)
    #define BLT_ERROR(format, ...)
    #define BLT_FATAL(format, ...)
#else
    /*#define BLT_TRACE(format, ...) log(format, blt::logging::TRACE, false, ##__VA_ARGS__);
    #define BLT_DEBUG(format, ...) log(format, blt::logging::DEBUG, false, ##__VA_ARGS__);
    #define BLT_INFO(format, ...) log(format, blt::logging::INFO, false, ##__VA_ARGS__);
    #define BLT_WARN(format, ...) log(format, blt::logging::WARN, false, ##__VA_ARGS__);
    #define BLT_ERROR(format, ...) log(format, blt::logging::ERROR, false, ##__VA_ARGS__);
    #define BLT_FATAL(format, ...) log(format, blt::logging::FATAL, false, ##__VA_ARGS__);
    
    #define BLT_TRACE_LN(format, ...) log(format, blt::logging::TRACE, true, ##__VA_ARGS__);
    #define BLT_DEBUG_LN(format, ...) log(format, blt::logging::DEBUG, true, ##__VA_ARGS__);
    #define BLT_INFO_LN(format, ...) log(format, blt::logging::INFO, true, ##__VA_ARGS__);
    #define BLT_WARN_LN(format, ...) log(format, blt::logging::WARN, true, ##__VA_ARGS__);
    #define BLT_ERROR_LN(format, ...) log(format, blt::logging::ERROR, true, ##__VA_ARGS__);
    #define BLT_FATAL_LN(format, ...) log(format, blt::logging::FATAL, true, ##__VA_ARGS__);*/
    
    #define BLT_TRACE(format, ...) log(format, blt::logging::TRACE, true, ##__VA_ARGS__);
    #define BLT_DEBUG(format, ...) log(format, blt::logging::DEBUG, true, ##__VA_ARGS__);
    #define BLT_INFO(format, ...) log(format, blt::logging::INFO, true, ##__VA_ARGS__);
    #define BLT_WARN(format, ...) log(format, blt::logging::WARN, true, ##__VA_ARGS__);
    #define BLT_ERROR(format, ...) log(format, blt::logging::ERROR, true, ##__VA_ARGS__);
    #define BLT_FATAL(format, ...) log(format, blt::logging::FATAL, true, ##__VA_ARGS__);
#endif

#endif //BLT_TESTS_LOGGING_H
