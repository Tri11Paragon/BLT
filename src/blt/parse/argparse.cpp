/*
 * Created by Brett on 28/07/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/parse/argparse.h>

namespace blt::parser {
    
    arg_vector::arg_vector(std::vector<std::string> args) {
        for (auto& arg : args)
            insertAndSort(arg);
    }
    
    arg_vector::arg_vector(std::initializer_list<std::string> args) {
        for (auto& arg : args)
            insertAndSort(arg);
    }
    
    arg_vector::arg_vector(const std::string& arg) {
        insertAndSort(arg);
    }
    
    arg_vector::arg_vector(const char* arg) {
        insertAndSort(arg);
    }
    
    arg_vector& arg_vector::operator=(const std::string& arg) {
        insertAndSort(arg);
        return *this;
    }
    
    arg_vector& arg_vector::operator=(const char* arg) {
        insertAndSort(arg);
        return *this;
    }
    
    arg_vector& arg_vector::operator=(std::initializer_list<std::string>& args) {
        for (auto& arg : args)
            insertAndSort(arg);
        return *this;
    }
    
    arg_vector& arg_vector::operator=(std::vector<std::string>& args) {
        for (auto& arg : args)
            insertAndSort(arg);
        return *this;
    }
    
    void arg_vector::insertAndSort(const std::string& str) {
        if (str.starts_with('-'))
            flags.push_back(str);
        else
            names.push_back(str);
    }
    
    arg_nargs::arg_nargs(char c) {
        decode(c);
    }
    
    arg_nargs::arg_nargs(std::string s) {
        decode(s[0]);
    }
    
    arg_nargs::arg_nargs(const char* s) {
        decode(*s);
    }
    
    arg_nargs& arg_nargs::operator=(const std::string& s) {
        decode(s[0]);
        return *this;
    }
    
    arg_nargs& arg_nargs::operator=(char c) {
        decode(c);
        return *this;
    }
    
    arg_nargs& arg_nargs::operator=(int a) {
        args = a;
        return *this;
    }
    
    arg_nargs& arg_nargs::operator=(const char* s) {
        decode(*s);
        return *this;
    }
    
    void arg_nargs::decode(char c) {
        if (c == '?')
            flags |= UNKNOWN;
        if (c == '+')
            flags |= ALL_REQUIRED;
        if (c == '*')
            flags |= ALL;
    }
}