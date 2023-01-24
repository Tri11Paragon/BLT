/*
 * Created by Brett on 23/01/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/std/logging.h>
#include <blt/std/time.h>
#include <cstdarg>
#include <string>
#include <sstream>
#include <iostream>
#include "blt/std/string.h"

// https://en.cppreference.com/w/cpp/utility/variadic
// https://medium.com/swlh/variadic-functions-3419c287a0d2
// https://publications.gbdirect.co.uk//c_book/chapter9/stdarg.html
// https://cplusplus.com/reference/cstdio/printf/


namespace blt::logging {
    
    void applyFormatting(const std::string& format, std::string& output, va_list& args){
        char formattedChars[format.length()];
        vsprintf(formattedChars, format.c_str(), args);
        output = std::string(formattedChars);
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
    LOG_PROPERTIES BLT_LOGGING_PROPERTIES{true, true, true, "./"};
    
    void init(LOG_PROPERTIES properties) {
        BLT_LOGGING_PROPERTIES = properties;
    }
    
    inline void log(const std::string& str, bool hasEndingLinefeed, LOG_LEVEL level, int auto_line){
        std::string outputString = System::getTimeStringLog();
        outputString += levelNames[level];
        outputString += str;
    
        if (BLT_LOGGING_PROPERTIES.m_useColor) {
            outputString = levelColors[level] + outputString;
            outputString += "\033[0m";
        }
    
        if (hasEndingLinefeed || auto_line)
            outputString += "\n";
    
        if (BLT_LOGGING_PROPERTIES.m_logToConsole) {
            if (level > WARN)
                std::cerr << outputString;
            else
                std::cout << outputString;
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
    
}


