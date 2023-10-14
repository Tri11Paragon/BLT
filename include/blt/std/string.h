/*
 * Created by Brett on 26/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_STRING_H
#define BLT_STRING_H

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <cctype>

namespace blt::string
{
    
    class StringBuffer
    {
        private:
            const size_t BLOCK_SIZE = 4096;
            size_t front = 0;
            size_t size = 0;
            char* characterBuffer = nullptr;
            
            void expand();
        
        public:
            void trim();
            
            std::string str();
            
            StringBuffer()
            {
                characterBuffer = static_cast<char*>(malloc(BLOCK_SIZE));
                size = BLOCK_SIZE;
            }
            
            StringBuffer& operator<<(char c);
            
            StringBuffer& operator<<(const std::string& str)
            {
                for (char c : str)
                    *this << c;
                return *this;
            }
            
            template<typename T>
            inline StringBuffer& operator<<(T t)
            {
                *this << std::to_string(t);
                return *this;
            }
            
            ~StringBuffer()
            {
                free(characterBuffer);
            }
    };
    
    static inline bool starts_with(const std::string& string, const std::string& search)
    {
        if (search.length() > string.length())
            return false;
        auto chars = string.c_str();
        auto search_chars = search.c_str();
        for (unsigned int i = 0; i < search.length(); i++)
        {
            if (chars[i] != search_chars[i])
                return false;
        }
        return true;
    }
    
    static inline bool ends_with(const std::string& string, const std::string& search)
    {
        if (search.length() > string.length())
            return false;
        auto chars = string.c_str();
        auto search_chars = search.c_str();
        auto startPosition = string.length() - search.length();
        for (unsigned int i = 0; i < search.length(); i++)
        {
            if (chars[startPosition + i] != search_chars[i])
                return false;
        }
        return true;
    }
    
    static inline bool contains(const std::string& string, const char search)
    {
        std::ranges::any_of(string, [search](const char c) -> bool {
            return c == search;
        });
    }
    
    static inline bool contains(const std::string& string, const std::string& search)
    {
        if (search.length() > string.length())
            return false;
        auto chars = string.c_str();
        auto search_chars = search.c_str();
        for (unsigned int i = 0; i < string.length(); i++)
        {
            if (chars[i] == search_chars[0])
            {
                bool correct = true;
                for (unsigned int j = 0; j < search.length(); j++)
                {
                    if (chars[i + j] != search_chars[j])
                    {
                        correct = false;
                        break;
                    }
                }
                if (correct)
                    return true;
            }
        }
        return false;
    }
    
    /**
     * Converts the string into lower case
     * @param s string to lower case
     * @return a string copy that is all lower case
     */
    static inline std::string toLowerCase(const std::string& s)
    {
        std::stringstream str;
        std::for_each(
                s.begin(), s.end(), [&str](unsigned char ch) {
                    str << (char) std::tolower(ch);
                }
        );
        return str.str();
    }
    
    /**
     * Converts the string into upper case
     * @param s string to upper case
     * @return a string copy that is all upper case
     */
    static inline std::string toUpperCase(const std::string& s)
    {
        std::stringstream str;
        std::for_each(
                s.begin(), s.end(), [&str](unsigned char ch) {
                    str << (char) std::toupper(ch);
                }
        );
        return str.str();
    }
    
    // taken from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    // extended to return a vector
    static inline std::vector<std::string> split(std::string s, const std::string& delim)
    {
        size_t pos = 0;
        std::vector<std::string> tokens;
        while ((pos = s.find(delim)) != std::string::npos)
        {
            auto token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + delim.length());
        }
        tokens.push_back(s);
        return tokens;
    }
    
    // https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
    static inline bool replace(std::string& str, const std::string& from, const std::string& to)
    {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
    
    static inline void replaceAll(std::string& str, const std::string& from, const std::string& to)
    {
        if (from.empty())
            return;
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos)
        {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }
    
    
    // taken from https://stackoverflow.com/questions/216823/how-to-trim-an-stdstring
    // would've preferred to use boost lib but instructions said to avoid external libs
    // trim from start (in place)
    static inline std::string& ltrim(std::string& s)
    {
        s.erase(
                s.begin(), std::find_if(
                        s.begin(), s.end(), [](unsigned char ch) {
                            return !std::isblank(ch);
                        }
                ));
        return s;
    }
    
    // trim from end (in place)
    static inline std::string& rtrim(std::string& s)
    {
        s.erase(
                std::find_if(
                        s.rbegin(), s.rend(), [](unsigned char ch) {
                            return !std::isblank(ch);
                        }
                ).base(), s.end());
        return s;
    }
    
    // trim from both ends (in place)
    static inline std::string& trim(std::string& s)
    {
        ltrim(s);
        rtrim(s);
        return s;
    }
    
    // trim from start (copying)
    static inline std::string ltrim_copy(std::string s)
    {
        ltrim(s);
        return s;
    }
    
    // trim from end (copying)
    static inline std::string rtrim_copy(std::string s)
    {
        rtrim(s);
        return s;
    }
    
    // trim from both ends (copying)
    static inline std::string trim_copy(std::string s)
    {
        trim(s);
        return s;
    }
    
    static inline bool is_numeric(const std::string& s)
    {
        return std::ranges::all_of(s, [](char c) -> bool {
            return std::isdigit(c);
        });
    }
    
}

#endif //BLT_STRING_H
