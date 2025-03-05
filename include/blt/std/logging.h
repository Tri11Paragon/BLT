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
#include <sstream>
#include <blt/config.h>
#include <blt/std/types.h>
#include <iostream>
#include <cstdarg>

namespace blt::logging
{
    
    namespace ansi
    {
        inline auto ESC(std::string_view str)
        {
            return std::string("\033") += str;
        }
        
        inline const auto CUR_HOME = ESC("[H");
        
        inline auto CUR_MOVE(blt::size_t line, blt::size_t column)
        {
            return ESC("[{" + std::to_string(line) + "};{" + std::to_string(column) + "}H");
        }
        
        inline auto CUR_UP(blt::size_t lines)
        {
            return ESC("[" + std::to_string(lines) + "A");
        }
        
        inline auto CUR_DOWN(blt::size_t lines)
        {
            return ESC("[" + std::to_string(lines) + "B");
        }
        
        inline auto CUR_RIGHT(blt::size_t columns)
        {
            return ESC("[" + std::to_string(columns) + "C");
        }
        
        inline auto CUR_LEFT(blt::size_t columns)
        {
            return ESC("[" + std::to_string(columns) + "D");
        }
        
        inline auto CUR_BEGIN_NEXT(blt::size_t lines_down)
        {
            return ESC("[" + std::to_string(lines_down) + "E");
        }
        
        inline auto CUR_BEGIN_PREV(blt::size_t lines_up)
        {
            return ESC("[" + std::to_string(lines_up) + "F");
        }
        
        inline auto CUR_COLUMN(blt::size_t column)
        {
            return ESC("[" + std::to_string(column) + "G");
        }
        
        inline auto CUR_POS()
        {
            return ESC("[6n");
        }
        
        inline auto CUR_SCROLL_UP()
        {
            return ESC(" M");
        }
        
        inline auto CUR_SAVE_DEC()
        {
            return ESC(" 7");
        }
        
        inline auto CUR_LOAD_DEC()
        {
            return ESC(" 8");
        }
        
        inline auto CUR_SAVE_SCO()
        {
            return ESC("[s");
        }
        
        inline auto CUR_LOAD_SCO()
        {
            return ESC("[u");
        }
        
        inline auto RESET = ESC("[0m");
        inline auto BOLD = "1";
        inline auto RESET_BOLD = "22";
        inline auto DIM = "2";
        inline auto RESET_DIM = "22";
        inline auto ITALIC = "3";
        inline auto RESET_ITALIC = "23";
        inline auto UNDERLINE = "4";
        inline auto RESET_UNDERLINE = "24";
        inline auto BLINKING = "5";
        inline auto RESET_BLINKING = "25";
        inline auto INVERSE = "7";
        inline auto RESET_INVERSE = "27";
        inline auto HIDDEN = "8";
        inline auto RESET_HIDDEN = "28";
        inline auto STRIKETHROUGH = "9";
        inline auto RESET_STRIKETHROUGH = "29";
        
        inline auto COLOR_DEFAULT = "39";
        inline auto BACKGROUND_DEFAULT = "49";
        
        inline auto BLACK = "30";
        inline auto RED = "31";
        inline auto GREEN = "32";
        inline auto YELLOW = "33";
        inline auto BLUE = "34";
        inline auto MAGENTA = "35";
        inline auto CYAN = "36";
        inline auto WHITE = "37";
        inline auto BLACK_BACKGROUND = "40";
        inline auto RED_BACKGROUND = "41";
        inline auto GREEN_BACKGROUND = "42";
        inline auto YELLOW_BACKGROUND = "43";
        inline auto BLUE_BACKGROUND = "44";
        inline auto MAGENTA_BACKGROUND = "45";
        inline auto CYAN_BACKGROUND = "46";
        inline auto WHITE_BACKGROUND = "47";
        
        inline auto BRIGHT_BLACK = "90";
        inline auto BRIGHT_RED = "91";
        inline auto BRIGHT_GREEN = "92";
        inline auto BRIGHT_YELLOW = "93";
        inline auto BRIGHT_BLUE = "94";
        inline auto BRIGHT_MAGENTA = "95";
        inline auto BRIGHT_CYAN = "96";
        inline auto BRIGHT_WHITE = "97";
        inline auto BRIGHT_BLACK_BACKGROUND = "100";
        inline auto BRIGHT_RED_BACKGROUND = "101";
        inline auto BRIGHT_GREEN_BACKGROUND = "102";
        inline auto BRIGHT_YELLOW_BACKGROUND = "103";
        inline auto BRIGHT_BLUE_BACKGROUND = "104";
        inline auto BRIGHT_MAGENTA_BACKGROUND = "105";
        inline auto BRIGHT_CYAN_BACKGROUND = "106";
        inline auto BRIGHT_WHITE_BACKGROUND = "107";
        
        template<typename... Args>
        inline auto make_color(Args... colors)
        {
            std::string mode;
            ((mode += std::string(colors) + ";"), ...);
            return ESC("[" + mode.substr(0, mode.size() - 1) + "m");
        }
    }
    
