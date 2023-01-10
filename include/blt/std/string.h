/*
 * Created by Brett on 26/12/22.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */

#ifndef BLT_STRING_H
#define BLT_STRING_H

#include <string>
#include <sstream>
#include <algorithm>
#include <vector>

namespace blt::String {
    /**
     * Converts the string into lower case
     * @param s string to lower case
     * @return a string copy that is all lower case
     */
    static inline std::string toLowerCase(const std::string& s) {
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
    static inline std::string toUpperCase(const std::string& s) {
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
    static inline std::vector<std::string> split(std::string s, const std::string& delim) {
        size_t pos = 0;
        std::vector<std::string> tokens;
        while ((pos = s.find(delim)) != std::string::npos) {
            auto token = s.substr(0, pos);
            tokens.push_back(token);
            s.erase(0, pos + delim.length());
        }
        tokens.push_back(s);
        return tokens;
    }
    
    // taken from https://stackoverflow.com/questions/216823/how-to-trim-an-stdstring
    // would've preferred to use boost lib but instructions said to avoid external libs
    // trim from start (in place)
    static inline std::string& ltrim(std::string& s) {
        s.erase(
                s.begin(), std::find_if(
                        s.begin(), s.end(), [](unsigned char ch) {
                            return !std::isspace(ch);
                        }
                ));
        return s;
    }
    
    // trim from end (in place)
    static inline std::string& rtrim(std::string& s) {
        s.erase(
                std::find_if(
                        s.rbegin(), s.rend(), [](unsigned char ch) {
                            return !std::isspace(ch);
                        }
                ).base(), s.end());
        return s;
    }
    
    // trim from both ends (in place)
    static inline std::string& trim(std::string& s) {
        ltrim(s);
        rtrim(s);
        return s;
    }
    
    // trim from start (copying)
    static inline std::string ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }
    
    // trim from end (copying)
    static inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }
    
    // trim from both ends (copying)
    static inline std::string trim_copy(std::string s) {
        trim(s);
        return s;
    }
}

#endif //BLT_STRING_H
