/*
 * Created by Brett on 28/07/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/parse/argparse.h>
#include "blt/std/logging.h"

namespace blt::parser {
    
    arg_vector_t::arg_vector_t(const std::vector<std::string>& args) {
        for (auto& arg : args)
            insertAndSort(arg);
    }
    
    arg_vector_t::arg_vector_t(std::initializer_list<std::string> args) {
        for (auto& arg : args)
            insertAndSort(arg);
    }
    
    arg_vector_t::arg_vector_t(const std::string& arg) {
        insertAndSort(arg);
    }
    
    arg_vector_t::arg_vector_t(const char* arg) {
        insertAndSort(arg);
    }
    
    arg_vector_t& arg_vector_t::operator=(const std::string& arg) {
        insertAndSort(arg);
        return *this;
    }
    
    arg_vector_t& arg_vector_t::operator=(const char* arg) {
        insertAndSort(arg);
        return *this;
    }
    
    arg_vector_t& arg_vector_t::operator=(std::initializer_list<std::string>& args) {
        for (auto& arg : args)
            insertAndSort(arg);
        return *this;
    }
    
    arg_vector_t& arg_vector_t::operator=(std::vector<std::string>& args) {
        for (auto& arg : args)
            insertAndSort(arg);
        return *this;
    }
    
    void arg_vector_t::insertAndSort(const std::string& str) {
        if (str.starts_with('-'))
            flags.push_back(str);
        else
            names.push_back(str);
    }
    
    arg_nargs_t::arg_nargs_t(char c) {
        decode(c);
    }
    
    arg_nargs_t::arg_nargs_t(std::string s) {
        decode(s[0]);
    }
    
    arg_nargs_t::arg_nargs_t(const char* s) {
        decode(*s);
    }
    
    arg_nargs_t& arg_nargs_t::operator=(const std::string& s) {
        decode(s[0]);
        return *this;
    }
    
    arg_nargs_t& arg_nargs_t::operator=(char c) {
        decode(c);
        return *this;
    }
    
    arg_nargs_t& arg_nargs_t::operator=(int a) {
        args = a;
        return *this;
    }
    
    arg_nargs_t& arg_nargs_t::operator=(const char* s) {
        decode(*s);
        return *this;
    }
    
    void arg_nargs_t::decode(char c) {
        if (c == '?')
            flags = UNKNOWN;
        else if (c == '+')
            flags = ALL_REQUIRED;
        else if (c == '*')
            flags = ALL;
        else
            flags = 0;
    }
    
    arg_tokenizer_t::arg_tokenizer_t(size_t argc, const char** argv) {
        for (size_t i = 0; i < argc; i++)
            args.emplace_back(argv[i]);
    }
    
    bool argparse::validateArgument(const arg_properties_t& args) {
        return !args.a_flags.getFlags().empty() ^ !args.a_flags.getNames().empty();
    }
    
    void argparse::addArgument(const arg_properties_t& args) {
        if (!validateArgument(args)) {
            BLT_WARN("Argument contains both flags and positional names, this is not allowed!");
            BLT_WARN("(Discarding argument)");
            return;
        }
        // store one copy of the properties (arg properties could in theory be very large!)
        auto pos = user_args.arg_storage.size();
        user_args.arg_storage.push_back(args);
        auto& arg = user_args.arg_storage[pos];
        // associate the arg properties per name and flag to allow for quick access when parsing
        auto& names = args.a_flags.getNames();
        for (const auto& name : names)
            user_args.name_associations.emplace_back(name, &arg);
        
        auto& flags = args.a_flags.getFlags();
        for (const auto& flag : flags)
            user_args.flag_associations[flag] = &arg;
        
        if (args.a_required)
            user_args.required_vars.insert(args.a_flags);
    }
    
    const argparse::arg_results& argparse::parse_args(int argc, const char** argv) {
        loaded_args = {};
        arg_tokenizer_t arg_tokenizer(argc, argv);
        loaded_args.program_name = arg_tokenizer.next();
        BLT_TRACE("Loading args for %s", loaded_args.program_name.c_str());
        
        size_t last_positional;
        while (arg_tokenizer.hasNext()) {
            // a token isn't a flag it must be a positional arg as flags will consume nargs
            if (!arg_tokenizer.isFlag()){
                handlePositionalArgument(arg_tokenizer, last_positional);
                continue;
            }
            handleFlagArgument(arg_tokenizer);
        }
        
        return loaded_args;
    }
    
    void argparse::printHelp() {
    
    }
    
    void argparse::handlePositionalArgument(arg_tokenizer_t& arg_tokenizer, size_t& last_pos) {
        // TODO:
        loaded_args.positional_args[user_args.name_associations[last_pos++].first] = arg_tokenizer.next();
    }
    
    void argparse::handleFlagArgument(arg_tokenizer_t& arg_tokenizer) {
        // token is a flag, find out special information about it
        auto flag = arg_tokenizer.next();
        
        auto loc = user_args.flag_associations.find(flag);
        if (loc == user_args.flag_associations.end()){
            BLT_WARN("Flag '%s' not an option!");
            printHelp();
            return;
        }
        
        auto flag_properties = loc->second;
        
        arg_data_t data;
        switch(flag_properties->a_action){
            case arg_action_t::HELP:
                printHelp();
                break;
            case arg_action_t::STORE:
                break;
            case arg_action_t::STORE_CONST:
                data = flag_properties->a_const;
                break;
            case arg_action_t::STORE_FALSE:
                data = false;
                break;
            case arg_action_t::STORE_TRUE:
                data = true;
                break;
            case arg_action_t::APPEND:
                if (holds_alternative<std::vector<std::string>>(data)){
                    auto& l = get<std::vector<std::string>>(data);
                    
                } else
                    BLT_WARN("Requested append to data which doesn't contain a list!");
                break;
            case arg_action_t::APPEND_CONST:
                break;
            case arg_action_t::COUNT:
                break;
            case arg_action_t::EXTEND:
                break;
            case arg_action_t::VERSION:
                break;
        }
        loaded_args.flag_args[flag_properties->a_flags] = data;
    }
    
    bool argparse::consumeFlagArguments(arg_tokenizer_t& arg_tokenizer, const arg_properties_t& properties, arg_data_t& arg_data) {
        // since the function is called after the flag is consumed, isFlag()/ will return information about the next arg
        if (properties.a_nargs.flags) {
        
        } else {
            if (arg_tokenizer.isFlag())
                return false;
            
        }
        return false;
    }
    
    
}