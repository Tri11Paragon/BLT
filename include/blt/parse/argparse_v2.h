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
    class parsed_argset_t;
    class argument_builder_t;

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

        template <typename... Args>
        struct arg_data_helper_t
        {
            using arg_primitive_data_t = std::variant<Args...>;
            using arg_list_data_t = std::variant<std::vector<Args>...>;
        };

        using data_helper_t = arg_data_helper_t<i8, i16, i32, i64, u8, u16, u32, u64, float, double>;

        using arg_primitive_data_t = data_helper_t::arg_primitive_data_t;
        using arg_list_data_t = data_helper_t::arg_list_data_t;
        using arg_data_t = std::variant<arg_primitive_data_t, arg_list_data_t>;

        template <typename T>
        struct arg_type_t
        {
            static T convert(const std::string_view value)
            {
                static_assert(std::is_arithmetic_v<T>, "Type must be arithmetic!");
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

    class argument_builder_t
    {
    public:

    private:
    };

    class parsed_argset_t
    {
    public:

    private:
    };

    class argument_parser_t
    {
    public:
        explicit argument_parser_t(const std::optional<std::string_view> name = {}, const std::optional<std::string_view> usage = {}):
            m_name(name), m_usage(usage)
        {
        }

        argument_parser_t& set_name(const std::string_view name)
        {
            m_name = name;
            return *this;
        }

    private:
        std::optional<std::string> m_name;
        std::optional<std::string> m_usage;
        std::optional<std::string> m_description;
        std::optional<std::string> m_epilogue;
    };

    class argument_subparser_t
    {
    public:

        argument_parser_t add_subparser(const std::string_view dest)
        {

        }

        argument_parser_t add_subparser(std::string& dest)
        {

        }
    private:
    };
}

#endif //BLT_PARSE_ARGPARSE_V2_H
