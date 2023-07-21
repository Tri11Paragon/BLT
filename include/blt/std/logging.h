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
#include <iostream>

namespace blt::logging {
    
    template<typename K, typename V>
    using hashmap = std::unordered_map<K, V>;
    
    enum class log_level {
        // default
        NONE,
        // low level
        TRACE0, TRACE1, TRACE2, TRACE3,
        // normal
        TRACE, DEBUG, INFO,
        // errors
        WARN, ERROR, FATAL,
    };
    
    struct tag_func_param {
        blt::logging::log_level level;
        const std::string& file, line, raw_string, formatted_string;
    };
    
    struct tag {
        // tag without the ${{ or }}
        std::string tag;
        // function to run: log level, file, line, and raw user input string are provided
        std::function<std::string(const tag_func_param&)> func;
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
        std::string logOutputFormat = "[${{HOUR}}:${{MINUTE}}:${{SECOND}] ${{LF}}[${{LOG_LEVEL}}]${{R}} ${{CNR}}${{STR}}";
        std::string levelNames[10] = {"STDOUT", "TRACE0, TRACE1, TRACE2", "TRACE3", "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};
        std::string levelColors[10] = {"\033[0m", "\033[22;97m", "\033[97m", "\033[97m", "\033[97m", "\033[36m", "\033[92m", "\033[93m", "\033[91m", "\033[97;41m"};
    };
    
    struct logger {
        log_level level;
        std::string file;
        std::string line;
    };
    
    struct empty_logger {
    
    };
    
    void log(const std::string& format, log_level level, const char* file, int line, ...);
    void log_stream(const std::string& str, const logger& logger);
    
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
    inline void log(T t, log_level level, const char* file, int line) {
        log(std::to_string(t), level, file, line);
    }
    
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
    inline void log_stream(T t, const logger& logger) {
        log_stream(std::to_string(t), logger);
    }
    
    static inline const blt::logging::logger& operator<<(const blt::logging::logger& out, const std::string& str) {
        log_stream(str, out);
        return out;
    }
    
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
    static inline const blt::logging::logger& operator<<(const blt::logging::logger& out, T t) {
        log_stream(std::to_string(t), out);
        return out;
    }

    static inline const blt::logging::empty_logger& operator<<(const blt::logging::empty_logger& out, const std::string&) {
        return out;
    }
    
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
    static inline const blt::logging::empty_logger& operator<<(const blt::logging::empty_logger& out, T) {
        return out;
    }
    
    void flush();
    
    void setThreadName(const std::string& name);
}

#define BLT_LOGGING_IMPLEMENTATION
#ifdef BLT_LOGGING_IMPLEMENTATION

    #include <iostream>
    #include <chrono>
    #include <ctime>
    #include <unordered_map>
    #include <thread>

namespace blt::logging {
    
    /**
     * Used to store fast associations between built in tags and their respective values
     */
    class tag_map {
        private:
            tag* tags;
            size_t size;
            
            [[nodiscard]] inline size_t hash(const tag& t) const {
                size_t h = t.tag[0]+ t.tag[1] * 3;
                return h;
            }
            
            inline void expand(){
                auto newSize = size * 2;
                tag* newTags = new tag[newSize];
                for (size_t i = 0; i < size; i++)
                    newTags[i] = tags[i];
                delete[] tags;
                tags = newTags;
                size = newSize;
            }
        public:
            tag_map(std::initializer_list<tag> initial_tags){
                tags = new tag[(size = 16)];
                for (auto& t : initial_tags)
                    insert(t);
            }
            
            tag_map& insert(const tag& t){
                auto h = hash(t);
                if (h > size)
                    expand();
                if (!tags[h].tag.empty())
                    std::cerr << "Tag not empty! " << tags[h].tag << "!!!\n";
                tags[h] = t;
                return *this;
            }
            
            tag& operator[](const std::string& name){
                auto h = hash(tag{name, nullptr});
                if (h > size)
                    std::cerr << "Tag out of bounds";
                return tags[h];
            }
            
            ~tag_map(){
                delete[] tags;
            }
    };
    
