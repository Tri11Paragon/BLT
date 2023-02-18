/*
 * Created by Brett on 08/02/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/std/loader.h>

std::vector<std::string> blt::fs::getLinesFromFile(const std::string& path) {
    std::string shaderSource;
    std::ifstream shaderFile;
    if (!shaderFile.good())
        BLT_ERROR("Input stream not good!\n");
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        // open file
        shaderFile.open(path);
        std::stringstream shaderStream;
        // read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();
        // close file handlers
        shaderFile.close();
        // convert stream into std::string
        shaderSource = shaderStream.str();
    } catch (std::ifstream::failure& e) {
        BLT_WARN("Unable to read file '%s'!\n", path.c_str());
        throw std::runtime_error("Failed to read file!\n");
    }
    
    // split the shader into the lines, this way we can get out the #include statements.
    return string::split(shaderSource, "\n");
}

std::vector<std::string> blt::fs::recursiveShaderInclude(const std::string& path) {
    std::string pathOnly = path.substr(0, path.find_last_of('/'));
    
    auto mainLines = getLinesFromFile(path);
    std::unordered_map<int, std::vector<std::string>> includes;
    
    for (int i = 0; i < mainLines.size(); i++) {
        auto& line = mainLines[i];
        // if the line is an include statement then we want to add lines recursively.
        if (string::starts_with(line, "#include")) {
            std::vector<std::string> include_statement = string::split(line, "<");
            
            if (include_statement.size() <= 1)
                include_statement = string::split(line, "\"");
            
            string::trim(line);
            if (!(string::ends_with(line, ">") || string::ends_with(line, "\""))) {
                BLT_FATAL("Shader file contains an invalid #include statement. (Missing terminator)\n");
                std::abort();
            }
            try {
                // filter out the > or " at the end of the include statement.
                std::string file;
                file = include_statement[1];
                if (string::ends_with(include_statement[1], ">"))
                    file = file.substr(0, file.size() - 1);
                
                BLT_TRACE("Recusing into %s/%s\n", pathOnly.c_str(), file.c_str());
                
                includes.insert({i, recursiveShaderInclude((pathOnly + "/" + file))});
            } catch (std::exception& e) {
                BLT_FATAL("Shader file contains an invalid #include statement. (Missing < or \")\n");
                std::abort();
            }
        }
    }
    
    std::vector<std::string> returnLines;
    
    // now combine all the loaded files while respecting the include's position in the file.
    for (int i = 0; i < mainLines.size(); i++) {
        if (!includes[i].empty()) {
            auto includedFileLines = includes[i];
            
            for (const auto& line : includedFileLines)
                returnLines.push_back(line);
        } else
            returnLines.push_back(mainLines[i]);
    }
    return returnLines;
}
