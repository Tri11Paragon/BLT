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
#include <iostream>
#include <blt/fs/stream_wrappers.h>
#include <blt/logging/ansi.h>
#include <blt/logging/logging_config.h>
#include <blt/std/hashmap.h>

namespace blt::logging
{
	namespace tags::detail
	{
		hashmap_t<std::string_view, log_tag_token_t> make_map()
		{
			hashmap_t<std::string_view, log_tag_token_t> map{};
			map[YEAR] = log_tag_token_t::YEAR;
			map[MONTH] = log_tag_token_t::MONTH;
			map[DAY] = log_tag_token_t::DAY;
			map[HOUR] = log_tag_token_t::HOUR;
			map[MINUTE] = log_tag_token_t::MINUTE;
			map[SECOND] = log_tag_token_t::SECOND;
			map[MILLISECOND] = log_tag_token_t::MS;
			map[NANOSECOND] = log_tag_token_t::NS;
			map[UNIX_TIME] = log_tag_token_t::UNIX;
			map[ISO_YEAR] = log_tag_token_t::ISO_YEAR;
			map[TIME] = log_tag_token_t::TIME;
			map[FULL_TIME] = log_tag_token_t::FULL_TIME;
			map[LOG_COLOR] = log_tag_token_t::LC;
			map[ERROR_COLOR] = log_tag_token_t::EC;
			map[CONDITIONAL_ERROR_COLOR] = log_tag_token_t::CEC;
			map[RESET] = log_tag_token_t::RESET;
			map[LOG_LEVEL] = log_tag_token_t::LL;
			map[THREAD_NAME] = log_tag_token_t::TN;
			map[FILE] = log_tag_token_t::FILE;
			map[LINE] = log_tag_token_t::LINE;
			map[STR] = log_tag_token_t::STR;
			return map;
		}

		hashmap_t<std::string_view, log_tag_token_t> tag_map = make_map();
	}

	std::string logging_config_t::get_default_log_format()
	{
		return build(fg(ansi::color::color8_bright::CYAN)) + "[" + tags::FULL_TIME + "]" + tags::RESET + " " + tags::LOG_COLOR + "[" + tags::LOG_LEVEL
		+ "]" + tags::RESET + " " + build(fg(ansi::color::color8::MAGENTA)) + "(" + tags::FILE + ":" + tags::LINE + ")" + tags::RESET + " " +
		tags::CONDITIONAL_ERROR_COLOR + tags::STR + tags::RESET + "\n";
	}

	std::vector<fs::writer_t*> logging_config_t::get_default_log_outputs()
	{
		static fs::fstream_writer_t cout_writer{std::cout};
		std::vector<fs::writer_t*> outputs{};
		outputs.push_back(&cout_writer);
		return outputs;
	}

	std::array<std::string, LOG_LEVEL_COUNT> logging_config_t::get_default_log_level_colors()
	{
		return {
			// TRACE
			build(fg(ansi::color::color8_bright::WHITE)),
			// DEBUG
			build(fg(ansi::color::color8::CYAN)),
			// INFO
			build(fg(ansi::color::color8_bright::GREEN)),
			// WARN
			build(fg(ansi::color::color8_bright::YELLOW)),
			// ERROR
			build(fg(ansi::color::color8_bright::RED)),
			// FATAL
			build(fg(ansi::color::color8_bright::WHITE), bg(ansi::color::color8_bright::RED)),
		};
	}

	std::array<std::string, LOG_LEVEL_COUNT> logging_config_t::get_default_log_level_names()
	{
		return {
			"TRACE",
			"DEBUG",
			"INFO",
			"WARN",
			"ERROR",
			"FATAL",
		};
	}

	std::string logging_config_t::get_default_error_color()
	{
		return build(fg(ansi::color::color8_bright::RED));
	}
}
