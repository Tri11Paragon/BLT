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
#include <cstdio>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <blt/logging/ansi.h>
#include <blt/logging/fmt_tokenizer.h>
#include <blt/logging/status.h>
#include <blt/math/vectors.h>

namespace blt::logging
{
	vec2i get_cursor_position()
	{
		termios save{}, raw{};

		tcgetattr(0, &save);
		cfmakeraw(&raw);
		tcsetattr(0,TCSANOW, &raw);

		char buf[32];
		char cmd[] = "\033[6n";

		int row = 0;
		int col = 0;

		if (isatty(fileno(stdin)))
		{
			write(1, cmd, sizeof(cmd));
			read(0, buf, sizeof(buf));

			int sep = 0;
			int end = 0;
			for (int i = 2; i < 8; i++)
			{
				if (buf[i] == ';')
					sep = i;
				if (buf[i] == 'R')
				{
					end = i;
					break;
				}
			}
			row = std::stoi(std::string(buf + 2, buf + sep));
			col = std::stoi(std::string(buf + sep + 1, buf + end));
			// printf("{Row: %d, Col: %d}", row, col);
		}

		tcsetattr(0,TCSANOW, &save);

		return vec2i{row, col};
	}

	vec2i get_screen_size()
	{
		std::cout << ansi::cursor::move_to(9999, 9999);
		const auto pos = get_cursor_position();
		std::cout << ansi::cursor::lower_left_corner;
		std::cout << " ";
		return pos;
	}

	status_bar_t::status_bar_t(const i32 status_size): m_status_size(status_size)
	{
		std::cout << ansi::cursor::hide_cursor;
	}

	std::string status_item_t::print()
	{}

	injector_output_t status_bar_t::inject(const std::string& input)
	{
		injector_output_t output{input, false, false};

		std::cout << ansi::cursor::lower_left_corner;
		std::cout << ansi::scroll::scroll_up(1);
		for (int i = 0; i < m_status_size; i++)
			std::cout << ansi::erase::entire_line << ansi::cursor::move_begin_up(1);
		std::cout << output.new_logging_output;
		std::cout << ansi::erase::entire_line;
		std::cout << "[----status----]";
		std::cout << ansi::cursor::move_begin_down(1) << ansi::erase::entire_line;
		std::cout << "[----Second Line----]";
		std::cout << std::flush;

		return output;
	}

	status_bar_t::~status_bar_t()
	{
		// std::cout << "\033[" << m_scrolling_region + 1 << ";1H";
		std::cout << ansi::cursor::lower_left_corner;
		std::cout << ansi::cursor::show_cursor;
	}
}
