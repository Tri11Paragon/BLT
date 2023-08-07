/*
 * Created by Brett on 06/08/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <blt/parse/argparse_2.h>
#include <blt/std/logging.h>
#include <blt/std/string.h>

namespace blt
{
    
    void arg_nargs_t::decode(char c)
    {
        if (c == '?')
            flags = UNKNOWN;
        else if (c == '+')
            flags = ALL_REQUIRED;
        else if (c == '*')
            flags = ALL;
        else
            flags = 0;
    }
    
    arg_nargs_t::arg_nargs_t(char c)
    {
        decode(c);
    }
    
    arg_nargs_t::arg_nargs_t(std::string s)
    {
        decode(s[0]);
    }
    
    arg_nargs_t::arg_nargs_t(const char* s)
    {
        decode(*s);
    }
    
    class invalid_argument_exception : public std::runtime_error
    {
        public:
            invalid_argument_exception(const std::string& str): std::runtime_error(str)
            {}
    };
    
    void arg_vector_t::validateFlags()
    {
        for (const auto& flag : flags)
            if (!flag.starts_with('-'))
                throw invalid_argument_exception("Flag '" + flag + "' must start with - or --");
    }
    
    arg_vector_t::arg_vector_t(const char* str) {
        std::string as_string(str);
        if (as_string.starts_with('-'))
            flags.emplace_back(as_string);
        else
            name = as_string;
    }
    arg_vector_t::arg_vector_t(const std::string& str) {
        if (str.starts_with('-'))
            flags.emplace_back(str);
        else
            name = str;
    }
    
    std::string to_string(const arg_data_t& v)
    {
        if (holds_alternative<arg_data_internal_t>(v))
            return to_string(std::get<arg_data_internal_t>(v));
        else if (std::holds_alternative<arg_data_vec_t>(v))
        {
            const auto& vec = std::get<arg_data_vec_t>(v);
            if (vec.size() == 1)
                return to_string(vec[0]);
            if (vec.empty())
                return "Empty Vector";
            std::string str;
            for (const auto& r : vec)
            {
                str += to_string(r);
                str += ' ';
            }
            return "Vector of contents: " + str;
        }
        return "Empty";
    }
    
    std::string to_string(const arg_data_internal_t& v)
    {
        if (std::holds_alternative<std::string>(v)){
            return std::get<std::string>(v);
        } else if (std::holds_alternative<bool>(v)) {
            return std::get<bool>(v) ? "True" : "False";
        }
        return std::to_string(std::get<int32_t>(v));
    }
    
    std::string arg_parse::filename(const std::string& path)
    {
        auto paths = blt::string::split(path, "/");
        auto final = paths[paths.size() - 1];
        if (final == "/")
            return paths[paths.size() - 2];
        return final;
    }
    
    void arg_parse::addArgument(const arg_properties_t& args)
    {
        auto properties = new arg_properties_t(args);
        
        // determine where to store the arg when parsing
        if (properties->a_dest.empty())
        {
            if (properties->a_flags.isFlag())
            {
                // take first arg so a_dest exists, could be - or --
                properties->a_dest = properties->a_flags.flags[0];
                // look for a -- arg (python's behaviour)
                for (const auto& flag : properties->a_flags.flags)
                {
                    if (flag.starts_with("--"))
                    {
                        properties->a_dest = flag;
                        break;
                    }
                }
            } else
                properties->a_dest = properties->a_flags.name;
        }
        
        // associate flags with their properties
        for (const auto& flag : properties->a_flags.flags)
            user_args.flag_associations[flag] = properties;
        
        // positional args uses index (vector) to store the properties
        if (!properties->a_flags.isFlag())
            user_args.name_associations.push_back(properties);
        
        user_args.arg_properties_storage.push_back(properties);
    }
    
    bool arg_parse::consumeArguments(arg_tokenizer& tokenizer, const arg_properties_t& properties, std::vector<arg_data_internal_t>& v_out)
    {
        switch (properties.a_nargs.flags)
        {
            case 0:
                for (int i = 0; i < properties.a_nargs.args; i++)
                {
                    // if we don't have another arg to consume we have a problem!
                    if (!tokenizer.hasCurrent())
                    {
                        BLT_WARN("Expected %d arguments got %d instead!", properties.a_nargs.args, i);
                        return false;
                    }
                    // if we do have one, but it is a flag then we also have a problem!
                    if (tokenizer.isFlag())
                    {
                        BLT_WARN("Expected %d arguments, found flag instead!", properties.a_nargs.args);
                        return false;
                    }
                    // get the value and advance
                    v_out.emplace_back(tokenizer.get());
                    tokenizer.advance();
                }
                return true;
            case arg_nargs_t::UNKNOWN:
                // no arg next
                if (!tokenizer.hasCurrent() || tokenizer.isFlag())
                {
                    // python's default is to store const if around otherwise store default
                    if (!properties.a_const.empty())
                        v_out.emplace_back(properties.a_const);
                    else // this should no longer be required with the changes to how defaults are handled
                        v_out.emplace_back(properties.a_default);
                    return true;
                }
                v_out.emplace_back(tokenizer.get());
                tokenizer.advance();
                return true;
            case arg_nargs_t::ALL:
                while (tokenizer.hasCurrent() && !tokenizer.isFlag())
                {
                    v_out.emplace_back(tokenizer.get());
                    tokenizer.advance();
                }
                return true;
            case arg_nargs_t::ALL_REQUIRED:
                if (tokenizer.hasCurrent() && tokenizer.isFlag())
                {
                    BLT_WARN("At least one argument is required!");
                    return false;
                }
                while (tokenizer.hasCurrent() && !tokenizer.isFlag())
                {
                    v_out.emplace_back(tokenizer.get());
                    tokenizer.advance();
                }
                return true;
        }
        return false;
    }
    
    void arg_parse::handlePositionalArgument(arg_tokenizer& tokenizer, size_t& last_pos)
    {
        auto index = last_pos++;
        if (index >= user_args.name_associations.size())
            loaded_args.unrecognized_args.push_back(tokenizer.get());
        else
            loaded_args.data[user_args.name_associations[index]->a_dest] = tokenizer.get();
        tokenizer.advance();
    }
    
    void arg_parse::handleFlagArgument(arg_tokenizer& tokenizer)
    {
        auto flag = tokenizer.get();
        tokenizer.advance();
        
        // token is a flag, figure out how to handle it
        if (flag.starts_with("--"))
            processFlag(tokenizer, flag);
        else
        {
            // handle special args like -vvv
            if (!flag.starts_with('-'))
                BLT_ERROR("Flag processed but does not start with '-'");
            // make sure the flag only contains the same character
            auto type = flag[1];
            for (char c : flag.substr(1))
            {
                if (c != type)
                {
                    BLT_ERROR("Processed flag '%s' expected %c found %c", flag.c_str(), type, c);
                    return;
                }
            }
            // size without -
            auto len = flag.size() - 1;
            // get flag type
            std::string str = "- ";
            str[1] = flag[1];
            for (size_t i = 0; i < len; i++)
                processFlag(tokenizer, str);
        }
    }
    
    void arg_parse::processFlag(arg_tokenizer& tokenizer, const std::string& flag)
    {
        auto flag_itr = user_args.flag_associations.find(flag);
        if (flag_itr == user_args.flag_associations.end())
        {
            loaded_args.unrecognized_args.push_back(flag);
            return;
        }
        
        const auto* const properties = user_args.flag_associations.at(flag);
        
        
        if (properties->a_dest.empty())
        {
            loaded_args.unrecognized_args.push_back(flag);
            return;
        }
        auto dest = properties->a_dest;
        if (dest.starts_with("--"))
            dest = dest.substr(2);
        else if (dest.starts_with('-'))
            dest = dest.substr(1);
        
        switch (properties->a_action)
        {
            case arg_action_t::HELP:
                printHelp();
                break;
            case arg_action_t::STORE:
            {
                arg_data_t& data = loaded_args.data[dest];
                arg_data_vec_t v;
                if (!consumeArguments(tokenizer, *properties, v))
                {
                    printHelp();
                    return;
                }
                if (v.size() == 1)
                    data = v[0];
                else
                    data = v;
                break;
            }
            case arg_action_t::STORE_CONST:
                loaded_args.data[dest] = properties->a_const;
                break;
            case arg_action_t::STORE_FALSE:
                loaded_args.data[dest] = false;
                break;
            case arg_action_t::STORE_TRUE:
                loaded_args.data[dest] = true;
                break;
            case arg_action_t::COUNT:
            {
                auto& data = loaded_args.data[dest];
                if (!holds_alternative<int32_t>(data))
                    data = 0;
                data = get<int32_t>(data) + 1;
                break;
            }
            case arg_action_t::EXTEND:
            {
                
                break;
            }
            case arg_action_t::VERSION:
            {
                auto file = filename(loaded_args.program_name);
                BLT_INFO("%s, %s", file.c_str(), properties->a_version.c_str());
                break;
            }
            case arg_action_t::APPEND_CONST:
            {
                auto& data = loaded_args.data[dest];
                if (!std::holds_alternative<arg_data_vec_t>(data))
                {
                    data = arg_data_vec_t();
                }
                auto& l = get<arg_data_vec_t>(data);
                l.emplace_back(properties->a_const);
                break;
            }
            case arg_action_t::APPEND:
            {
                auto& data = loaded_args.data[dest];
                if (!holds_alternative<arg_data_vec_t>(data))
                    data = arg_data_vec_t();
                auto& l = get<arg_data_vec_t>(data);
                consumeArguments(tokenizer, *properties, l);
                break;
            }
        }
    }
    
    arg_parse::arg_results arg_parse::parse_args(int argc, const char** argv)
    {
        std::vector<std::string> args;
        args.reserve(argc);
        for (int i = 0; i < argc; i++)
            args.emplace_back(argv[i]);
        return parse_args(args);
    }
    
    arg_parse::arg_results arg_parse::parse_args(const std::vector<std::string>& args)
    {
        arg_tokenizer tokenizer(args);
        loaded_args.program_name = tokenizer.get();
        tokenizer.advance();
        
        size_t last_positional = 0;
        while (tokenizer.hasCurrent())
        {
            if (tokenizer.isFlag())
                handleFlagArgument(tokenizer);
            else
                handlePositionalArgument(tokenizer, last_positional);
        }
        
        // if there was no problems processing then return the loaded args
        if (loaded_args.unrecognized_args.empty())
            return loaded_args;
        
        // otherwise construct a string detailing the unrecognized args
        std::string unrec;
        for (const auto& r : loaded_args.unrecognized_args)
        {
            unrec += '\'';
            unrec += r;
            unrec += '\'';
            unrec += ' ';
        }
        // remove the last space caused by the for loop
        unrec = unrec.substr(0, unrec.size() - 1);
        // TODO: use exceptions?
        BLT_WARN("Unrecognized args: %s", unrec.c_str());
        printHelp();
        
        return loaded_args;
    }
    
    void arg_parse::printHelp()
    {
        BLT_TRACE("I am helpful!");
        std::exit(0);
    }
}