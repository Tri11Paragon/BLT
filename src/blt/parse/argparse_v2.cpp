/*
 *  <Short Description>
 *  Copyright (C) 2025  Brett Terpstra
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
#include <iostream>
#include <blt/parse/argparse_v2.h>
#include <blt/std/assert.h>

namespace blt::argparse
{
    namespace detail
    {
        // Unit Tests for class argument_string_t
        // Test Case 1: Ensure the constructor handles flags correctly
        void test_argument_string_t_flag_basic(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("-f", prefixes);
            BLT_ASSERT(arg.is_flag() && "Expected argument to be identified as a flag.");
            BLT_ASSERT(arg.value() == "f" && "Flag value should match the input string.");
        }

        // Test Case 2: Ensure the constructor handles long flags correctly
        void test_argument_string_t_long_flag(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("--file", prefixes);
            BLT_ASSERT(arg.is_flag() && "Expected argument to be identified as a flag.");
            BLT_ASSERT(arg.value() == "file" && "Long flag value should match the input string.");
        }

        // Test Case 3: Ensure positional arguments are correctly identified
        void test_argument_string_t_positional_argument(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("filename.txt", prefixes);
            BLT_ASSERT(!arg.is_flag() && "Expected argument to be identified as positional.");
            BLT_ASSERT(arg.value() == "filename.txt" && "Positional argument value should match the input string.");
        }

        // Test Case 5: Handle an empty string
        void test_argument_string_t_empty_input(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("", prefixes);
            BLT_ASSERT(!arg.is_flag() && "Expected an empty input to be treated as positional, not a flag.");
            BLT_ASSERT(arg.value().empty() && "Empty input should have an empty value.");
        }

        // Test Case 6: Handle edge case of a single hyphen (`-`) which might be ambiguous
        void test_argument_string_t_single_hyphen(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("-", prefixes);
            BLT_ASSERT(arg.is_flag() && "Expected single hyphen (`-`) to be treated as a flag.");
            BLT_ASSERT(arg.value().empty() && "Single hyphen flag should have empty value.");
            BLT_ASSERT(arg.get_flag() == "-" && "Single hyphen flag should match the input string.");
        }

        // Test Case 8: Handle arguments with prefix only (like "--")
        void test_argument_string_t_double_hyphen(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("--", prefixes);
            BLT_ASSERT(arg.is_flag() && "Double hyphen ('--') should be treated as a flag.");
            BLT_ASSERT(arg.value().empty() && "Double hyphen flag should have empty value.");
            BLT_ASSERT(arg.get_flag() == "--" && "Double hyphen value should match the input string.");
        }

        // Test Case 9: Validate edge case of an argument with spaces
        void test_argument_string_t_with_spaces(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("  ", prefixes);
            BLT_ASSERT(!arg.is_flag() && "Arguments with spaces should not be treated as flags.");
            BLT_ASSERT(arg.value() == "  " && "Arguments with spaces should match the input string.");
        }

        // Test Case 10: Validate arguments with numeric characters
        void test_argument_string_t_numeric_flag(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("-123", prefixes);
            BLT_ASSERT(arg.is_flag() && "Numeric flags should still be treated as flags.");
            BLT_ASSERT(arg.value() == "123" && "Numeric flag value should match the input string.");
        }

        // Test Case 11: Ensure the constructor handles '+' flag correctly
        void test_argument_string_t_plus_flag_basic(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("+f", prefixes);
            BLT_ASSERT(arg.is_flag() && "Expected argument to be identified as a flag.");
            BLT_ASSERT(arg.value() == "f" && "Plus flag value should match the input string.");
        }

        // Test Case 13: Handle edge case of a single plus (`+`) which might be ambiguous
        void test_argument_string_t_single_plus(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("+", prefixes);
            BLT_ASSERT(arg.is_flag() && "Expected single plus (`+`) to be treated as a flag.");
            BLT_ASSERT(arg.value().empty() && "Single plus flag should have empty value.");
            BLT_ASSERT(arg.get_flag() == "+" && "Single plus flag should match the input string.");
        }

        // Test Case 14: Handle arguments with prefix only (like '++')
        void test_argument_string_t_double_plus(const hashset_t<char>& prefixes)
        {
            const argument_string_t arg("++", prefixes);
            BLT_ASSERT(arg.is_flag() && "Double plus ('++') should be treated as a flag.");
            BLT_ASSERT(arg.value().empty() && "Double plus flag should have empty value.");
            BLT_ASSERT(arg.get_flag() == "++" && "Double plus value should match the input string.");
        }


        void run_all_tests_argument_string_t()
        {
            const hashset_t<char> prefixes = {'-', '+'};
            test_argument_string_t_flag_basic(prefixes);
            test_argument_string_t_long_flag(prefixes);
            test_argument_string_t_positional_argument(prefixes);
            test_argument_string_t_empty_input(prefixes);
            test_argument_string_t_single_hyphen(prefixes);
            test_argument_string_t_double_hyphen(prefixes);
            test_argument_string_t_with_spaces(prefixes);
            test_argument_string_t_numeric_flag(prefixes);
            test_argument_string_t_plus_flag_basic(prefixes);
            test_argument_string_t_single_plus(prefixes);
            test_argument_string_t_double_plus(prefixes);
        }

        void test()
        {
            run_all_tests_argument_string_t();
        }

        [[nodiscard]] std::string subparse_error::error_string() const
        {
            std::string message = "Subparser Error: ";
            message += m_found_string;
            message += " is not a valid command. Allowed commands are: {";
            for (const auto [i, allowed_string] : enumerate(m_allowed_strings))
            {
                if (allowed_string.size() > 1)
                    message += '[';
                for (const auto [j, alias] : enumerate(allowed_string))
                {
                    message += alias;
                    if (j < alias.size() - 2)
                        message += ", ";
                    else if (j < alias.size())
                        message += ", or ";
                }
                if (allowed_string.size() > 1)
                    message += ']';
                if (i != m_allowed_strings.size() - 1)
                    message += ' ';
            }
            message += "}";
            return message;
        }
    }

    argument_subparser_t& argument_parser_t::add_subparser(const std::string_view dest)
    {
        m_subparsers.emplace_back(dest, argument_subparser_t{*this});
        return m_subparsers.back().second;
    }

    argument_storage_t argument_parser_t::parse(argument_consumer_t& consumer)
    {
        hashset_t<std::string_view> found_flags;
        hashset_t<std::string_view> found_positional;
        argument_storage_t parsed_args;
        // first, we consume flags which may be part of this parser
        while (consumer.can_consume() && consumer.peek().is_flag())
        {
            const auto key = consumer.consume();
            const auto flag = m_flag_arguments.find(key.get_argument());
            if (flag == m_flag_arguments.end())
            {
                std::cerr << "Error: Unknown flag: " << key.get_argument() << std::endl;
                exit(1);
            }
            found_flags.insert(key.get_argument());
            parse_flag(parsed_args, consumer, key.get_argument());
        }
        try
        {
            for (auto& [key, subparser] : m_subparsers)
            {
                auto [parsed_subparser, storage] = subparser.parse(consumer);
                storage.m_data.insert({key, detail::arg_data_t{parsed_subparser.get_argument()}});
                parsed_args.add(storage);
            }
        }
        catch (const detail::missing_argument_error& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            print_usage();
            exit(1);
        } catch (const detail::subparse_error& e)
        {
            std::cerr << e.error_string() << std::endl;
            exit(1);
        }
        while (consumer.can_consume())
        {
            const auto key = consumer.consume();
            if (key.is_flag())
            {
                const auto flag = m_flag_arguments.find(key.get_argument());
                if (flag == m_flag_arguments.end())
                {
                    std::cerr << "Error: Unknown flag: " << key.get_argument() << std::endl;
                    exit(1);
                }
                found_flags.insert(key.get_argument());
                parse_flag(parsed_args, consumer, key.get_argument());
            }
            else
            {
                const auto pos = m_positional_arguments.find(key.get_argument());
                if (pos == m_positional_arguments.end())
                {
                    std::cerr << "Error: Unknown positional argument: " << key.get_argument() << std::endl;
                    exit(1);
                }
                found_positional.insert(key.get_argument());
                parse_positional(parsed_args, consumer, key.get_argument());
            }
        }
        handle_missing_and_default_args(m_flag_arguments, found_flags, parsed_args, "flag");
        handle_missing_and_default_args(m_positional_arguments, found_positional, parsed_args, "positional");

        return parsed_args;
    }

    void argument_parser_t::print_help()
    {
    }

    void argument_parser_t::print_usage()
    {
    }

    void argument_parser_t::parse_flag(argument_storage_t& parsed_args, argument_consumer_t& consumer, const std::string_view arg)
    {
        auto& flag = m_flag_arguments[arg];
        auto dest = flag->m_dest.value_or(arg);
        std::visit(lambda_visitor{
                       [&parsed_args, &consumer, &dest, &flag, arg](const nargs_t arg_enum)
                       {
                           switch (arg_enum)
                           {
                           case nargs_t::IF_POSSIBLE:
                               if (consumer.can_consume() && !consumer.peek().is_flag())
                                   flag->m_dest_func(dest, parsed_args, consumer.consume().get_argument());
                               else
                               {
                                   if (flag->m_const_value)
                                       parsed_args.m_data.insert({dest, *flag->m_const_value});
                               }
                               break;
                           [[fallthrough]] case nargs_t::ALL_AT_LEAST_ONE:
                               if (!consumer.can_consume())
                                   std::cout << "Error expected at least one argument to be consumed by '" << arg << '\'' << std::endl;
                           case nargs_t::ALL:
                               std::vector<std::string_view> args;
                               while (consumer.can_consume() && !consumer.peek().is_flag())
                                   args.emplace_back(consumer.consume().get_argument());
                               flag->m_dest_vec_func(dest, parsed_args, args);
                               break;
                           }
                       },
                       [](const i32 argc)
                       {
                       }
                   }, flag->m_nargs);
    }

    void argument_parser_t::parse_positional(argument_storage_t& parsed_args, argument_consumer_t& consumer, const std::string_view arg)
    {
    }

    void argument_parser_t::handle_missing_and_default_args(hashmap_t<std::string_view, argument_builder_t*>& arguments,
                                                            const hashset_t<std::string_view>& found, argument_storage_t& parsed_args,
                                                            const std::string_view type)
    {
        for (const auto& [key, value] : arguments)
        {
            if (!found.contains(key))
            {
                if (value->m_required)
                {
                    std::cerr << "Error: " << type << " argument '" << key << "' was not found but is required by the program" << std::endl;
                    exit(1);
                }
                auto dest = value->m_dest.value_or(key);
                if (value->m_default_value && !parsed_args.contains(dest))
                    parsed_args.m_data.insert({dest, *value->m_default_value});
            }
        }
    }

    std::pair<argument_string_t, argument_storage_t> argument_subparser_t::parse(argument_consumer_t& consumer)
    {
        if (!consumer.can_consume())
            throw detail::missing_argument_error("Subparser requires an argument.");
        const auto key = consumer.consume();
        if (key.is_flag())
            throw detail::subparse_error(key.get_argument(), get_allowed_strings());
        const auto it = m_parsers.find(key.get_name());
        argument_parser_t* parser;
        if (it == m_parsers.end())
        {
            const auto it2 = m_aliases.find(key.get_name());
            if (it2 == m_aliases.end())
                throw detail::subparse_error(key.get_argument(), get_allowed_strings());
            parser = it2->second;
        }
        else
            parser = &it->second;
        parser->m_name = m_parent->m_name;
        return {key, parser->parse(consumer)};
    }

    std::vector<std::vector<std::string_view>> argument_subparser_t::get_allowed_strings() const
    {
        std::vector<std::vector<std::string_view>> vec;
        for (const auto& [key, value] : m_parsers)
        {
            std::vector<std::string_view> aliases;
            aliases.push_back(key);
            for (const auto& [alias, parser] : m_aliases)
            {
                if (parser == &value)
                    aliases.push_back(alias);
            }
            vec.emplace_back(std::move(aliases));
        }
        return vec;
    }
}
