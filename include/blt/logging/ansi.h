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

#ifndef BLT_LOGGING_COLORS_H
#define BLT_LOGGING_COLORS_H

#include <string>
#include <blt/std/types.h>

#define BLT_ANSI_ESCAPE "\x1B"
#define BLT_ANSI_CSI BLT_ANSI_ESCAPE "["
#define BLT_ANSI_DSC BLT_ANSI_ESCAPE "P"
#define BLT_ANSI_OSC BLT_ANSI_ESCAPE "]"

// https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797
namespace blt::logging::ansi
{
    namespace color
    {
        enum class color_mode : i32
        {
            RESET_ALL = 0,
            BOLD = 1,
            DIM = 2,
            ITALIC = 3,
            UNDERLINE = 4,
            BLINK = 5,
            REVERSE = 7,
            HIDDEN = 8,
            STRIKE_THROUGH = 9,
        };

        enum class color8 : i32
        {
            BLACK = 0,
            RED = 1,
            GREEN = 2,
            YELLOW = 3,
            BLUE = 4,
            MAGENTA = 5,
            CYAN = 6,
            WHITE = 7,
            DEFAULT = 9
        };

        enum class color8_bright : i32
        {
            BLACK = 0,
            RED = 1,
            GREEN = 2,
            YELLOW = 3,
            BLUE = 4,
            MAGENTA = 5,
            CYAN = 6,
            WHITE = 7,
        };

        namespace detail
        {
            template <typename T>
            struct color_converter
            {
            };

            template <>
            struct color_converter<color8>
            {
                static std::string to_string(color8 color, const bool background)
                {
                    return (background ? "4" : "3") + std::to_string(static_cast<i32>(color));
                }
            };

            template <>
            struct color_converter<color8_bright>
            {
                static std::string to_string(color8_bright color, const bool background)
                {
                    return (background ? "10" : "9") + std::to_string(static_cast<i32>(color));
                }
            };
        }
    }

    namespace general
    {
        inline const std::string bell = "\x07";
        inline const std::string bs = "\x08";
        inline const std::string horizontal_tab = "\x09";
        inline const std::string linefeed = "\x0A";
        inline const std::string vertical_tab = "\x0B";
        inline const std::string form_feed = "\x0C";
        inline const std::string carriage_return = "\x0D";
        inline const std::string escape = BLT_ANSI_ESCAPE;
        inline const std::string del = "\0x7F";
        inline const std::string csi = BLT_ANSI_CSI;
        inline const std::string dsc = BLT_ANSI_DSC;
        inline const std::string osc = BLT_ANSI_OSC;
    }

    namespace cursor
    {
        inline const std::string home = BLT_ANSI_CSI "H";

        template <bool UseH>
        inline std::string move_to(i64 line, i64 column)
        {
            if constexpr (UseH)
                return std::string(BLT_ANSI_CSI) + std::to_string(line) + ";" + std::to_string(column) + "H";
            else
                return std::string(BLT_ANSI_CSI) + std::to_string(line) + ";" + std::to_string(column) + "f";
        }

        inline std::string move_up(i64 lines)
        {
            return std::string(BLT_ANSI_CSI) + std::to_string(lines) + "A";
        }

        inline std::string move_down(i64 lines)
        {
            return std::string(BLT_ANSI_CSI) + std::to_string(lines) + "B";
        }

        inline std::string move_right(i64 columns)
        {
            return std::string(BLT_ANSI_CSI) + std::to_string(columns) + "C";
        }

        inline std::string move_left(i64 columns)
        {
            return std::string(BLT_ANSI_CSI) + std::to_string(columns) + "D";
        }

        inline std::string move_begin_down(i64 lines)
        {
            return std::string(BLT_ANSI_CSI) + std::to_string(lines) + "E";
        }

        inline std::string move_begin_up(i64 lines)
        {
            return std::string(BLT_ANSI_CSI) + std::to_string(lines) + "F";
        }

        inline std::string move_to(i64 column)
        {
            return std::string(BLT_ANSI_CSI) + std::to_string(column) + "G";
        }

        inline const std::string request_cursor_position = BLT_ANSI_CSI "6n";
        inline const std::string move_up_one_line = BLT_ANSI_ESCAPE " M";
        inline const std::string save_cursor_position_dec = BLT_ANSI_ESCAPE " 7";
        inline const std::string resotre_cursor_position_dec = BLT_ANSI_ESCAPE " 8";
        inline const std::string save_cursor_position_sco = BLT_ANSI_CSI "s";
        inline const std::string resotre_cursor_position_sco = BLT_ANSI_CSI "u";
    }

    namespace erase
    {
        inline const std::string to_end_of_screen = BLT_ANSI_CSI "0J";
        inline const std::string from_begin_of_screen = BLT_ANSI_CSI "1J";
        inline const std::string entire_screen = BLT_ANSI_CSI "2J";
        inline const std::string saved_lines = BLT_ANSI_CSI "3J";
        inline const std::string to_end_of_line = BLT_ANSI_CSI "0K";
        inline const std::string from_begin_of_line = BLT_ANSI_CSI "1K";
        inline const std::string entire_line = BLT_ANSI_CSI "2K";
    }
}

#endif //BLT_LOGGING_COLORS_H
