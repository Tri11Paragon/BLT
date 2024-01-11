/*
 *  <Short Description>
 *  Copyright (C) 2024  Brett Terpstra
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <blt/parse/obj_loader.h>
#include <blt/std/loader.h>
#include <blt/std/string.h>
#include <blt/std/logging.h>
#include <cctype>
#include <charconv>
#include "blt/std/assert.h"
#include "blt/std/utility.h"

namespace blt::gfx
{
    class char_tokenizer
    {
        private:
            std::string_view string;
            std::size_t current_pos = 0;
        public:
            explicit char_tokenizer(std::string_view view): string(view)
            {}
            
            inline char advance()
            {
                return string[current_pos++];
            }
            
            inline bool has_next(size_t offset = 0)
            {
                return current_pos + offset < string.size();
            }
            
            inline std::string_view read_fully()
            {
                return blt::string::trim(string.substr(current_pos));
            }
    };
    
    template<typename T = float>
    T get(std::string_view str)
    {
        T x;
        const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), x);
        // probably not needed.
        if (ec != std::errc())
        {
//            int i;
//            const auto [ptr2, ec2] = std::from_chars(str.data(), str.data() + str.size(), i);
//            if (ec2 == std::errc())
//            {
//                x = static_cast<float>(i);
//            } else
//            {
            BLT_WARN("Unable to parse string '%s' into number!", std::string(str).c_str());
            x = 0;
//            }
        }
        return x;
    }
    
    void obj_loader::parse_vertex_line(char_tokenizer& tokenizer)
    {
        char type = tokenizer.advance();
        
        if (type == 'p')
        {
            BLT_WARN("Unexpected type '%c' (not supported)", type);
            return;
        }
        
        auto elements = blt::string::split(std::string(tokenizer.read_fully()), " ");
        BLT_ASSERT(elements.size() >= 2 && "Current line doesn't have enough arguments to process!");
        float x = get(elements[0]), y = get(elements[1]);
        if (elements.size() < 3)
        {
            if (type == 't')
                uvs.push_back(uv_t{x, y});
            else
                BLT_ERROR("Unable to parse line '%s' type '%c' not recognized for arg count", std::string(tokenizer.read_fully()).c_str(), type);
        } else
        {
            float z = get(elements[2]);
            if (!handle_vertex_and_normals(x, y, z, type))
                BLT_ERROR("Unable to parse line '%s' type '%c' not recognized", std::string(tokenizer.read_fully()).c_str(), type);
        }
    }
    
    bool obj_loader::handle_vertex_and_normals(float x, float y, float z, char type)
    {
        if (std::isspace(type))
        {
            vertices.push_back(vertex_t{x, y, z});
        } else if (type == 'n')
        {
            normals.push_back(normal_t{x, y, z});
        } else
            return false;
        return true;
    }
    
    obj_objects_t quick_load(std::string_view file)
    {
        return obj_loader().parseFile(file);
    }
    
    obj_objects_t obj_loader::parseFile(std::string_view file)
    {
        auto lines = blt::fs::getLinesFromFile(std::string(file));
        for (const auto& line : lines)
        {
            char_tokenizer token(line);
            if (!token.has_next() || token.read_fully().empty())
                continue;
            switch (token.advance())
            {
                case '#':
                    continue;
                case 'f':
                    parse_face(token);
                    break;
                case 'v':
                    parse_vertex_line(token);
                    break;
                case 'o':
                {
                    if (!current_object.indices.empty())
                        data.push_back(current_object);
                    current_object = {};
                    current_object.object_name = token.read_fully();
                    break;
                }
                case 'm':
                {
                    BLT_TRACE("Material '%s' needs to be loaded!", std::string(token.read_fully()).c_str());
                    break;
                }
                case 'u':
                {
                    BLT_TRACE("Using material '%s'", std::string(token.read_fully()).c_str());
                    break;
                }
                case 's':
                    BLT_TRACE("Using shading: %s", std::string(token.read_fully()).c_str());
                    break;
            }
        }
        data.push_back(current_object);
        return {std::move(vertex_data), std::move(data)};
    }
    
    void obj_loader::parse_face(char_tokenizer& tokenizer)
    {
        auto faces = blt::string::split(std::string(tokenizer.read_fully()), ' ');
        if (faces.size() == 3)
        {
            triangle_t triangle{};
            handle_face_vertex(faces, triangle.v);
            current_object.indices.push_back(triangle);
        } else if (faces.size() == 4)
        {
            quad_t quad{};
            handle_face_vertex(faces, quad.v);
            triangle_t t1{};
            triangle_t t2{};
            
            for (int i = 0; i < 3; i++)
                t1.v[i] = quad.v[i];
            t2.v[0] = quad.v[0];
            t2.v[1] = quad.v[2];
            t2.v[2] = quad.v[3];
            
            current_object.indices.push_back(t1);
            current_object.indices.push_back(t2);
        } else
            BLT_WARN("Unsupported vertex count! %d", faces.size());
    }
    
    void obj_loader::handle_face_vertex(const std::vector<std::string>& face_list, int32_t* arr)
    {
        for (const auto& pair : blt::enumerate(face_list))
        {
            auto indices = blt::string::split(pair.second, '/');
            BLT_ASSERT(indices.size() == 3 && "Must have vertex, uv, and normal indices!!");
            
            auto vi = get<std::int32_t>(indices[0]);
            auto ui = get<std::int32_t>(indices[1]);
            auto ni = get<std::int32_t>(indices[2]);
            
            face_t face{vi, ui, ni};
            
            auto loc = vertex_map.find(face);
            if (loc == vertex_map.end())
            {
                auto index = static_cast<std::int32_t>(vertex_data.size());
                vertex_data.push_back({vertices[vi], uvs[ui], normals[ni]});
                vertex_map.insert({face, index});
                arr[pair.first] = index;
            } else
            {
                arr[pair.first] = loc->second;
            }
        }
    }
}