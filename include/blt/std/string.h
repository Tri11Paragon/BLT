/*
 * Created by Brett on 26/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_STRING_H
#define BLT_STRING_H

#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <algorithm>
#include <vector>
#include <optional>
#include <cctype>
#include <unordered_set>
#include <blt/compatibility.h>

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
    
    static inline BLT_CPP20_CONSTEXPR bool starts_with(std::string_view string, std::string_view search)
    {
#ifdef BLT_USE_CPP20
        return string.starts_with(search);
#else
        if (search.length() > string.length())
            return false;
        for (unsigned int i = 0; i < search.length(); i++)
        {
            if (string[i] != search[i])
                return false;
        }
        return true;
#endif
    }
    
    static inline BLT_CPP20_CONSTEXPR bool starts_with(std::string_view string, char search)
    {
#ifdef BLT_USE_CPP20
        return string.starts_with(search);
#else
        if (string.empty())
            return false;
        return string.front() == search;
#endif
    }
    
    static inline BLT_CPP20_CONSTEXPR bool ends_with(std::string_view string, std::string_view search)
    {
#ifdef BLT_USE_CPP20
        return string.ends_with(search);
#else
        if (search.length() > string.length())
            return false;
        auto startPosition = string.length() - search.length();
        for (unsigned int i = 0; i < search.length(); i++)
        {
            if (string[startPosition + i] != search[i])
                return false;
        }
        return true;
#endif
    }
    
    static inline BLT_CPP20_CONSTEXPR bool ends_with(std::string_view string, char search)
    {
#ifdef BLT_USE_CPP20
        return string.ends_with(search);
#else
        if (string.empty())
            return false;
        return string.back() == search;
#endif
    }
    
    static inline std::optional<std::vector<size_t>> containsAll(std::string_view string, const std::unordered_set<char>& search)
    {
        std::vector<size_t> pos;
        for (size_t i = 0; i < string.length(); i++)
        {
            if (BLT_CONTAINS(search, string[i]))
                pos.push_back(i);
        }
        if (!pos.empty())
            return pos;
        return {};
    }
    
    static inline size_t contains(std::string_view string, const std::unordered_set<char>& search)
    {
        for (size_t i = 0; i < string.length(); i++)
        {
            if (BLT_CONTAINS(search, string[i]))
                return i;
        }
        return false;
    }
    
    static inline BLT_CPP20_CONSTEXPR bool contains(std::string_view string, const char search)
    {
#if __cplusplus >= 202002L
        return std::ranges::any_of(string, [search](const char c) -> bool {
            return c == search;
        });
#else
        for (const char c : string)
        {
            if (c == search)
                return true;
        }
        return false;
#endif
    }
    
    static inline BLT_CPP20_CONSTEXPR bool contains(std::string_view string, std::string_view search)
    {
        if (search.length() > string.length())
            return false;
        for (unsigned int i = 0; i < string.length(); i++)
        {
            if (string[i] == search[0])
            {
                bool correct = true;
                for (unsigned int j = 0; j < search.length(); j++)
                {
                    if (string[i + j] != search[j])
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
    static inline BLT_CPP20_CONSTEXPR std::string toLowerCase(std::string_view s)
    {
        std::string str;
        std::for_each(
                s.begin(), s.end(), [&str](unsigned char ch) {
                    str += (char) std::tolower(ch);
                }
        );
        return str;
    }
    
    /**
     * Converts the string into upper case
     * @param s string to upper case
     * @return a string copy that is all upper case
     */
    static inline BLT_CPP20_CONSTEXPR std::string toUpperCase(std::string_view s)
    {
        std::string str;
        std::for_each(
                s.begin(), s.end(), [&str](unsigned char ch) {
                    str += (char) std::toupper(ch);
                }
        );
        return str;
    }
    
    // taken from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c
    // extended to return a vector
    static inline BLT_CPP20_CONSTEXPR std::vector<std::string> split(std::string s, std::string_view delim)
    {
        size_t pos = 0;
        std::vector<std::string> tokens;
        while ((pos = s.find(delim)) != std::string::npos)
        {
            auto token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + delim.length());
        }
        tokens.push_back(std::move(s));
        return tokens;
    }
    
    static inline BLT_CPP20_CONSTEXPR std::vector<std::string> split(std::string s, char delim)
    {
        size_t pos = 0;
        std::vector<std::string> tokens;
        while ((pos = s.find(delim)) != std::string::npos)
        {
            auto token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + 1);
        }
        tokens.push_back(s);
        return tokens;
    }
    
    // https://stackoverflow.com/questions/3418231/replace-part-of-a-string-with-another-string
    static inline BLT_CPP20_CONSTEXPR bool replace(std::string& str, std::string_view from, std::string_view to)
    {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
    
    static inline BLT_CPP20_CONSTEXPR void replaceAll(std::string& str, std::string_view from, std::string_view to)
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
    static inline BLT_CPP20_CONSTEXPR std::string& ltrim(std::string& s)
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
    static inline BLT_CPP20_CONSTEXPR std::string& rtrim(std::string& s)
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
    static inline BLT_CPP20_CONSTEXPR std::string& trim(std::string& s)
    {
        ltrim(s);
        rtrim(s);
        return s;
    }
    
    // trim from start (copying)
    static inline BLT_CPP20_CONSTEXPR std::string ltrim_copy(std::string s)
    {
        ltrim(s);
        return s;
    }
    
    // trim from end (copying)
    static inline BLT_CPP20_CONSTEXPR std::string rtrim_copy(std::string s)
    {
        rtrim(s);
        return s;
    }
    
    // trim from both ends (copying)
    static inline BLT_CPP20_CONSTEXPR std::string trim_copy(std::string s)
    {
        trim(s);
        return s;
    }
    
    static inline BLT_CPP20_CONSTEXPR bool is_numeric(const std::string& s)
    {
#if __cplusplus >= 202002L
        return std::ranges::all_of(s, [](char c) -> bool {
            return std::isdigit(c);
        });
#else
        for (const char c : s)
        {
            if (!std::isdigit(c))
                return false;
        }
        return true;
#endif
    }
    
}

#endif //BLT_STRING_H
