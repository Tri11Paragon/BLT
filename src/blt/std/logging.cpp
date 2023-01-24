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

// https://en.cppreference.com/w/cpp/utility/variadic
// https://medium.com/swlh/variadic-functions-3419c287a0d2
// https://publications.gbdirect.co.uk//c_book/chapter9/stdarg.html
// https://cplusplus.com/reference/cstdio/printf/


namespace blt::logging {
    
    inline std::string applyFormat(const char* format, va_list args) {
        std::stringstream output;
        char currentCharacter;
        while ((currentCharacter = *format) != '\0') {
            if (currentCharacter == '%') {
                char nextCharacter = *(++format);
                if (nextCharacter == '\0')
                    break;
                switch (nextCharacter) {
                    case 'd':
                    case 'i':
                        output << static_cast<char>(va_arg(args, int));
                        break;
                    case 'u':
                        output << static_cast<char>(va_arg(args, unsigned int));
                        break;
                    case 'o':
                        // TODO octal
                        break;
                    case 'x':
                    case 'X':
                        // TODO hex
                        break;
                    case 'f':
                    case 'F':
                        output << va_arg(args, double);
                    case 'e':
                    case 'E':
                        // TODO: scientific notation
                        break;
                    case 'g':
                    case 'G':
                        // TODO: shortest representation
                        output << va_arg(args, double);
                        break;
                    case 'a':
                    case 'A':
                        output << va_arg(args, double);
                        break;
                    case 'c':
                        output << va_arg(args, int);
                        break;
                    case 's':
                        output << va_arg(args, const char*);
                        break;
                    case 'p':
                        output << va_arg(args, void*);
                        break;
                    case 'n':
                        va_arg(args, int*);
                        break;
                    case '%':
                        output << '%';
                        break;
                    default:
                        output << nextCharacter;
                        break;
                }
            } else
                output << currentCharacter;
            ++format;
        }
        return output.str();
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
    
    void log(LOG_LEVEL level, bool auto_line, const char* format, ...) {
        va_list args;
        va_start(args, format);
        
        auto formattedString = applyFormat(format, args);
        bool hasEndingLinefeed = formattedString[formattedString.length()-1] == '\n';
        
        if (hasEndingLinefeed)
            formattedString = formattedString.substr(0, formattedString.length()-1);
    
        std::string outputString = System::getTimeStringLog();
        outputString += levelNames[level];
        
        if (BLT_LOGGING_PROPERTIES.m_useColor)
            outputString = levelColors[level] + outputString;
        
        outputString += formattedString;
        
        if (BLT_LOGGING_PROPERTIES.m_useColor)
            outputString += "\033[0m";
        if (hasEndingLinefeed || auto_line)
            outputString += "\n";
        
        if (BLT_LOGGING_PROPERTIES.m_logToConsole) {
            if (level > WARN)
                std::cerr << outputString;
            else
                std::cout << outputString;
        }
    
        va_end(args);
    }
}


