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

#ifndef BLT_LOGGING_LOGGING_H
#define BLT_LOGGING_LOGGING_H

#include <sstream>
#include <string>
#include <vector>
#include <blt/meta/meta.h>
#include <blt/logging/fmt_tokenizer.h>

namespace blt::logging
{
    struct logger_t
    {
        explicit logger_t() = default;

        template <typename... Args>
        std::string log(std::string fmt, Args&&... args)
        {
            compile(std::move(fmt));
            auto sequence = std::make_integer_sequence<size_t, sizeof...(Args)>{};
            while (auto pair = consume_until_fmt())
            {
                auto [begin, end] = *pair;
                if (end - begin > 0)
                {
                    auto format_data = handle_fmt(m_fmt.substr(begin + 1, begin - end - 1));
                    auto [arg_pos, fmt_type] = format_data;
                    if (arg_pos == -1)
                        arg_pos = static_cast<i64>(m_arg_pos++);
                    if (fmt_type)
                    {
                        if (fmt_type == fmt_type_t::GENERAL)
                        {
                            apply_func([this](auto&& value)
                            {
                                if (static_cast<u64>(value) > 0xFFFFFFFFFul)
                                    exponential();
                                else
                                    fixed();
                                m_stream << std::forward<decltype(value)>(value);
                            }, arg_pos, sequence, std::forward<Args>(args)...);
                        } else if (fmt_type == fmt_type_t::CHAR)
                        {

                        } else if (fmt_type == fmt_type_t::BINARY)
                        {

                        }
                    }
                    else
                    {
                        apply_func([this](auto&& value)
                        {
                            m_stream << std::forward<decltype(value)>(value);
                        }, arg_pos, sequence, std::forward<Args>(args)...);
                    }
                }
                else
                    apply_func([this](auto&& value)
                    {
                        m_stream << std::forward<decltype(value)>(value);
                    }, m_arg_pos++, sequence, std::forward<Args>(args)...);
            }
            finish();
            return to_string();
        }

        std::string to_string();

    private:
        template <typename Func, typename... Args, size_t... Indexes>
        void apply_func(const Func& func, const size_t arg, std::integer_sequence<size_t, Indexes...>, Args&&... args)
        {
            ((handle_func<Indexes>(func, arg, std::forward<Args>(args))), ...);
        }

        template <size_t index, typename Func, typename T>
        void handle_func(const Func& func, const size_t arg, T&& t)
        {
            if (index == arg)
                func(std::forward<T>(t));
        }

        [[nodiscard]] std::pair<i64, std::optional<fmt_type_t>> handle_fmt(std::string_view fmt);

        void exponential();
        void fixed();

        void compile(std::string fmt);

        std::optional<std::pair<size_t, size_t>> consume_until_fmt();

        void finish();

        std::string m_fmt;
        std::stringstream m_stream;
        fmt_parser_t m_parser;
        size_t m_last_fmt_pos = 0;
        size_t m_arg_pos = 0;
    };

    void print(const std::string& fmt);

    void newline();

    logger_t& get_global_logger();

    template <typename... Args>
    void print(std::string fmt, Args&&... args)
    {
        auto& logger = get_global_logger();
        print(logger.log(std::move(fmt), std::forward<Args>(args)...));
    }

    template <typename... Args>
    void println(std::string fmt, Args&&... args)
    {
        print(std::move(fmt), std::forward<Args>(args)...);
        newline();
    }
}

#endif // BLT_LOGGING_LOGGING_H
