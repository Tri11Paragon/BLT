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

namespace blt::fs {
    std::vector<std::string> getLinesFromFile(const std::string& path);
    
    std::vector<std::string> recursiveShaderInclude(const std::string& path);
    
    static inline std::string loadShaderFile(const std::string& path) {
        std::stringstream stringStream;
        
        auto lines = recursiveShaderInclude(path);
        
        for (const auto& line : lines) {
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
