#pragma once
/*
 *  Copyright (C) 2024  Brett Terpstra
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

#ifndef BLT_PARSE_ARGPARSE_V2_H
#define BLT_PARSE_ARGPARSE_V2_H

#include <blt/std/types.h>
#include <blt/std/hashmap.h>
#include <blt/fs/path_helper.h>
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <optional>
#include <memory>
#include <functional>
#include <type_traits>
#include <blt/iterator/enumerate.h>
#include <blt/std/ranges.h>
#include <blt/std/utility.h>

namespace blt::argparse
{
    class argument_string_t;
    class argument_consumer_t;
    class argument_parser_t;
    class argument_subparser_t;
    class argument_builder_t;
    class argument_storage_t;

    enum class action_t
    {
        STORE,
        STORE_CONST,
        STORE_TRUE,
        STORE_FALSE,
        APPEND,
        APPEND_CONST,
        COUNT,
        HELP,
        VERSION,
        EXTEND,
        SUBCOMMAND
    };

    enum class nargs_t
    {
        IF_POSSIBLE,
        ALL,
        ALL_AT_LEAST_ONE
    };

    using nargs_v = std::variant<nargs_t, i32>;

    namespace detail
    {
        class bad_flag final : public std::runtime_error
        {
        public:
            explicit bad_flag(const std::string& message): std::runtime_error(message)
            {
            }

            explicit bad_flag(const char* message): std::runtime_error(message)
            {
            }
        };

        class missing_argument_error final : public std::runtime_error
        {
        public:
            explicit missing_argument_error(const std::string& message): std::runtime_error(message)
            {
            }
        }

        class subparse_error final : public std::exception
        {
        public:
            explicit subparse_error(const std::string_view found_string, std::vector<std::vector<std::string_view>> allowed_strings):
                m_found_string(found_string),
                m_allowed_strings(std::move(allowed_strings))
            {
            }

            [[nodiscard]] const std::vector<std::vector<std::string_view>>& get_allowed_strings() const
            {
                return m_allowed_strings;
            }

            [[nodiscard]] std::string_view get_found_string() const
            {
                return m_found_string;
            }

            [[nodiscard]] std::string error_string() const
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

            [[nodiscard]] const char* what() const override
            {
                return "Please use error_string() method instead of what(). This exception should *always* be caught!";
            }

        private:
            std::string_view m_found_string;
            std::vector<std::vector<std::string_view>> m_allowed_strings;
        };

        template <typename... Args>
        struct arg_data_helper_t
        {
            using arg_primitive_data_t = std::variant<Args...>;
            using arg_list_data_t = std::variant<std::vector<Args>...>;
        };

        using data_helper_t = arg_data_helper_t<i8, i16, i32, i64, u8, u16, u32, u64, float, double, std::string_view>;

        using arg_primitive_data_t = data_helper_t::arg_primitive_data_t;
        using arg_list_data_t = data_helper_t::arg_list_data_t;
        using arg_data_t = std::variant<arg_primitive_data_t, arg_list_data_t>;

        template <typename T>
        struct arg_type_t
        {
            static T convert(const std::string_view value)
            {
                static_assert(std::is_arithmetic_v<T> || std::is_same_v<T, std::string_view> || std::is_same_v<T, std::string>,
                              "Type must be arithmetic, string_view or string!");
                const std::string temp{value};

                if constexpr (std::is_same_v<T, float>)
                {
                    return std::stof(temp);
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return std::stod(temp);
                }
                else if constexpr (std::is_unsigned_v<T>)
                {
                    return static_cast<T>(std::stoull(temp));
                }
                else if constexpr (std::is_signed_v<T>)
                {
                    return static_cast<T>(std::stoll(temp));
                }
                else if constexpr (std::is_same_v<T, std::string_view>)
                {
                    return value;
                }
                else if constexpr (std::is_same_v<T, std::string>)
                {
                    return std::string(value);
                }
                BLT_UNREACHABLE;
            }
        };

        void test();
    }

    class argument_string_t
    {
    public:
        explicit argument_string_t(const char* input, const hashset_t<char>& allowed_flag_prefix): m_argument(input),
                                                                                                   allowed_flag_prefix(&allowed_flag_prefix)
        {
            if (input == nullptr)
                throw detail::bad_flag("Argument cannot be null!");
            process_argument();
        }

        [[nodiscard]] std::string_view get_flag() const
        {
            return m_flag_section;
        }

        [[nodiscard]] std::string_view get_name() const
        {
            return m_name_section;
        }

        [[nodiscard]] std::string_view value() const
        {
            return get_name();
        }

        [[nodiscard]] bool is_flag() const
        {
            return !m_flag_section.empty();
        }

        [[nodiscard]] std::string_view get_argument() const
        {
            return m_argument;
        }

    private:
        void process_argument()
        {
            size_t start = 0;
            for (; start < m_argument.size() && allowed_flag_prefix->contains(m_argument[start]); start++)
            {
            }

            m_flag_section = {m_argument.data(), start};
            m_name_section = {m_argument.data() + start, m_argument.size() - start};
        }

        std::string_view m_argument;
        std::string_view m_flag_section;
        std::string_view m_name_section;
        const hashset_t<char>* allowed_flag_prefix;
    };

    class argument_consumer_t
    {
    public:
        explicit argument_consumer_t(const span<argument_string_t>& args): m_args(args)
        {
        }

        [[nodiscard]] argument_string_t peek(const i32 offset = 0) const
        {
            return m_args[m_forward_index + offset];
        }

        argument_string_t consume()
        {
            return m_args[m_forward_index++];
        }

        [[nodiscard]] i32 position() const
        {
            return m_forward_index;
        }

        [[nodiscard]] i32 remaining() const
        {
            return static_cast<i32>(m_args.size()) - m_forward_index;
        }

        [[nodiscard]] bool has_next(const i32 offset = 0) const
        {
            return (offset + m_forward_index) < m_args.size();
        }

    private:
        span<argument_string_t> m_args;
        i32 m_forward_index = 0;
    };

    class argument_storage_t
    {
        friend argument_parser_t;
        friend argument_subparser_t;
        friend argument_builder_t;

    public:
        template <typename T>
        const T& get(const std::string_view key)
        {
            return std::get<T>(m_data[key]);
        }

        std::string_view get(const std::string_view key)
        {
            return std::get<std::string_view>(m_data[key]);
        }

        bool contains(const std::string_view key)
        {
            return m_data.find(key) != m_data.end();
        }

    private:
        hashmap_t<std::string_view, detail::arg_data_t> m_data;
    };

    class argument_builder_t
    {
        friend argument_parser_t;

    public:
        argument_builder_t()
        {
            dest_func = [](const std::string_view dest, argument_storage_t& storage, std::string_view value)
            {
                storage.m_data[dest] = value;
            };
        }

        template <typename T>
        argument_builder_t& as_type()
        {
            dest_func = [](const std::string_view dest, argument_storage_t& storage, std::string_view value)
            {
                storage.m_data[dest] = detail::arg_type_t<T>::convert(value);
            };
            return *this;
        }

    private:
        action_t action = action_t::STORE;
        bool required = false; // do we require this argument to be provided as an argument?
        nargs_v nargs = 1; // number of arguments to consume
        std::optional<std::string> metavar; // variable name to be used in the help string
        std::optional<std::string> help; // help string to be used in the help string
        std::optional<std::vector<std::string>> choices; // optional allowed choices for this argument
        std::optional<std::string> default_value;
        std::optional<std::string> const_value;
        // dest, storage, value input
        std::function<void(std::string_view, argument_storage_t&, std::string_view)> dest_func;
    };

    class argument_parser_t
    {
        friend argument_subparser_t;

    public:
        explicit argument_parser_t(const std::optional<std::string_view> name = {}, const std::optional<std::string_view> usage = {},
                                   const std::optional<std::string_view> description = {}, const std::optional<std::string_view> epilogue = {}):
            m_name(name), m_usage(usage), m_description(description), m_epilogue(epilogue)
        {
        }

        template <typename... Aliases>
        argument_builder_t& add_flag(const std::string_view arg, Aliases... aliases)
        {
            static_assert(
                std::conjunction_v<std::disjunction<std::is_convertible<Aliases, std::string_view>, std::is_constructible<
                                                        std::string_view, Aliases>>...>,
                "Arguments must be of type string_view, convertible to string_view or be string_view constructable");
            m_argument_builders.emplace_back();
            m_flag_arguments[arg] = &m_argument_builders.back();
            ((m_flag_arguments[std::string_view{aliases}] = &m_argument_builders.back()), ...);
            return m_argument_builders.back();
        }

        argument_builder_t& add_positional(const std::string_view arg)
        {
            m_argument_builders.emplace_back();
            m_positional_arguments[arg] = &m_argument_builders.back();
            return m_argument_builders.back();
        }

        argument_subparser_t& add_subparser(std::string_view dest);

        void parse(argument_consumer_t& consumer); // NOLINT

        void print_help();

        argument_parser_t& set_name(const std::string_view name)
        {
            m_name = name;
            return *this;
        }

        argument_parser_t& set_usage(const std::string_view usage)
        {
            m_usage = usage;
            return *this;
        }

        [[nodiscard]] const std::optional<std::string>& get_usage() const
        {
            return m_usage;
        }

        argument_parser_t& set_description(const std::string_view description)
        {
            m_description = description;
            return *this;
        }

        [[nodiscard]] const std::optional<std::string>& get_description() const
        {
            return m_description;
        }

        argument_parser_t& set_epilogue(const std::string_view epilogue)
        {
            m_epilogue = epilogue;
            return *this;
        }

        [[nodiscard]] const std::optional<std::string>& get_epilogue() const
        {
            return m_epilogue;
        }

    private:
        std::optional<std::string> m_name;
        std::optional<std::string> m_usage;
        std::optional<std::string> m_description;
        std::optional<std::string> m_epilogue;
        std::vector<std::pair<std::string_view, argument_subparser_t>> m_subparsers;
        std::vector<argument_builder_t> m_argument_builders;
        hashmap_t<std::string_view, argument_builder_t*> m_flag_arguments;
        hashmap_t<std::string_view, argument_builder_t*> m_positional_arguments;
    };

    class argument_subparser_t
    {
    public:
        explicit argument_subparser_t(const argument_parser_t& parent): m_parent(&parent)
        {
        }

        template <typename... Aliases>
        argument_parser_t& add_parser(const std::string_view name, Aliases... aliases)
        {
            static_assert(
                std::conjunction_v<std::disjunction<std::is_convertible<Aliases, std::string_view>, std::is_constructible<
                                                        std::string_view, Aliases>>...>,
                "Arguments must be of type string_view, convertible to string_view or be string_view constructable");
            m_parsers.emplace(name);
            ((m_aliases[std::string_view{aliases}] = &m_parsers[name]), ...);
            return m_parsers[name];
        }


        /**
         * Parses the next argument using the provided argument consumer.
         *
         * This function uses an argument consumer to extract and process the next argument.
         * If the argument is a flag or if it cannot be matched against the available parsers,
         * an exception is thrown.
         *
         * @param consumer Reference to an argument_consumer_t object, which handles argument parsing.
         *                  The consumer provides the next argument to be parsed.
         *
         * @throws detail::subparse_error If the argument is a flag or does not match any known parser.
         */
        argument_string_t parse(argument_consumer_t& consumer); // NOLINT

    private:
        [[nodiscard]] std::vector<std::vector<std::string_view>> get_allowed_strings() const;

        const argument_parser_t* m_parent;
        hashmap_t<std::string_view, argument_parser_t> m_parsers;
        hashmap_t<std::string_view, argument_parser_t*> m_aliases;
    };
}

#endif //BLT_PARSE_ARGPARSE_V2_H