    enum class log_level
    {
        // default
        NONE,
        // low level
        TRACE0, TRACE1, TRACE2, TRACE3,
        // normal
        TRACE, DEBUG, INFO,
        // errors
        WARN, ERROR, FATAL,
    };
    
    struct tag_func_param
    {
        blt::logging::log_level level;
        const std::string& file, line, raw_string, formatted_string;
    };
    
    struct tag
    {
        // tag without the ${{ or }}
        std::string tag;
        // function to run: log level, file, line, and raw user input string are provided
        std::function<std::string(const tag_func_param&)> func;
    };
    
    struct log_format
    {
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
         *  - ${{MS}}           // current unix time
         *  - ${{NS}}           // current ns from the high resolution system timer
         *  - ${{ISO_YEAR}}     // ISO formatted 'year-month-day' in a single variable
         *  - ${{TIME}}         // 'hour:minute:second' formatted string in a single variable
         *  - ${{FULL_TIME}}    // 'year-month-day hour:minute:second' in a single variable
         *  - ${{LF}}           // log level color (ANSI color code)
         *  - ${{ER}}           // Error red
         *  - ${{CNR}}          // conditional error red (only outputs if log level is an error!)
         *  - ${{RC}}           // ANSI color reset
         *  - ${{LOG_LEVEL}}    // current log level
         *  - ${{THREAD_NAME}}  // current thread name, NOTE: thread names must be set by calling "setThreadName()" from the thread in question!
         *  - ${{FILE}}         // file from which the macro is invoked
         *  - ${{LINE}}         // line in the from which the macro is invoked
         *  - ${{RAW_STR}}      // raw user string without formatting applied (NOTE: format args are not provided!)
         *  - ${{STR}}          // the user supplied string (format applied!)
         */
        std::string logOutputFormat = "\033[94m[${{TIME}}]${{RC}} ${{LF}}[${{LOG_LEVEL}}]${{RC}} \033[35m(${{FILE}}:${{LINE}})${{RC}} ${{CNR}}${{STR}}${{RC}}\n";
        std::string levelNames[11] = {"STDOUT", "TRACE0", "TRACE1", "TRACE2", "TRACE3", "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
        std::string levelColors[11] = {"\033[0m", "\033[22;97m", "\033[97m", "\033[97m", "\033[97m", "\033[97m", "\033[36m", "\033[92m", "\033[93m",
                                       "\033[91m", "\033[97;41m"};
        // if true prints the whole path to the file (eg /home/user/.../.../project/src/source.cpp:line#)
        bool printFullFileName = false;
        // the logging lib will keep track of the largest line found so far and try to keep the spacing accordingly
        // this is not thread safe!
        bool ensureAlignment = false;
        // should we log to file?
        bool logToFile = false;
        // should we log to console?
        bool logToConsole = true;
        // where should we log? (empty for current binary directory) should end in a / if not empty!
        std::string logFilePath;
        // logs to a file called $fileName_$count.log where count is the number of rollover files
        // this accepts any of the macros above, using level names and colors should work but isn't supported.
        std::string logFileName = "${{ISO_YEAR}}";
        // default limit on file size: 10mb;
        size_t logMaxFileSize = 1024 * 1024 * 10;
        /**
         * Variables below this line should never be changed by the user!
         */
        // the current alignment width found (you shouldn't chance this variable!)
        size_t currentWidth = 0;
        // current number of file roll-overs. you shouldn't change this either.
        size_t currentRollover = 0;
        std::string lastFile;
    };
    
    struct logger
    {
        log_level level;
        const char* file;
        int line;
    };
    
    struct empty_logger
    {
    
    };
    
    void log_internal(const std::string& format, log_level level, const char* file, int line, std::va_list& args);
    
    void log_stream_internal(const std::string& str, const logger& logger);
    
    template<typename T>
    inline std::string to_string_stream(const T& t)
    {
        std::stringstream stream;
        stream << t;
        return stream.str();
    }
    
    template<typename T>
    inline static void log_stream(const T& t, const logger& logger)
    {
        if constexpr (std::is_arithmetic_v<T> && !std::is_same_v<T, char>)
        {
            log_stream_internal(std::to_string(t), logger);
        } else if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, const char*>)
        {
            log_stream_internal(t, logger);
        } else
        {
            log_stream_internal(to_string_stream(t), logger);
        }
    }
    
    template<typename T>
    inline void log(T t, log_level level, const char* file, int line, ...)
    {
        std::va_list args;
        va_start(args, line);
        if constexpr (std::is_arithmetic_v<T>)
        {
            log_internal(std::to_string(t), level, file, line, args);
        } else if constexpr (std::is_same_v<T, std::string>)
        {
            log_internal(t, level, file, line, args);
        } else if constexpr (std::is_same_v<T, const char*>)
        {
            log_internal(std::string(t), level, file, line, args);
        } else
        {
            log_internal(to_string_stream(t), level, file, line, args);
        }
        va_end(args);
    }
    
    template<typename T>
    static inline const blt::logging::logger& operator<<(const blt::logging::logger& out, const T& t)
    {
        log_stream(t, out);
        return out;
    }
    
