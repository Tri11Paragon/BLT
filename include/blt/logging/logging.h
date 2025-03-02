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

#include <iostream>
#include <ostream>
#include <string>
#include <blt/std/utility.h>

namespace blt::logging
{
    struct logger_t
    {
        explicit logger_t(std::string fmt): fmt(std::move(fmt))
        {
        }

        template <typename T>
        std::string make_string(T&& t)
        {
            if constexpr (std::is_same_v<T, std::string> || std::is_convertible_v<T, std::string>)
                return std::forward<T>(t);
            else if constexpr (std::is_same_v<T, std::string_view> || std::is_same_v<std::remove_const_t<T>, char*> || std::is_convertible_v<
                T, std::string_view>)
                return std::string(std::forward<T>(t));
            else if constexpr (std::is_same_v<T, char>)
                return std::string() + std::forward<T>(t);
            else if constexpr (std::is_arithmetic_v<T>)
                return std::to_string(std::forward<T>(t));
            else
            {
                BLT_UNREACHABLE;
            }
        }

        void compile();

        void insert_next_value(const std::string& arg);

        template <typename... Args>
        const std::string& log(Args&&... args)
        {
            (insert_next_value(make_string(std::forward<Args>(args))), ...);
            return fmt;
        }

    private:
        std::string fmt;
    };

    void print(const std::string& fmt);

    void newline();

    template<typename... Args>
    void print(std::string fmt, Args&&... args)
    {
        logger_t logger{std::move(fmt)};
        logger.compile();
        print(logger.log(std::forward<Args>(args)...));
    }

    template<typename... Args>
    void println(std::string fmt, Args&&... args)
    {
        print(std::move(fmt), std::forward<Args>(args)...);
        newline();
    }
}

#endif // BLT_LOGGING_LOGGING_H
