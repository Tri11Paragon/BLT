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

		std::array<bool, static_cast<u8>(log_tag_token_t::CONTENT)> tag_known_values{
			// year
			false,
			// month
			false,
			// day
			false,
			// hour
			false,
			// minute
			false,
			// second
			false,
			// ms
			false,
			// ns
			false,
			// unix
			false,
			// iso year
			false,
			// time
			false,
			// full_time
			false,
			// lc
			true,
			// ec
			true,
			// conditional error
			false,
			// reset
			true,
			// log level
			false,
			// thread_name
			false,
			// file
			false,
			// line
			false,
			// str
			false
		};
	}

	void logging_config_t::compile()
	{
		static hashmap_t<std::string_view, tags::detail::log_tag_token_t> tag_map = tags::detail::make_map();
		log_tag_content.clear();
		log_tag_tokens.clear();

		size_t i = 0;
		for (; i < log_format.size(); ++i)
		{
			size_t start = i;
			while (i < log_format.size() && log_format[i] != '{')
				++i;
			if (i == log_format.size() || (i < log_format.size() && (i - start) > 0))
			{
				log_tag_content.emplace_back(std::string_view(log_format.data() + start, i - start));
				log_tag_tokens.emplace_back(tags::detail::log_tag_token_t::CONTENT);
				if (i == log_format.size())
					break;
			}
			start = i;
			while (i < log_format.size() && log_format[i] != '}')
				++i;
			const auto tag = std::string_view(log_format.data() + start, i - start + 1);
			auto it = tag_map.find(tag);
			if (it == tag_map.end())
				throw std::runtime_error("Invalid log tag: " + std::string(tag));
			log_tag_tokens.emplace_back(it->second);
		}

		if (i < log_format.size())
		{
			log_tag_content.emplace_back(std::string_view(log_format.data() + i, log_format.size() - i));
			log_tag_tokens.emplace_back(tags::detail::log_tag_token_t::CONTENT);
		}
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
		return {"TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL",};
	}

	std::string logging_config_t::get_default_error_color()
	{
		return build(fg(ansi::color::color8_bright::RED));
	}
}
