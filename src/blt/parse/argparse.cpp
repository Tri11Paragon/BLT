/*
 * Created by Brett on 28/07/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include "blt/std/logging.h"

#include <blt/std/string.h>

//namespace blt::parser {
//
//    arg_vector_t::arg_vector_t(const std::vector<std::string>& args) {
//        for (auto& arg : args)
//            insertAndSort(arg);
//    }
//
//    arg_vector_t::arg_vector_t(std::initializer_list<std::string> args) {
//        for (auto& arg : args)
//            insertAndSort(arg);
//    }
//
//    arg_vector_t::arg_vector_t(const std::string& arg) {
//        insertAndSort(arg);
//    }
//
//    arg_vector_t::arg_vector_t(const char* arg) {
//        insertAndSort(arg);
//    }
//
//    arg_vector_t& arg_vector_t::operator=(const std::string& arg) {
//        insertAndSort(arg);
//        return *this;
//    }
//
//    arg_vector_t& arg_vector_t::operator=(const char* arg) {
//        insertAndSort(arg);
//        return *this;
//    }
//
//    arg_vector_t& arg_vector_t::operator=(std::initializer_list<std::string>& args) {
//        for (auto& arg : args)
//            insertAndSort(arg);
//        return *this;
//    }
//
//    arg_vector_t& arg_vector_t::operator=(std::vector<std::string>& args) {
//        for (auto& arg : args)
//            insertAndSort(arg);
//        return *this;
//    }
//
//    void arg_vector_t::insertAndSort(const std::string& str) {
//        if (str.starts_with('-')) {
//            flags.push_back(str);
//            isFlags = true;
//        } else {
//            names.push_back(str);
//            isFlags = false;
//        }
//        if (!flags.empty() && !names.empty())
//            BLT_WARN("Flags provided alongside named positional args. This is not supported!");
//    }
//
//    arg_nargs_t::arg_nargs_t(char c) {
//        decode(c);
//    }
//
//    arg_nargs_t::arg_nargs_t(std::string s) {
//        decode(s[0]);
//    }
//
//    arg_nargs_t::arg_nargs_t(const char* s) {
//        decode(*s);
//    }
//
//    arg_nargs_t& arg_nargs_t::operator=(const std::string& s) {
//        decode(s[0]);
//        return *this;
//    }
//
//    arg_nargs_t& arg_nargs_t::operator=(char c) {
//        decode(c);
//        return *this;
//    }
//
//    arg_nargs_t& arg_nargs_t::operator=(int a) {
//        args = a;
//        return *this;
//    }
//
//    arg_nargs_t& arg_nargs_t::operator=(const char* s) {
//        decode(*s);
//        return *this;
//    }
//
//    void arg_nargs_t::decode(char c) {
//        if (c == '?')
//            flags = UNKNOWN;
//        else if (c == '+')
//            flags = ALL_REQUIRED;
//        else if (c == '*')
//            flags = ALL;
//        else
//            flags = 0;
//    }
//
//    arg_tokenizer_t::arg_tokenizer_t(size_t argc, const char** argv) {
//        for (size_t i = 0; i < argc; i++)
//            args.emplace_back(argv[i]);
//    }
//
//    bool arg_parse::validateArgument(const arg_properties_t& args) {
//        return !(args.a_flags.getFlags().empty() ^ args.a_flags.getNames().empty());
//    }
//
//    void arg_parse::addArgument(const arg_properties_t& args) {
//        if (validateArgument(args)) {
//            BLT_WARN("Argument contains both flags and positional names, this is not allowed!");
//            return;
//        }
//        // store one copy of the properties (arg properties could in theory be very large!)
//        auto pos = user_args.arg_storage.size();
//        user_args.arg_storage.push_back(args);
//        auto& arg = user_args.arg_storage[pos];
//        if (arg.a_dest.empty()) {
//            // update dest with first full flag (same as python's behaviour)
//            if (arg.a_flags.isFlag()) {
//                for (const auto& flag : arg.a_flags.getFlags()) {
//                    if (flag.starts_with("--")) {
//                        arg.a_dest = flag;
//                        break;
//                    }
//                }
//            } else {
//                for (const auto& name : arg.a_flags.getNames()) {
//                    arg.a_dest = name;
//                    break;
//                }
//            }
//        }
//        BLT_TRACE("Flag %s: %p", arg.a_dest.c_str(), &arg);
//        // associate the arg properties per name and flag to allow for quick access when parsing
//        auto& names = arg.a_flags.getNames();
//        for (const auto& name : names) {
//            user_args.name_associations.emplace_back(name, &arg);
//            user_args.required_vars.insert(name);
//        }
//
//        auto& flags = arg.a_flags.getFlags();
//        for (const auto& flag : flags)
//            user_args.flag_associations[flag] = &arg;
//
//        if (arg.a_required)
//            user_args.required_vars.insert(arg.a_dest);
//
//        if (arg.a_nargs.flags == arg_nargs_t::UNKNOWN) {
//            if (arg.a_flags.getNames().empty()) {
//                for (const auto& flag : arg.a_flags.getFlags())
//                    loaded_args.flag_args[flag] = arg.a_default;
//            } else {
//                for (const auto& name : arg.a_flags.getNames())
//                    loaded_args.positional_args[name] = arg.a_default;
//            }
//        }
//
//        BLT_DEBUG("PRINTING: %d, %d", user_args.arg_storage[pos].a_flags.getFlags().size(), user_args.arg_storage[pos].a_flags.getNames().size());
//
//    }
//
//    const arg_parse::arg_results& arg_parse::parse_args(int argc, const char** argv) {
//        loaded_args = {};
//        arg_tokenizer_t arg_tokenizer(argc, argv);
//        loaded_args.program_name = arg_tokenizer.next();
//
//        size_t last_positional = 0;
//        while (arg_tokenizer.hasNext()) {
//            // a token isn't a flag it must be a positional arg as flags will consume nargs
//            if (!arg_tokenizer.isFlag()){
//                handlePositionalArgument(arg_tokenizer, last_positional);
//                continue;
//            }
//            handleFlagArgument(arg_tokenizer);
//        }
//
//        if (loaded_args.unrecognized_args.empty())
//            return loaded_args;
//
//        std::string unrec;
//        for (const auto& r : loaded_args.unrecognized_args) {
//            unrec += '\'';
//            unrec += r;
//            unrec += '\'';
//            unrec += ' ';
//        }
//        unrec = unrec.substr(0, unrec.size()-1);
//        BLT_WARN("Unrecognized args: %s", unrec.c_str());
//        printHelp();
//
//        return loaded_args;
//    }
//
//    void arg_parse::printHelp() {
//
//    }
//
//    void arg_parse::handlePositionalArgument(arg_tokenizer_t& arg_tokenizer, size_t& last_pos) {
//        // TODO:
//        auto index = last_pos++;
//        if (index >= user_args.name_associations.size())
//            loaded_args.unrecognized_args.push_back(arg_tokenizer.next());
//        else
//            loaded_args.positional_args[user_args.name_associations[index].first] = arg_tokenizer.next();
//    }
//
//    void arg_parse::handleFlagArgument(arg_tokenizer_t& arg_tokenizer) {
//        // token is a flag, find out special information about it
//        auto flag = arg_tokenizer.next();
//
//        if (flag.starts_with("--"))
//            processFlag(arg_tokenizer, flag);
//        else {
//            // handle special args like -vvv
//            if (!flag.starts_with('-'))
//                BLT_TRACE("Flag processed but does not start with '-'!");
//            // size without -
//            auto len = flag.size()-1;
//            // get flag type
//            std::string str = "- ";
//            str[1] = flag[1];
//            for (size_t i = 0; i < len; i++)
//                processFlag(arg_tokenizer, str);
//        }
//    }
//
//    void arg_parse::processFlag(arg_tokenizer_t& arg_tokenizer, const std::string& flag) {
//        auto loc = user_args.flag_associations.find(flag);
//        if (loc == user_args.flag_associations.end()){
//            BLT_WARN("Flag '%s' not an option!", flag.c_str());
//            printHelp();
//            return;
//        }
//
//        auto* flag_properties = loc->second;
//        if (flag_properties->a_dest.empty()) {
//            BLT_ERROR("Flag dest empty! %s : %d, %p", flag.c_str(), flag_properties->a_flags.getFlags().size(), flag_properties);
//            return;
//        }
//        auto dest = flag_properties->a_dest;
//
//        arg_data_t& data = loaded_args.flag_args[dest];
//        switch(flag_properties->a_action){
//            case arg_action_t::HELP:
//                printHelp();
//                break;
//            case arg_action_t::STORE: {
//                std::vector<std::string> v;
//                if (!consumeArguments(arg_tokenizer, *flag_properties, v)) {
//                    printHelp();
//                    return;
//                }
//                if (v.size() == 1)
//                    data = v[0];
//                else
//                    data = v;
//                break;
//            }
//            case arg_action_t::STORE_CONST:
//                data = flag_properties->a_const;
//                break;
//            case arg_action_t::STORE_FALSE:
//                data = false;
//                break;
//            case arg_action_t::STORE_TRUE:
//                data = true;
//                break;
//            case arg_action_t::COUNT: {
//                if (!std::holds_alternative<int32_t>(data))
//                    data = 0;
//                data = std::get<int32_t>(data) + 1;
//                break;
//            }
//            case arg_action_t::EXTEND: {
//
//                break;
//            }
//            case arg_action_t::VERSION: {
//                auto file = filename(loaded_args.program_name);
//                BLT_INFO("%s, %s", file.c_str(), flag_properties->a_version.c_str());
//                break;
//            }
//            case arg_action_t::APPEND_CONST: {
//                if (!holds_alternative<std::vector<std::string>>(data)) {
//                    data = std::vector<std::string>();
//                }
//                auto& l = get<std::vector<std::string>>(data);
//                l.emplace_back(flag_properties->a_const);
//                break;
//            }
//            case arg_action_t::APPEND: {
//                if (!holds_alternative<std::vector<std::string>>(data))
//                    data = std::vector<std::string>();
//                auto& l = get<std::vector<std::string>>(data);
//                consumeArguments(arg_tokenizer, *flag_properties, l);
//                break;
//            }
//        }
//    }
//
//    bool arg_parse::consumeArguments(arg_tokenizer_t& arg_tokenizer, const arg_properties_t& properties, std::vector<std::string>& v_out) {
//        switch (properties.a_nargs.flags) {
//            case 0:
//                for (int i = 0; i < properties.a_nargs.args; i++) {
//                    if (!arg_tokenizer.hasNext()){
//                        BLT_WARN("Expected %d arguments got %d instead!", properties.a_nargs.args, i);
//                        return false;
//                    }
//                    if (arg_tokenizer.isFlag()) {
//                        BLT_WARN("Expected %d arguments, found flag instead!", properties.a_nargs.args);
//                        return false;
//                    }
//                    v_out.emplace_back(arg_tokenizer.next());
//                }
//                return true;
//            case arg_nargs_t::UNKNOWN:
//                // no arg next
//                if (!arg_tokenizer.hasNext() || arg_tokenizer.isFlag()) {
//                    if (!properties.a_const.empty())
//                        v_out.emplace_back(properties.a_const);
//                    else
//                        v_out.emplace_back(properties.a_default);
//                    return true;
//                }
//                v_out.emplace_back(arg_tokenizer.next());
//                return true;
//            case arg_nargs_t::ALL:
//                while (arg_tokenizer.hasNext() && !arg_tokenizer.isFlag())
//                    v_out.emplace_back(arg_tokenizer.next());
//                return true;
//            case arg_nargs_t::ALL_REQUIRED:
//                if (arg_tokenizer.isFlag()) {
//                    BLT_WARN("At least one argument is required!");
//                    return false;
//                }
//                while (arg_tokenizer.hasNext() && !arg_tokenizer.isFlag())
//                    v_out.emplace_back(arg_tokenizer.next());
//                return true;
//        }
//        return false;
//    }
//
//}