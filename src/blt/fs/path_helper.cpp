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
#include <blt/fs/path_helper.h>
#include <blt/std/string.h>
#include <blt/compatibility.h>

namespace blt::fs
{
#ifdef BLT_WINDOWS
    constexpr static char delim = '\\';
#else
    constexpr static char delim = '/';
#endif

    std::string base_name(const std::string& str)
    {
        return std::string(base_name_sv(str));
    }

    std::string_view base_name_sv(const std::string_view str)
    {
        const auto parts = string::split_sv(str, delim);
        return parts.back();
    }
}
