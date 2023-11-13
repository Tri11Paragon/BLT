/*
 * Created by Brett on 08/02/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_TESTS_LOADER_H
#define BLT_TESTS_LOADER_H

#include <fstream>
#include <ios>
#include <vector>
#include <unordered_map>
#include <string>
#include <blt/std/string.h>
#include <blt/std/logging.h>
#include <unordered_set>

namespace blt::fs
{
    struct include_guard
    {
        char open = '<';
        char close = '>';
    };
    
    std::vector<std::string> getLinesFromFile(const std::string& path);
    
    /**
     * Recursively include files
     * @param path initial file to load
     * @param include_header the beginning of the line that should be used to recognize when a line is to be treated as an include
     * @param guards characters used to identify the parts that specify the file path. if empty it will assume everything after the include header
     * @return a list of lines in all files. added together in order.
     */
    std::vector<std::string> recursiveInclude(const std::string& path, const std::string& include_header = "#include",
                                              const std::vector<include_guard>& guards = {{'<',  '>'}, {'"', '"'}});
    
    static inline std::string loadBrainFuckFile(const std::string& path)
    {
        std::string buffer;
        
        auto lines = recursiveInclude(path, "~");
        
        return buffer;
    }
    
    static inline std::string loadShaderFile(const std::string& path)
    {
        std::stringstream stringStream;
        
        auto lines = recursiveInclude(path);
        
        for (const auto& line : lines)
        {
            // now process the defines, if they exist
//            if (line.starts_with("#define")) {
//                auto defineParts = String::split(line, " ");
//                // create a new define statement in the defines place but with the respective value.
//                if (defines.contains(defineParts[1])) {
//                    stringStream << "#define ";
//                    stringStream << defineParts[1] << " ";
//                    stringStream << defines[defineParts[1]];
//                    stringStream << "\n";
//                    continue;
//                }
//            }
            stringStream << line;
            stringStream << "\n";
        }
        
        //tlog << stringStream.str();
        return stringStream.str();
    }
}

#endif //BLT_TESTS_LOADER_H
