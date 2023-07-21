/*
 * Created by Brett on 23/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_LOGGING_H
#define BLT_LOGGING_H

#include <string>
#include <type_traits>
#include <blt/config.h>

namespace blt::logging {
    
    enum class LogLevel {
        // low level
        TRACE0, TRACE1, TRACE2, TRACE3,
        // normal
        TRACE, DEBUG, INFO,
        WARN,
        // errors
        ERROR, FATAL,
        // default
        NONE
    };
    
    class ILogHandler {
        public:
            virtual void log(std::string message, LogLevel level) = 0;
    };
    
    struct LOG_PROPERTIES {
        bool m_useColor = true;
        bool m_logToConsole = true;
        bool m_logToFile = true;
        // include file + line data?
        bool m_logWithData = true;
        // print the whole path or just the file name?
        bool m_logFullPath = false;
        const char* m_directory = "./";
        LogLevel minLevel = LogLevel::TRACE;
        
        explicit constexpr LOG_PROPERTIES(
                bool useColor, bool logToConsole, bool logToFile, const char* directory
        ):
                m_useColor(useColor), m_logToConsole(logToConsole), m_logToFile(logToFile),
                m_directory(directory) {}
        
        explicit constexpr LOG_PROPERTIES() = default;
    };
    
    struct logger {
        LogLevel level;
        
        void log_internal(const std::string& str) const;
        // evil hack, todo: better way
#ifdef BLT_DISABLE_LOGGING
        void log(const std::string& str) const {
        
        }
#else
        
        void log(const std::string& str) const {
            log_internal(str);
        }

#endif
        
        void flush() const;
        
        static void flush_all();
    };
    
    static logger std_out{LogLevel::NONE};
    
    static logger trace{LogLevel::TRACE};
    static logger debug{LogLevel::DEBUG};
    static logger info{LogLevel::INFO};
    static logger warn{LogLevel::WARN};
    static logger error{LogLevel::ERROR};
    static logger fatal{LogLevel::FATAL};
    
    static inline logger& getLoggerFromLevel(LogLevel level) {
        static logger loggerLevelDecode[11]{trace, trace, trace, trace, trace, debug, info, warn, error, fatal, std_out};
        return loggerLevelDecode[(int)level];
    }
    
    static inline logger& operator<<(logger& out, const std::string& str) {
        out.log(str);
        return out;
    }
    
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
    static inline logger& operator<<(logger& out, T t) {
        out.log(std::to_string(t));
        return out;
    }
    
    void init(LOG_PROPERTIES properties);
    
    void log_internal(
            const std::string& format, LogLevel level, const char* file, int currentLine,
            int auto_line, ...
    );
    
    // template voodoo magic (SFINAE, "Substitution Failure Is Not An Error")
    // https://stackoverflow.com/questions/44848011/c-limit-template-type-to-numbers
    // std::enable_if has a member called type if the condition is true.
    // What I am doing is saying that if the condition is true then the template has a non type
    // template parameter of type type* (which is void* since the default type for enable_if is void) and it's value is nullptr.
    // if the condition is false, the substitution fails, and the entire template is silently (no compiler error) discarded from the overload set.
    /**
     * Logs an is_arithmetic type to the console.
     * @tparam T type to log
     * @param t the value to log
     * @param level logger level to log at
     * @param auto_line put a new line at the end if none exists if true
     */
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
    inline void log_internal(
            T t, LogLevel level, const char* file, int currentLine, int auto_line
    ) {
        log_internal(std::to_string(t), level, file, currentLine, auto_line);
    }
    
    /**
     * Will flush all buffers! This might cause issues with threads!
     */
    void flush();
    
    void testLogging();
}

#ifndef BLT_ENABLE_LOGGING
    #define BLT_TRACE(format, ...)
    #define BLT_DEBUG(format, ...)
    #define BLT_INFO(format, ...)
    #define BLT_WARN(format, ...)
    #define BLT_ERROR(format, ...)
    #define BLT_FATAL(format, ...)
#else
    #ifndef BLT_DISABLE_LOGGING
        #define BLT_TRACE(format, ...) log_internal(format, blt::logging::LogLevel::TRACE, __FILE__, __LINE__, true, ##__VA_ARGS__)
        #define BLT_DEBUG(format, ...) log_internal(format, blt::logging::LogLevel::DEBUG, __FILE__, __LINE__, true, ##__VA_ARGS__)
        #define BLT_INFO(format, ...) log_internal(format, blt::logging::LogLevel::INFO, __FILE__, __LINE__, true, ##__VA_ARGS__)
        #define BLT_WARN(format, ...) log_internal(format, blt::logging::LogLevel::WARN, __FILE__, __LINE__, true, ##__VA_ARGS__)
        #define BLT_ERROR(format, ...) log_internal(format, blt::logging::LogLevel::ERROR, __FILE__, __LINE__, true, ##__VA_ARGS__)
        #define BLT_FATAL(format, ...) log_internal(format, blt::logging::LogLevel::FATAL, __FILE__, __LINE__, true, ##__VA_ARGS__)
    #endif
#endif

#endif //BLT_LOGGING_H
