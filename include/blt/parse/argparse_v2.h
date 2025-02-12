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
#include <string>
#include <string_view>
#include <vector>
#include <variant>
#include <optional>
#include <blt/iterator/enumerate.h>

namespace blt::argparse
{
    namespace detail
    {
        inline hashset_t<std::string_view> allowed_flag_prefixes = {"-", "--", "+"};

        std::string flag_prefixes_as_string();

        inline std::string flag_prefix_list_string = flag_prefixes_as_string();

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
        class arg_type_t;

        void test();
    }

    class argument_string_t
    {
    public:
        explicit argument_string_t(const char* input): m_argument(input)
        {
            if (input == nullptr)
                throw detail::bad_flag("Argument cannot be null!");
        }

        [[nodiscard]] std::string_view get_flag() const
        {
            if (!flag_section)
                process_argument();
            return *flag_section;
        }

        [[nodiscard]] std::string_view get_name() const
        {
            if (!name_section)
                process_argument();
            return *name_section;
        }

        [[nodiscard]] std::string_view value() const
        {
            return get_name();
        }

        [[nodiscard]] bool is_flag() const
        {
            if (!m_is_flag)
                process_argument();
            return *m_is_flag;
        }

        [[nodiscard]] std::string_view get_argument() const
        {
            return m_argument;
        }

    private:
        void process_argument() const
        {
            size_t start = m_argument.size();
            for (auto [i, c] : enumerate(m_argument))
            {
                if (std::isalnum(c))
                {
                    start = i;
                    break;
                }
            }
            m_is_flag = (start != 0);
            flag_section = {m_argument.data(), start};
            name_section = {m_argument.data() + start, m_argument.size() - start};

            if (!flag_section->empty() && !detail::allowed_flag_prefixes.contains(*flag_section))
                throw detail::bad_flag(
                    "Invalid flag " + std::string(*flag_section) + " detected, flag is not in allowed list of flags! Must be one of " +
                    detail::flag_prefix_list_string);
        }

        std::string_view m_argument;
        mutable std::optional<bool> m_is_flag;
        mutable std::optional<std::string_view> flag_section;
        mutable std::optional<std::string_view> name_section;
    };

    class argument_consumer_t
    {
    public:
        argument_consumer_t(const i32 argc, const char** argv): argv(argv), argc(argc)
        {
        }

        [[nodiscard]] std::string_view peek(const i32 offset = 0) const
        {
            return argv[forward_index + offset];
        }

        std::string_view consume()
        {
            return argv[forward_index++];
        }

        [[nodiscard]] i32 position() const
        {
            return argc;
        }

        [[nodiscard]] i32 remaining() const
        {
            return argc - forward_index;
        }

        [[nodiscard]] bool has_next(const i32 offset = 0) const
        {
            return (offset + forward_index) < argc;
        }

    private:
        const char** argv;
        i32 argc;
        i32 forward_index = 0;
    };
}

#endif //BLT_PARSE_ARGPARSE_V2_H
