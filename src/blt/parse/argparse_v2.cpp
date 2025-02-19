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
#include <blt/meta/type_traits.h>
#include <blt/std/logging.h>

namespace blt::argparse
{
    constexpr static auto printer_primitive = [](const auto& v)
    {
        std::cout << v;
    };

    constexpr static auto printer_vector = [](const auto& v)
    {
        std::cout << "[";
        for (const auto& [i, a] : enumerate(v))
        {
            std::cout << a;
            if (i != v.size() - 1)
                std::cout << ", ";
        }
        std::cout << "]";
    };

    auto print_visitor = detail::arg_meta_type_helper_t::make_visitor(printer_primitive, printer_vector);

    template <typename T>
    size_t get_const_char_size(const T& t)
    {
        if constexpr (std::is_convertible_v<T, const char*>)
        {
            return std::char_traits<char>::length(t);
        }
        else if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char> || std::is_same_v<T, signed char>)
        {
            return 1;
        }
        else if constexpr (std::is_same_v<T, std::string_view> || std::is_same_v<T, std::string>)
        {
            return t.size();
        }
        else
        {
            return 0;
        }
    }

    template <typename T>
    auto ensure_is_string(T&& t)
    {
        if constexpr (std::is_arithmetic_v<meta::remove_cvref_t<T>>)
            return std::to_string(std::forward<T>(t));
        else
            return std::forward<T>(t);
    }

    template <typename... Strings>
    std::string make_string(Strings&&... strings)
    {
        std::string out;
        out.reserve((get_const_char_size(strings) + ...));
        ((out += ensure_is_string(std::forward<Strings>(strings))), ...);
        return out;
    }

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

        void test_argparse_empty()
        {
            std::vector<std::string> const argv;
            argument_parser_t parser;
            const auto args = parser.parse(argv);
            BLT_ASSERT(args.size() == 0 && "Empty argparse should have no args on output");
        }

        void test_single_flag_prefixes()
        {
            argument_parser_t parser;
            parser.add_flag("-a").set_action(action_t::STORE_TRUE);
            parser.add_flag("+b").set_action(action_t::STORE_FALSE);
            parser.add_flag("/c").as_type<int>().set_action(action_t::STORE);

            const std::vector<std::string> args = {"-a", "+b", "/c", "42"};
            const auto parsed_args = parser.parse(args);

            BLT_ASSERT(parsed_args.get<bool>("-a") == true && "Flag '-a' should store `true`");
            BLT_ASSERT(parsed_args.get<bool>("+b") == false && "Flag '+b' should store `false`");
            BLT_ASSERT(parsed_args.get<int>("/c") == 42 && "Flag '/c' should store the value 42");
        }

        // Test: Invalid flag prefixes
        void test_invalid_flag_prefixes()
        {
            argument_parser_t parser;
            parser.add_flag("-a");
            parser.add_flag("+b");
            parser.add_flag("/c");

            const std::vector<std::string> args = {"!d", "-a"};
            try
            {
                parser.parse(args);
                BLT_ASSERT(false && "Parsing should fail with invalid flag prefix '!'");
            }
            catch (...)
            {
                BLT_ASSERT(true && "Correctly threw on bad flag prefix");
            }
        }

        void test_compound_flags()
        {
            argument_parser_t parser;
            parser.add_flag("-v").as_type<int>().set_action(action_t::COUNT);

            const std::vector<std::string> args = {"-vvv"};
            const auto parsed_args = parser.parse(args);

            BLT_ASSERT(parsed_args.get<size_t>("-v") == 3 && "Flag '-v' should count occurrences in compound form");
        }

        void test_combination_of_valid_and_invalid_flags()
        {
            using namespace argparse;

            argument_parser_t parser;
            parser.add_flag("-x").as_type<int>();
            parser.add_flag("/y").as_type<std::string_view>();

            const std::vector<std::string> args = {"-x", "10", "!z", "/y", "value"};
            try
            {
                parser.parse(args);
                BLT_ASSERT(false && "Parsing should fail due to invalid flag '!z'");
            }
            catch (...)
            {
                BLT_ASSERT(true && "Correctly threw an exception for invalid flag");
            }
        }

        void test_flags_with_different_actions()
        {
            using namespace argparse;

            argument_parser_t parser;
            parser.add_flag("-k").as_type<int>().set_action(action_t::STORE); // STORE action
            parser.add_flag("-t").as_type<int>().set_action(action_t::STORE_CONST).set_const(999); // STORE_CONST action
            parser.add_flag("-f").set_action(action_t::STORE_FALSE); // STORE_FALSE action
            parser.add_flag("-c").set_action(action_t::STORE_TRUE); // STORE_TRUE action

            const std::vector<std::string> args = {"-k", "100", "-t", "-f", "-c"};
            const auto parsed_args = parser.parse(args);

            BLT_ASSERT(parsed_args.get<int>("-k") == 100 && "Flag '-k' should store 100");
            BLT_ASSERT(parsed_args.get<int>("-t") == 999 && "Flag '-t' should store a const value of 999");
            BLT_ASSERT(parsed_args.get<bool>("-f") == false && "Flag '-f' should store `false`");
            BLT_ASSERT(parsed_args.get<bool>("-c") == true && "Flag '-c' should store `true`");
        }

        void run_argparse_flag_tests()
        {
            test_single_flag_prefixes();
            test_invalid_flag_prefixes();
            test_compound_flags();
            test_combination_of_valid_and_invalid_flags();
            test_flags_with_different_actions();
        }

        void test()
        {
            run_all_tests_argument_string_t();
            test_argparse_empty();
            run_argparse_flag_tests();
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
        hashset_t<std::string> found_flags;
        hashset_t<std::string> found_positional;
        argument_storage_t parsed_args;
        // first, we consume flags which may be part of this parser
        while (consumer.can_consume() && consumer.peek().is_flag())
            handle_compound_flags(found_flags, parsed_args, consumer, consumer.consume());

        for (auto& [key, subparser] : m_subparsers)
        {
            auto [parsed_subparser, storage] = subparser.parse(consumer);
            storage.m_data.emplace(std::string{key}, detail::arg_data_t{parsed_subparser.get_argument()});
            parsed_args.add(storage);
        }

        while (consumer.can_consume())
        {
            const auto key = consumer.consume();
            if (key.is_flag())
                handle_compound_flags(found_flags, parsed_args, consumer, key);
            else
            {
                const auto pos = m_positional_arguments.find(key.get_argument());
                if (pos == m_positional_arguments.end())
                    throw detail::bad_positional(make_string("Error: Unknown positional argument: ", key.get_argument()));
                found_positional.insert(std::string{key.get_argument()});
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

    void argument_parser_t::print_version()
    {
    }

    void argument_parser_t::handle_compound_flags(hashset_t<std::string>& found_flags, argument_storage_t& parsed_args,
                                                  argument_consumer_t& consumer, const argument_string_t& arg)
    {
        // i kinda hate this, TODO?
        std::vector<std::string> compound_flags;
        if (arg.get_flag().size() == 1)
        {
            for (const auto c : arg.get_name())
                compound_flags.emplace_back(std::string{arg.get_flag()} + c);
        }
        else
        {
            if (arg.get_flag().size() > 2)
                throw detail::bad_flag(make_string("Error: Flag '", arg.get_argument(), "' is too long!"));
            compound_flags.emplace_back(arg.get_argument());
        }

        for (const auto& flag_key : compound_flags)
        {
            const auto flag = m_flag_arguments.find(flag_key);
            if (flag == m_flag_arguments.end())
                throw detail::bad_flag(make_string("Error: Unknown flag: ", flag_key));
            found_flags.insert(flag_key);
            parse_flag(parsed_args, consumer, flag_key);
        }
    }

    void argument_parser_t::parse_flag(argument_storage_t& parsed_args, argument_consumer_t& consumer, const std::string_view arg)
    {
        auto flag = m_flag_arguments.find(arg)->second;
        const auto dest = flag->m_dest.value_or(std::string{arg});
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
                           case nargs_t::ALL_AT_LEAST_ONE:
                               if (!consumer.can_consume())
                                   throw detail::missing_argument_error(
                                       make_string("Error expected at least one argument to be consumed by '", arg, '\''));
                               [[fallthrough]];
                           case nargs_t::ALL:
                               std::vector<std::string_view> args;
                               while (consumer.can_consume() && !consumer.peek().is_flag())
                                   args.emplace_back(consumer.consume().get_argument());
                               flag->m_dest_vec_func(dest, parsed_args, args);
                               break;
                           }
                       },
                       [&parsed_args, &consumer, &dest, &flag, arg, this](const i32 argc)
                       {
                           std::vector<std::string_view> args;
                           for (i32 i = 0; i < argc; ++i)
                           {
                               if (!consumer.can_consume())
                               {
                                   throw detail::missing_argument_error(
                                       make_string("Expected ", argc, " arguments to be consumed by '", arg, "' but found ", i));
                               }
                               if (consumer.peek().is_flag())
                               {
                                   throw detail::unexpected_argument_error(make_string(
                                       "Expected ", argc, " arguments to be consumed by '", arg, "' but found a flag '",
                                       consumer.peek().get_argument(), "' instead!"
                                   ));
                               }
                               args.push_back(consumer.consume().get_argument());
                           }
                           if (flag->m_choices)
                           {
                               auto& choices = *flag->m_choices;
                               for (const auto str : args)
                               {
                                   if (!choices.contains(str))
                                   {
                                       std::string valid_choices = "{";
                                       for (const auto& [i, choice] : enumerate(choices))
                                       {
                                           valid_choices += choice;
                                           if (i != choices.size() - 1)
                                               valid_choices += ", ";
                                       }
                                       valid_choices += "}";
                                       throw detail::bad_choice_error(make_string('\'', str, "' is not a valid choice for argument '", arg,
                                                                                  "'! Expected one of ", valid_choices));
                                   }
                               }
                           }
                           if (args.size() != static_cast<size_t>(argc))
                           {
                               throw std::runtime_error(
                                   "This error condition should not be possible. "
                                   "Args consumed didn't equal the arguments requested and previous checks didn't fail. "
                                   "Please report as an issue on the GitHub");
                           }

                           switch (flag->m_action)
                           {
                           case action_t::STORE:
                               if (argc == 0)
                                   throw detail::missing_argument_error(
                                       make_string("Argument '", arg, "'s action is store but takes in no arguments?"));
                               if (argc == 1)
                                   flag->m_dest_func(dest, parsed_args, args.front());
                               else
                                   throw detail::unexpected_argument_error(make_string("Argument '", arg,
                                                                                       "'s action is store but takes in more than one argument. "
                                                                                       "Did you mean to use action_t::APPEND or action_t::EXTEND?"));
                               break;
                           case action_t::APPEND:
                           case action_t::EXTEND:
                               if (argc == 0)
                                   throw detail::missing_argument_error(
                                       make_string("Argument '", arg, "'s action is append or extend but takes in no arguments."));
                               flag->m_dest_vec_func(dest, parsed_args, args);
                               break;
                           case action_t::APPEND_CONST:
                               if (argc != 0)
                                   throw detail::unexpected_argument_error(
                                       make_string("Argument '", arg, "'s action is append const but takes in arguments."));
                               if (flag->m_const_value)
                               {
                                   throw detail::missing_value_error(
                                       make_string("Append const chosen as an action but const value not provided for argument '", arg, '\''));
                               }
                               if (parsed_args.contains(dest))
                               {
                                   auto& data = parsed_args.m_data[dest];
                                   auto visitor = detail::arg_meta_type_helper_t::make_visitor(
                                       [arg](auto& primitive)
                                       {
                                           throw detail::type_error(make_string("Invalid type for argument '", arg, "' expected list type, found '",
                                                                                blt::type_string<decltype(primitive)>(), "' with value ", primitive));
                                       },
                                       [&flag, arg](auto& vec)
                                       {
                                           using type = typename meta::remove_cvref_t<decltype(vec)>::value_type;
                                           if (!std::holds_alternative<type>(*flag->m_const_value))
                                           {
                                               throw detail::type_error(make_string("Constant value for argument '", arg,
                                                                                    "' type doesn't match values already present! Expected to be of type '",
                                                                                    blt::type_string<type>(), "'!"));
                                           }
                                           vec.push_back(std::get<type>(*flag->m_const_value));
                                       });
                                   std::visit(visitor, data);
                               }
                               else
                               {
                                   auto visitor = detail::arg_meta_type_helper_t::make_visitor(
                                       [&parsed_args, &dest](auto& primitive)
                                       {
                                           std::vector<meta::remove_cvref_t<decltype(primitive)>> vec;
                                           vec.push_back(primitive);
                                           parsed_args.m_data.insert({dest, std::move(vec)});
                                       },
                                       [](auto&)
                                       {
                                           throw detail::type_error("Append const should not be a list type!");
                                       });
                                   std::visit(visitor, *flag->m_const_value);
                               }
                               break;
                           case action_t::STORE_CONST:
                               if (argc != 0)
                               {
                                   print_usage();
                                   throw detail::unexpected_argument_error(
                                       make_string("Argument '", arg, "' is store const but called with an argument."));
                               }
                               if (!flag->m_const_value)
                                   throw detail::missing_value_error(
                                       make_string("Argument '", arg, "' is store const, but const storage has no value."));
                               parsed_args.m_data.emplace(dest, *flag->m_const_value);
                               break;
                           case action_t::STORE_TRUE:
                               if (argc != 0)
                               {
                                   print_usage();
                                   throw detail::unexpected_argument_error("Store true flag called with an argument.");
                               }
                               parsed_args.m_data.emplace(dest, true);
                               break;
                           case action_t::STORE_FALSE:
                               if (argc != 0)
                               {
                                   print_usage();
                                   throw detail::unexpected_argument_error("Store false flag called with an argument.");
                               }
                               parsed_args.m_data.insert({dest, false});
                               break;
                           case action_t::COUNT:
                               if (parsed_args.m_data.contains(dest))
                               {
                                   auto visitor = detail::arg_meta_type_helper_t::make_visitor(
                                       [](auto& primitive) -> detail::arg_data_t
                                       {
                                           using type = meta::remove_cvref_t<decltype(primitive)>;
                                           if constexpr (std::is_convertible_v<decltype(1), type>)
                                           {
                                               return primitive + static_cast<type>(1);
                                           }
                                           else
                                               throw detail::type_error("Error: count called but stored type is " + blt::type_string<type>());
                                       },
                                       [](auto&) -> detail::arg_data_t
                                       {
                                           throw detail::type_error("List present on count. This condition doesn't make any sense! "
                                               "(How did we get here, please report this!)");
                                       }
                                   );
                                   parsed_args.m_data[dest] = std::visit(visitor, parsed_args.m_data[dest]);
                               }
                               else // I also hate this!
                                   flag->m_dest_func(dest, parsed_args, "1");
                               break;
                           case action_t::HELP:
                               print_help();
                               std::exit(1);
                           case action_t::VERSION:
                               print_version();
                               break;
                           }
                       }
                   }, flag->m_nargs);
    }

    void argument_parser_t::parse_positional(argument_storage_t& parsed_args, argument_consumer_t& consumer, const std::string_view arg)
    {
    }

    void argument_parser_t::handle_missing_and_default_args(hashmap_t<std::string_view, argument_builder_t*>& arguments,
                                                            const hashset_t<std::string>& found, argument_storage_t& parsed_args,
                                                            const std::string_view type)
    {
        for (const auto& [key, value] : arguments)
        {
            if (!found.contains(key))
            {
                if (value->m_required)
                    throw detail::missing_argument_error(make_string("Error: ", type, " argument '", key,
                                                                     "' was not found but is required by the program"));
                auto dest = value->m_dest.value_or(std::string{key});
                if (value->m_default_value && !parsed_args.contains(dest))
                    parsed_args.m_data.emplace(dest, *value->m_default_value);
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
