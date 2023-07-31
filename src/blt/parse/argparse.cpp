/*
 * Created by Brett on 28/07/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/parse/argparse.h>
#include "blt/std/logging.h"

namespace blt::parser {
    
    arg_vector::arg_vector(const std::vector<std::string>& args) {
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
    
    arg_tokenizer::arg_tokenizer(size_t argc, const char** argv) {
        for (size_t i = 0; i < argc; i++)
            args.emplace_back(argv[i]);
    }
    
    bool argparse::validateArgument(const arg_properties& args) {
        return !args.a_flags.getFlags().empty() ^ !args.a_flags.getNames().empty();
    }
    
    void argparse::addArgument(const arg_properties& args) {
        if (!validateArgument(args)) {
            BLT_WARN("Argument contains both flags and positional names, this is not allowed!");
            BLT_WARN("(Discarding argument)");
            return;
        }
        // store one copy of the properties (arg properties could in theory be very large!)
        auto pos = user_args.argStorage.size();
        user_args.argStorage.push_back(args);
        auto& arg = user_args.argStorage[pos];
        // associate the arg properties per name and flag to allow for quick access when parsing
        auto& names = args.a_flags.getNames();
        for (const auto& name : names)
            user_args.nameAssociations.emplace_back(name, &arg);
        
        auto& flags = args.a_flags.getFlags();
        for (const auto& flag : flags)
            user_args.flagAssociations[flag] = &arg;
    }
    
    const argparse::arg_results& argparse::parse_args(int argc, const char** argv) {
        loaded_args = {};
        arg_tokenizer asToken(argc, argv);
        loaded_args.programName = asToken.next();
        BLT_TRACE("Loading args for %s", loaded_args.programName.c_str());
        
        size_t lastPositional;
        while (asToken.hasNext()) {
            if (!asToken.isFlag()){
                loaded_args.positionalArgs[user_args.nameAssociations[lastPositional].first] = asToken.next();
                continue;
            }
            
        }
        
        return loaded_args;
    }
    
    
}