    #define BLT_NOW() auto t = std::time(nullptr); auto now = std::localtime(&t)
    #define BLT_ISO_YEAR(S) auto S = std::to_string(now->tm_year); \
        S += '-'; \
        S += std::to_string(now->tm_mon); \
        S += '-'; \
        S += std::to_string(now->tm_mday);
    #define BLT_CUR_TIME(S) auto S = std::to_string(now->tm_hour); \
        S += '-'; \
        S += std::to_string(now->tm_min); \
        S += '-'; \
        S += std::to_string(now->tm_sec);
    
    static inline std::string ensureHasDigits(int current, int digits) {
        std::string asString = std::to_string(current);
        auto length = digits - asString.length();
        if (length <= 0)
            return asString;
        std::string zeros;
        zeros.reserve(length);
        for (unsigned int i = 0; i < length; i++){
            zeros += '0';
        }
        return zeros + asString;
    }

    log_format loggingFormat {};
    hashmap<std::thread::id, std::string> loggingThreadNames;
    hashmap<std::thread::id, hashmap<blt::logging::log_level, std::string>> loggingStreamLines;
    
    const tag_map tagMap = {
            {"YEAR", [](const tag_func_param&) -> std::string {
                BLT_NOW();
                return std::to_string(now->tm_year);
            }},
            {"MONTH", [](const tag_func_param&) -> std::string {
                BLT_NOW();
                return ensureHasDigits(now->tm_mon, 2);
            }},
            {"DAY", [](const tag_func_param&) -> std::string {
                BLT_NOW();
                return ensureHasDigits(now->tm_mday, 2);
            }},
            {"HOUR", [](const tag_func_param&) -> std::string {
                BLT_NOW();
                return ensureHasDigits(now->tm_hour, 2);
            }},
            {"MINUTE", [](const tag_func_param&) -> std::string {
                BLT_NOW();
                return ensureHasDigits(now->tm_min, 2);
            }},
            {"SECOND", [](const tag_func_param&) -> std::string {
                BLT_NOW();
                return ensureHasDigits(now->tm_sec, 2);
            }},
            {"MS", [](const tag_func_param&) -> std::string {
                return std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::high_resolution_clock::now().time_since_epoch()).count()
                );
            }},
            {"NS", [](const tag_func_param&) -> std::string {
                return std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(
                        std::chrono::high_resolution_clock::now().time_since_epoch()).count()
                );
            }},
            {"ISO_YEAR", [](const tag_func_param&) -> std::string {
                BLT_NOW();
                BLT_ISO_YEAR(returnStr);
                return returnStr;
            }},
            {"TIME", [](const tag_func_param&) -> std::string {
                BLT_NOW();
                BLT_CUR_TIME(returnStr);
                return returnStr;
            }},
            {"FULL_TIME", [](const tag_func_param&) -> std::string {
                BLT_NOW();
                BLT_ISO_YEAR(ISO);
                BLT_CUR_TIME(TIME);
                ISO += ' ';
                ISO += TIME;
                return ISO;
            }},
            {"LF", [](const tag_func_param& f) -> std::string {
                return loggingFormat.levelColors[(int)f.level];
            }},
            {"ER", [](const tag_func_param&) -> std::string {
                return loggingFormat.levelColors[(int)log_level::ERROR];
            }},
            {"CNR", [](const tag_func_param& f) -> std::string {
                return f.level >= log_level::ERROR ? loggingFormat.levelColors[(int)log_level::ERROR] : "";
            }},
            {"RC", [](const tag_func_param&) -> std::string {
                return "\033[0m";
            }},
            {"LOG_LEVEL", [](const tag_func_param& f) -> std::string {
                return loggingFormat.levelNames[(int)f.level];
            }},
            {"THREAD_NAME", [](const tag_func_param&) -> std::string {
                if (loggingThreadNames.find(std::this_thread::get_id()) == loggingThreadNames.end())
                    return "UNKNOWN";
                return loggingThreadNames[std::this_thread::get_id()];
            }},
            {"FILE", [](const tag_func_param& f) -> std::string {
                return f.file;
            }},
            {"LINE", [](const tag_func_param& f) -> std::string {
                return f.line;
            }},
            {"RAW_STR", [](const tag_func_param& f) -> std::string {
                return f.raw_string;
            }},
            {"STR", [](const tag_func_param& f) -> std::string {
                return f.formatted_string;
            }},
    };
    
    void log(const std::string& format, log_level level, const char* file, int line, ...) {
        
    }
    
    void log_stream(const std::string& str, const logger& logger) {
        loggingStreamLines[]
    }
    
    void setThreadName(const std::string& name) {
        loggingThreadNames[std::this_thread::get_id()] = name;
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
        #define BLT_TRACE0_STREAM blt::logging::empty_logger{}
        #define BLT_TRACE1_STREAM blt::logging::empty_logger{}
        #define BLT_TRACE2_STREAM blt::logging::empty_logger{}
        #define BLT_TRACE3_STREAM blt::logging::empty_logger{}
        #define BLT_TRACE_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_TRACE(format, ...) BLT_LOG(format, blt::logging::log_level::TRACE, ##__VA_ARGS__)
        #define BLT_TRACE0_STREAM blt::logging::logger{blt::logging::log_level::TRACE0, std::string(__FILE__), std::to_string(__LINE__)}
        #define BLT_TRACE1_STREAM blt::logging::logger{blt::logging::log_level::TRACE1, std::string(__FILE__), std::to_string(__LINE__)}
        #define BLT_TRACE2_STREAM blt::logging::logger{blt::logging::log_level::TRACE2, std::string(__FILE__), std::to_string(__LINE__)}
        #define BLT_TRACE3_STREAM blt::logging::logger{blt::logging::log_level::TRACE3, std::string(__FILE__), std::to_string(__LINE__)}
        #define BLT_TRACE_STREAM blt::logging::logger{blt::logging::log_level::TRACE, std::string(__FILE__), std::to_string(__LINE__)}
    #endif
    
    #ifndef BLT_ENABLE_DEBUG
        #define BLT_DEBUG(format, ...)
        #define BLT_DEBUG_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_DEBUG(format, ...) BLT_LOG(format, blt::logging::log_level::DEBUG, ##__VA_ARGS__)
        #define BLT_DEBUG_STREAM blt::logging::logger{blt::logging::log_level::DEBUG, std::string(__FILE__), std::to_string(__LINE__)}
    #endif
    
    #ifndef BLT_ENABLE_INFO
        #define BLT_INFO(format, ...)
        #define BLT_INFO_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_INFO(format, ...) BLT_LOG(format, blt::logging::log_level::INFO, ##__VA_ARGS__)
        #define BLT_INFO_STREAM blt::logging::logger{blt::logging::log_level::INFO, std::string(__FILE__), std::to_string(__LINE__)}
    #endif
    
    #ifndef BLT_ENABLE_WARN
        #define BLT_WARN(format, ...)
        #define BLT_WARN_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_WARN(format, ...) BLT_LOG(format, blt::logging::log_level::WARN, ##__VA_ARGS__)
        #define BLT_WARN_STREAM blt::logging::logger{blt::logging::log_level::WARN, std::string(__FILE__), std::to_string(__LINE__)}
    #endif
    
    #ifndef BLT_ENABLE_ERROR
        #define BLT_ERROR(format, ...)
        #define BLT_ERROR_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_ERROR(format, ...) BLT_LOG(format, blt::logging::log_level::ERROR, ##__VA_ARGS__)
        #define BLT_ERROR_STREAM blt::logging::logger{blt::logging::log_level::ERROR, std::string(__FILE__), std::to_string(__LINE__)}
    #endif
    
    #ifndef BLT_ENABLE_FATAL
        #define BLT_FATAL(format, ...)
        #define BLT_FATAL_STREAM blt::logging::empty_logger{}
    #else
        #define BLT_FATAL(format, ...) BLT_LOG(format, blt::logging::log_level::FATAL, ##__VA_ARGS__)
        #define BLT_FATAL_STREAM blt::logging::logger{blt::logging::log_level::FATAL, std::string(__FILE__), std::to_string(__LINE__)}
    #endif
#endif

#endif //BLT_TESTS_LOGGING2_H
