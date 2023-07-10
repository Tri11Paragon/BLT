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
#include "blt/std/format.h"
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
                auto currentTime = system::getTimeStringFS();
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
                    currentLines = 0;
                    auto currentTime = system::getTimeStringFS();
                    delete(output);
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
            "[BLT_TRACE]: ",
            "[BLT_DEBUG]: ",
            "[BLT_INFO]:  ",
            "[BLT_WARN]:  ",
            "[BLT_ERROR]: ",
            "[BLT_FATAL]: ",
    };
    
    // by default everything is enabled
    LOG_PROPERTIES BLT_LOGGING_PROPERTIES {};
    LogFileWriter* writer = new LogFileWriter{"./"};
    
    void init(LOG_PROPERTIES properties) {
        if (properties.m_logToFile && BLT_LOGGING_PROPERTIES.m_directory != properties.m_directory) {
            delete(writer);
            writer = new LogFileWriter{properties.m_directory};
        }
        if (properties.m_logToFile)
            std::filesystem::create_directory(properties.m_directory);
        BLT_LOGGING_PROPERTIES = properties;
    }
    
    inline std::string filename(const std::string& path){
        if (BLT_LOGGING_PROPERTIES.m_logFullPath)
            return path;
        auto paths = blt::string::split(path, "/");
        auto final = paths[paths.size()-1];
        if (final == "/")
            return paths[paths.size()-2];
        return final;
    }
    
    inline void log(const std::string& str, bool hasEndingLinefeed, LOG_LEVEL level, const char* file, int currentLine, int auto_line){
        if (level < BLT_LOGGING_PROPERTIES.minLevel)
            return;
        std::string outputString = system::getTimeStringLog();
        
        if (BLT_LOGGING_PROPERTIES.m_logWithData && currentLine >= 0) {
            outputString +=  '[';
            outputString += filename(file);
            outputString += ':';
            outputString += std::to_string(currentLine);
            outputString += "] ";
        }
        
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
            if (level > BLT_WARN)
                std::cerr << outputString;
            else
                std::cout << outputString;
        }
        
        if (BLT_LOGGING_PROPERTIES.m_logToFile) {
            writer->writeLine(fileString);
        }
    }
    
    void log_internal(const std::string& format, LOG_LEVEL level, const char* file, int currentLine, int auto_line, ...) {
        va_list args;
        va_start(args, auto_line);
        
        std::string formattedString;
        applyFormatting(format, formattedString, args);

        bool hasEndingLinefeed = false;

        if (formattedString.length() > 0)
            hasEndingLinefeed = formattedString[formattedString.length() - 1] == '\n';
        
        if (hasEndingLinefeed)
            formattedString = formattedString.substr(0, formattedString.length()-1);
    
        log(formattedString, hasEndingLinefeed, level, file, currentLine, auto_line);
    
        va_end(args);
    }
    
    // stores an association between thread -> log level -> current line buffer
    std::unordered_map<std::thread::id, std::unordered_map<LOG_LEVEL, std::string>> thread_local_strings;
    
    void logger::log_internal(const std::string& str) const {
        auto id = std::this_thread::get_id();
        auto th_str = thread_local_strings[id][level];
        th_str += str;
        
        if (blt::string::contains(str, "\n")){
            // make sure new lines are properly formatted to prevent danging lines. Ie "[trace]: .... [debug]: ...."
            bool hasEndingLinefeed = str[str.length()-1] == '\n';
            if (level == BLT_NONE) {
                std::cout << th_str;
            } else
                logging::log(th_str, false, level, "", -1, !hasEndingLinefeed);
            thread_local_strings[id][level] = "";
        } else {
            thread_local_strings[id][level] = th_str;
        }
    }
    
    void flushLogger_internal(std::thread::id id, LOG_LEVEL level){
        auto th_str = thread_local_strings[id][level];
        if (th_str.empty())
            return;
        bool hasEndingLinefeed = th_str[th_str.length() - 1] == '\n';
        logging::log(th_str, false, level, "", -1, !hasEndingLinefeed);
        thread_local_strings[id][level] = "";
    }
    
    void logger::flush() const {
        for (const auto& id : thread_local_strings) {
            flushLogger_internal(id.first, level);
        }
    }
    
    void logger::flush_all() {
        for (const auto& id : thread_local_strings) {
            for (const auto& level : thread_local_strings[id.first]){
                flushLogger_internal(id.first, level.first);
            }
        }
    }
    
    void flush() {
        logger::flush_all();
        std::cerr.flush();
        std::cout.flush();
    }
    
    void testLogging() {
        trace << "Trace Test!\n";
        debug << "Debug Test!\n";
        info << "Info Test!\n";
        warn << "Warn Test!\n";
        error << "Error Test!\n";
        fatal << "Fatal Test!\n";
    }
}


