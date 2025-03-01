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
#include <blt/logging/logging.h>
#include <blt/std/types.h>

namespace blt::logging
{
    void logger_t::compile()
    {

    }

    void logger_t::insert_next_value(const std::string& arg)
    {
        const auto begin = fmt.find('{');
        const auto end = fmt.find('}', begin);
        fmt.erase(fmt.begin() + static_cast<i64>(begin), fmt.begin() + static_cast<i64>(end) + 1);
        fmt.insert(begin, arg);
    }

    void print(const std::string& fmt)
    {
        std::cout << fmt;
    }

    void newline()
    {
        std::cout << std::endl;
    }
}
