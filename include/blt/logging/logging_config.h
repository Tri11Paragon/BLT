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

#ifndef BLT_LOGGING_LOGGING_CONFIG_H
#define BLT_LOGGING_LOGGING_CONFIG_H

#include <optional>
#include <string>
#include <blt/std/types.h>
#include <blt/logging/ansi.h>
#include <array>

namespace blt::logging
{
    enum class log_level_t
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    inline constexpr size_t LOG_LEVEL_COUNT = 6;

    class logging_config_t
    {
    public:
        std::optional<std::string> log_file_path;
        std::string log_format = "";
        std::array<std::string, LOG_LEVEL_COUNT> log_level_colors = {

        };
        log_level_t level = log_level_t::TRACE;
        bool use_color = true;
        bool log_to_console = true;

    private:
    };
}

#endif //BLT_LOGGING_LOGGING_CONFIG_H
