/*
 * Created by Brett on 08/02/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/std/loader.h>
#include <blt/std/assert.h>

std::vector<std::string> blt::fs::getLinesFromFile(const std::string& path)
{
    std::string shaderSource;
    std::ifstream shaderFile;
    if (!shaderFile.good())
        BLT_ERROR("Input stream not good!\n");
    // ensure ifstream objects can throw exceptions:
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open file
        shaderFile.open(path);
        std::stringstream shaderStream;
        // read file's buffer contents into streams
        shaderStream << shaderFile.rdbuf();
        // close file handlers
        shaderFile.close();
        // convert stream into std::string
        shaderSource = shaderStream.str();
    } catch (std::ifstream::failure& e)
    {
        BLT_WARN("Unable to read file '%s'!\n", path.c_str());
        BLT_WARN("Exception: %s", e.what());
        throw std::runtime_error("Failed to read file!\n");
    }
    
    // split the shader into the lines, this way we can get out the #include statements.
    return string::split(shaderSource, "\n");
}

std::vector<std::string> blt::fs::recursiveInclude(const std::string& path, const std::string& include_header,
                                                   const std::vector<include_guard>& guards)
{
    std::string pathOnly = path.substr(0, path.find_last_of('/'));
    
    auto mainLines = getLinesFromFile(path);
    std::vector<std::string> return_lines;
    
    for (auto& line : mainLines)
    {
        // if the line is an include statement then we want to add lines recursively.
        auto include_pos = line.find(include_header);
        if (include_pos != std::string::npos)
        {
            auto past_include = include_pos + include_header.size();
            BLT_TRACE(past_include);
            BLT_TRACE("%c", (char)line[past_include]);
            std::string file_to_include;
            
            if (guards.empty())
            {
                file_to_include = line.substr(line.find(include_header));
            } else
            {
                size_t index = past_include;
                while (std::find_if(guards.begin(), guards.end(), [&](const include_guard& item) {
                    return line[index] == item.open;
                }) == guards.end())
                    index++;
                index++;
                BLT_ASSERT(index < line.size() && "Include found but no file was provided!");
                
                std::cout << line.substr(index) << std::endl;
            }

//            size_t index = 0;
//
//                std::vector<std::string> include_statement = string::split(line, "<");
//
//            if (include_statement.size() <= 1)
//                include_statement = string::split(line, "\"");
//
//            string::trim(line);
//            if (!(string::ends_with(line, ">") || string::ends_with(line, "\"")))
//            {
//                BLT_FATAL("Shader file contains an invalid #include statement. (Missing terminator)\n");
//                std::abort();
//            }
//            try
//            {
//                // filter out the > or " at the end of the include statement.
//                std::string file;
//                file = include_statement[1];
//                if (string::ends_with(include_statement[1], ">"))
//                    file = file.substr(0, file.size() - 1);
//
//                BLT_TRACE("Recusing into %s/%s\n", pathOnly.c_str(), file.c_str());
//
//                includes.insert({i, recursiveInclude(pathOnly + "/" + file, include_header, guards)});
//            } catch (std::exception& e)
//            {
//                BLT_FATAL("Shader file contains an invalid #include statement. (Missing < or \")\n");
//                BLT_FATAL("Exception: %s", e.what());
//                std::abort();
//            }
        } else
            return_lines.push_back(line);
    }
    
    return return_lines;
}
