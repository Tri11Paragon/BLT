/*
 * Created by Brett on 23/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/std/logging.h>
#include <blt/std/time.h>
#include <cstdarg>
#include <string>
#include <iostream>
#include "blt/std/string.h"
#include <fstream>
#include <unordered_map>
#include <ios>
#include <thread>
#include <filesystem>
#include <blt/std/memory.h>

// https://en.cppreference.com/w/cpp/utility/variadic
// https://medium.com/swlh/variadic-functions-3419c287a0d2
// https://publications.gbdirect.co.uk//c_book/chapter9/stdarg.html
// https://cplusplus.com/reference/cstdio/printf/


namespace blt::logging {
    
    class LogFileWriter {
        private:
            std::string m_path;
            std::fstream* output;
            int currentLines = 0;
            static constexpr int MAX_LINES = 100000;
        public:
            explicit LogFileWriter(const std::string& path): m_path(path){
                auto currentTime = System::getTimeStringFS();
                output = new std::fstream(path + currentTime + ".log", std::ios::out | std::ios::app);
                if (!output->good()){
                    throw std::runtime_error("Unable to open console filestream!\n");
                }
            }
            
            void writeLine(const std::string& line){
                if (!output->good()){
                    std::cerr << "There has been an error in the logging file stream!\n";
                    output->clear();
                }
                *output << line;
                currentLines++;
                if (currentLines > MAX_LINES){
                    output->flush();
                    output->close();
                    delete(output);
                    currentLines = 0;
                    auto currentTime = System::getTimeStringFS();
                    output = new std::fstream(m_path + currentTime + ".log");
                }
            }
            
            ~LogFileWriter() {
                delete(output);
            }
    };
    
    void applyFormatting(const std::string& format, std::string& output, va_list& args){
        // args must be copied because they will be consumed by the first vsnprintf
        va_list args_copy;
        va_copy(args_copy, args);
        
        auto buffer_size = std::vsnprintf(nullptr, 0, format.c_str(), args_copy) + 1;
        // some compilers don't even allow you to do stack dynamic arrays. So always allocate on the heap.
        // originally if the buffer was small enough the buffer was allocated on the stack because it made no sense to make a heap object
        // that will be deleted a couple lines later.
        scoped_buffer<char> buffer{static_cast<unsigned long>(buffer_size)};
        vsnprintf(*buffer, buffer_size, format.c_str(), args);
        output = std::string(*buffer);
        
        va_end(args_copy);
    }
    
    const char* levelColors[6] = {
            "\033[97m",
            "\033[36m",
            "\033[92m",
            "\033[93m",
            "\033[91m",
            "\033[97;41m"
    };
    
    const char* levelNames[6] = {
            "[TRACE]: ",
            "[DEBUG]: ",
            "[INFO]:  ",
            "[WARN]:  ",
            "[ERROR]: ",
            "[FATAL]: ",
    };
    
    // by default everything is enabled
    LOG_PROPERTIES BLT_LOGGING_PROPERTIES {};
    LogFileWriter writer{"./"};
    
    void init(LOG_PROPERTIES properties) {
        if (BLT_LOGGING_PROPERTIES.m_directory != properties.m_directory)
            writer = LogFileWriter{properties.m_directory};
        if (properties.m_logToFile)
            std::filesystem::create_directory(properties.m_directory);
        BLT_LOGGING_PROPERTIES = properties;
    }
    
    inline void log(const std::string& str, bool hasEndingLinefeed, LOG_LEVEL level, int auto_line){
        if (level < BLT_LOGGING_PROPERTIES.minLevel)
            return;
        std::string outputString = System::getTimeStringLog();
        outputString += levelNames[level];
        outputString += str;
        
        std::string fileString = outputString;
    
        if (BLT_LOGGING_PROPERTIES.m_useColor) {
            outputString = levelColors[level] + outputString;
            outputString += "\033[0m";
        }
    
        if (hasEndingLinefeed || auto_line) {
            outputString += "\n";
            fileString += "\n";
        }
    
        if (BLT_LOGGING_PROPERTIES.m_logToConsole) {
            if (level > WARN)
                std::cerr << outputString;
            else
                std::cout << outputString;
        }
        
        if (BLT_LOGGING_PROPERTIES.m_logToFile) {
            writer.writeLine(fileString);
        }
    }
    
    void log(const std::string& format, LOG_LEVEL level, int auto_line, ...) {
        va_list args;
        va_start(args, auto_line);
        
        std::string formattedString;
        applyFormatting(format, formattedString, args);
        
        bool hasEndingLinefeed = formattedString[formattedString.length()-1] == '\n';
        
        if (hasEndingLinefeed)
            formattedString = formattedString.substr(0, formattedString.length()-1);
    
        log(formattedString, hasEndingLinefeed, level, auto_line);
    
        va_end(args);
    }
    
    void log(int i, LOG_LEVEL level, int auto_line) {
        log(std::to_string(i), false, level, true);
    }
    
    void log(long i, LOG_LEVEL level, int auto_line) {
        log(std::to_string(i), false, level, true);
    }
    
    void log(unsigned int i, LOG_LEVEL level, int auto_line) {
        log(std::to_string(i), false, level, true);
    }
    
    void log(unsigned long i, LOG_LEVEL level, int auto_line) {
        log(std::to_string(i), false, level, true);
    }
    
    void log(char i, LOG_LEVEL level, int auto_line) {
        log(std::to_string(i), false, level, true);
    }
    
    void log(unsigned char i, LOG_LEVEL level, int auto_line) {
        log(std::to_string(i), false, level, true);
    }
    
    void log(short i, LOG_LEVEL level, int auto_line) {
        log(std::to_string(i), false, level, true);
    }
    
    void log(unsigned short i, LOG_LEVEL level, int auto_line) {
        log(std::to_string(i), false, level, true);
    }
    
    void log(float f, LOG_LEVEL level, int auto_line) {
        log(std::to_string(f), false, level, true);
    }
    
    void log(double f, LOG_LEVEL level, int auto_line) {
        log(std::to_string(f), false, level, true);
    }
    
    std::unordered_map<std::thread::id, std::string> thread_local_strings;
    
    void logger::logi(const std::string& str) const {
        auto id = std::this_thread::get_id();
        auto th_str = thread_local_strings[id];
        th_str += str;
        
        if (blt::string::contains(str, "\n")){
            // make sure new lines are properly formatted to prevent danging lines. Ie "[trace]: .... [debug]: ...."
            bool hasEndingLinefeed = str[str.length()-1] == '\n';
            logging::log(th_str, false, level, !hasEndingLinefeed);
            thread_local_strings[id] = "";
        } else {
            thread_local_strings[id] = th_str;
        }
    }
    
    void logger::flush() const {
        for (const auto& id : thread_local_strings) {
            auto th_str = id.second;
            bool hasEndingLinefeed = th_str[th_str.length() - 1] == '\n';
            logging::log(th_str, false, level, !hasEndingLinefeed);
            thread_local_strings[id.first] = "";
        }
    }
    
    void flush() {
        // TODO: this will prevent proper level output. Please fixme
        tlog.flush();
        dlog.flush();
        ilog.flush();
        wlog.flush();
        elog.flush();
        flog.flush();
        trace.flush();
        debug.flush();
        info.flush();
        warn.flush();
        error.flush();
        fatal.flush();
        std::cerr.flush();
        std::cout.flush();
    }
}


