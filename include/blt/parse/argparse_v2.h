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
#include <type_traits>
#include <blt/iterator/enumerate.h>
#include <blt/std/ranges.h>
#include <blt/std/utility.h>

namespace blt::argparse
{
    namespace detail
    {
        inline hashset_t<std::string_view> allowed_flag_prefixes = {"-", "--", "+"};

        std::string flag_prefixes_as_string();
        hashset_t<char> prefix_characters();

        inline std::string flag_prefix_list_string = flag_prefixes_as_string();
        inline auto prefix_characters_set = prefix_characters();

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
                const std::string temp{value};

                if constexpr (std::is_same_v<T, i8> || std::is_same_v<T, i16> || std::is_same_v<T, i32>)
                {
                    return static_cast<T>(std::stoi(temp));
                }
                else if constexpr (std::is_same_v<T, i64>)
                {
                    return static_cast<i64>(std::stoll(temp));
                }
                else if constexpr (std::is_same_v<T, u8> || std::is_same_v<T, u16> || std::is_same_v<T, u32>)
                {
                    return static_cast<T>(std::stoul(temp));
                }
                else if constexpr (std::is_same_v<T, u64>)
                {
                    return static_cast<u64>(std::stoull(temp));
                }
                else if constexpr (std::is_same_v<T, float>)
                {
                    return std::stof(temp);
                }
                else if constexpr (std::is_same_v<T, double>)
                {
                    return std::stod(temp);
                } else
                {
                    static_assert(std::is_arithmetic_v<T>, "Unsupported type for this specialization");
                }
                BLT_UNREACHABLE;
            }
        };

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
                if (!detail::prefix_characters_set.contains(c))
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
        explicit argument_consumer_t(const span<argument_string_t>& args): args(args)
        {
        }

        [[nodiscard]] argument_string_t peek(const i32 offset = 0) const
        {
            return args[forward_index + offset];
        }

        argument_string_t consume()
        {
            return args[forward_index++];
        }

        [[nodiscard]] i32 position() const
        {
            return forward_index;
        }

        [[nodiscard]] i32 remaining() const
        {
            return static_cast<i32>(args.size()) - forward_index;
        }

        [[nodiscard]] bool has_next(const i32 offset = 0) const
        {
            return (offset + forward_index) < args.size();
        }

    private:
        span<argument_string_t> args;
        i32 forward_index = 0;
    };
}

#endif //BLT_PARSE_ARGPARSE_V2_H
