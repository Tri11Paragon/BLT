/*
 * Created by Brett on 23/08/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/std/assert.h>
#include <blt/std/utility.h>
#include <blt/std/logging.h>
#include <blt/std/string.h>
#include <iostream>
#include <iomanip>
#include <sstream>

#if defined(__GNUC__) && !defined(__EMSCRIPTEN__)
    
    #include <execinfo.h>
    #include <cstdlib>
#endif

#if defined(__GNUC__) && !defined(__EMSCRIPTEN__)
    #define BLT_STACK_TRACE(number) void* ptrs[number]; \
            int size = backtrace(ptrs, number);         \
            char** messages = backtrace_symbols(ptrs, size);
    
    #define BLT_FREE_STACK_TRACE() free(messages);

#else
    #define BLT_STACK_TRACE(number) void();
    #define BLT_FREE_STACK_TRACE() void();
#endif

namespace blt {
    
    static inline std::string _macro_filename(const std::string& path){
        auto paths = blt::string::split(path, "/");
        auto final = paths[paths.size()-1];
        if (final == "/")
            return paths[paths.size()-2];
        return final;
    }
    
    void b_throw(const char* what, const char* path, int line)
    {
#if defined(__GNUC__) && !defined(__EMSCRIPTEN__)
        BLT_STACK_TRACE(50);
        
        BLT_ERROR("An exception '%s' has occurred in file '%s:%d'", what, path, line);
        BLT_ERROR("Stack Trace:");
        printStacktrace(messages, size, path, line);
        
        BLT_FREE_STACK_TRACE();
#endif
    }
    
    void b_assert_failed(const char* expression, const char* path, int line)
    {
#if defined(__GNUC__) && !defined(__EMSCRIPTEN__)
        BLT_STACK_TRACE(50);
        
        BLT_ERROR("The assertion '%s' has failed in file '%s:%d'", expression, path, line);
        BLT_ERROR("Stack Trace:");
        
        printStacktrace(messages, size, path, line);
        
        BLT_FREE_STACK_TRACE();
#endif
    }
    
    void printStacktrace(char** messages, int size, const char* path, int line)
    {
        if (messages == nullptr)
            return;
#if defined(__GNUC__) && !defined(__EMSCRIPTEN__)
        for (int i = 1; i < size; i++){
            int tabs = i - 1;
            std::string buffer;
            for (int j = 0; j < tabs; j++)
                buffer += '\t';
            if (i != 1)
                buffer += "⮡";
            
            std::string message(messages[i]);
            
            auto f = message.find('(');
            
            auto mes = message.substr(f + 1, message.size());
            std::string loc;
            
            buffer += message.substr(0, f);
            if (i == 1)
            {
                loc = '\'';
                loc += _macro_filename(path);
                loc += ':';
                loc += std::to_string(line);
                loc += '\'';
            } else
                loc = demangle(mes.substr(0, mes.find('+')));
            
            if (!loc.empty())
                buffer += " in ";
            buffer += loc;
            

            BLT_ERROR(buffer);
        }
#endif
    }
    
    bool assert_print(const char* what)
    {
        BLT_ERROR(what);
        return true;
    }
    
    
}