    template<typename T>
    static inline const blt::logging::empty_logger& operator<<(const blt::logging::empty_logger& out, const T&)
    {
        return out;
    }
    
    void flush();
    
    void newline();
    
    void setThreadName(const std::string& name);
    
    void setLogFormat(const log_format& format);
    
    void setLogColor(log_level level, const std::string& newFormat);
    
    void setLogName(log_level level, const std::string& newFormat);
    
    void setLogOutputFormat(const std::string& newFormat);
    
    void setLogToFile(bool shouldLogToFile);
    
    void setLogToConsole(bool shouldLogToConsole);
    
    void setLogPath(const std::string& path);
    
    void setLogFileName(const std::string& fileName);
    
    void setMaxFileSize(size_t fileSize);
}

#if defined(__clang__) || defined(__llvm__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif

#ifdef BLT_DISABLE_LOGGING
    #define BLT_LOG(format, level, ...)
    #define BLT_LOG_STREAM(level)
    #define BLT_TRACE0_STREAM
    #define BLT_TRACE1_STREAM
    #define BLT_TRACE2_STREAM
    #define BLT_TRACE3_STREAM
    #define BLT_TRACE_STREAM
    #define BLT_DEBUG_STREAM
    #define BLT_INFO_STREAM
    #define BLT_WARN_STREAM
    #define BLT_ERROR_STREAM
    #define BLT_FATAL_STREAM
    #define BLT_TRACE(format, ...)
    #define BLT_DEBUG(format, ...)
    #define BLT_INFO(format, ...)
    #define BLT_WARN(format, ...)
    #define BLT_ERROR(format, ...)
    #define BLT_FATAL(format, ...)
#else
    #define BLT_NEWLINE() blt::logging::newline()
    #define BLT_LOG(format, level, ...) blt::logging::log(format, level, __FILE__, __LINE__, ##__VA_ARGS__)
    #define BLT_LOG_STREAM(level) blt::logging::logger{level, __FILE__, __LINE__}
    #ifdef BLT_DISABLE_TRACE
        #define BLT_TRACE(format, ...)
        #define BLT_TRACE0_STREAM blt::logging::empty_logger{}
        #define BLT_TRACE1_STREAM blt::logging::empty_logger{}
        #define BLT_TRACE2_STREAM blt::logging::empty_logger{}
        #define BLT_TRACE3_STREAM blt::logging::empty_logger{}
        #define BLT_TRACE_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_TRACE(format, ...) BLT_LOG(format, blt::logging::log_level::TRACE, ##__VA_ARGS__)
        #define BLT_TRACE0_STREAM BLT_LOG_STREAM(blt::logging::log_level::TRACE0)
        #define BLT_TRACE1_STREAM BLT_LOG_STREAM(blt::logging::log_level::TRACE1)
        #define BLT_TRACE2_STREAM BLT_LOG_STREAM(blt::logging::log_level::TRACE2)
        #define BLT_TRACE3_STREAM BLT_LOG_STREAM(blt::logging::log_level::TRACE3)
        #define BLT_TRACE_STREAM BLT_LOG_STREAM(blt::logging::log_level::TRACE)
    #endif
    
    #ifdef BLT_DISABLE_DEBUG
        #define BLT_DEBUG(format, ...)
        #define BLT_DEBUG_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_DEBUG(format, ...) BLT_LOG(format, blt::logging::log_level::DEBUG, ##__VA_ARGS__)
        #define BLT_DEBUG_STREAM BLT_LOG_STREAM(blt::logging::log_level::DEBUG)
    #endif
    
    #ifdef BLT_DISABLE_INFO
        #define BLT_INFO(format, ...)
        #define BLT_INFO_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_INFO(format, ...) BLT_LOG(format, blt::logging::log_level::INFO, ##__VA_ARGS__)
        #define BLT_INFO_STREAM BLT_LOG_STREAM(blt::logging::log_level::INFO)
    #endif
    
    #ifdef BLT_DISABLE_WARN
        #define BLT_WARN(format, ...)
        #define BLT_WARN_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_WARN(format, ...) BLT_LOG(format, blt::logging::log_level::WARN, ##__VA_ARGS__)
        #define BLT_WARN_STREAM BLT_LOG_STREAM(blt::logging::log_level::WARN)
    #endif
    
    #ifdef BLT_DISABLE_ERROR
        #define BLT_ERROR(format, ...)
        #define BLT_ERROR_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_ERROR(format, ...) BLT_LOG(format, blt::logging::log_level::ERROR, ##__VA_ARGS__)
        #define BLT_ERROR_STREAM BLT_LOG_STREAM(blt::logging::log_level::ERROR)
    #endif
    
    #ifdef BLT_DISABLE_FATAL
        #define BLT_FATAL(format, ...)
        #define BLT_FATAL_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_FATAL(format, ...) BLT_LOG(format, blt::logging::log_level::FATAL, ##__VA_ARGS__)
        #define BLT_FATAL_STREAM BLT_LOG_STREAM(blt::logging::log_level::FATAL)
    #endif
#endif

#if defined(__clang__) || defined(__llvm__)
#pragma clang diagnostic pop
#endif

#endif //BLT_TESTS_LOGGING2_H
