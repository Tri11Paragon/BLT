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

#include <array>
#include <string>
#include <vector>
#include <blt/fs/fwddecl.h>
#include <blt/logging/fwddecl.h>
#include <blt/std/types.h>

namespace blt::logging
{
	namespace tags
	{
		// Current Year
		inline constexpr auto YEAR = "{YEAR}";
		// Current Month
		inline constexpr auto MONTH = "{MONTH}";
		// Current Day
		inline constexpr auto DAY = "{DAY}";
		// Current Hour
		inline constexpr auto HOUR = "{HOUR}";
		// Current Minute
		inline constexpr auto MINUTE = "{MINUTE}";
		// Current Second
		inline constexpr auto SECOND = "{SECOND}";
		// Current Millisecond
		inline constexpr auto MILLISECOND = "{MS}";
		// Current Nanosecond time, This is direct output of nanotime
		inline constexpr auto NANOSECOND = "{NS}";
		// Current Unix time in milliseconds
		inline constexpr auto UNIX_TIME = "{UNIX}";
		// Formatted ISO year-month-day in a single variable
		inline constexpr auto ISO_YEAR = "{ISO_YEAR}";
		// Formatted hour:minute:second in a single variable
		inline constexpr auto TIME = "{TIME}";
		// Formatted year-month-day hour:minute:second in a single variable
		inline constexpr auto FULL_TIME = "{FULL_TIME}";
		// Color of the current log level, empty string if use_color = false
		inline constexpr auto LOG_COLOR = "{LC}";
		// Color of the error color, empty string if use_color = false
		inline constexpr auto ERROR_COLOR = "{EC}";
		// Empty is use_color = false or if log level is not an error. Otherwise, {EC}
		inline constexpr auto CONDITIONAL_ERROR_COLOR = "{CEC}";
		// Resets all ANSI sequences
		inline constexpr auto RESET = "{RESET}";
		// Current log level
		inline constexpr auto LOG_LEVEL = "{LL}";
		// Current thread name. Requires you to manually set the thread name using blt::logging::set_thread_name() from that thread.
		inline constexpr auto THREAD_NAME = "{TN}";
		// Current file from where the log call was invoked.
		inline constexpr auto FILE = "{FILE}";
		// Current line from where the log call was invoked
		inline constexpr auto LINE = "{LINE}";
		// User string input, formatted with provided args
		inline constexpr auto STR = "{STR}";

		namespace detail
		{
			enum class log_tag_token_t : u8
			{
				YEAR,
				MONTH,
				DAY,
				HOUR,
				MINUTE,
				SECOND,
				MS,
				NS,
				UNIX,
				ISO_YEAR,
				TIME,
				FULL_TIME,
				LC,
				EC,
				CEC,
				RESET,
				LL,
				TN,
				FILE,
				LINE,
				STR,
				// token used to describe that a non-format token should be consumed. aka a normal string from the file.
				CONTENT
			};
		}
	}

	enum class log_level_t : u8
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
		friend logger_t;

	public:
		logging_config_t()
		{
			compile();
		}

		void compile();

		logging_config_t& add_log_output(fs::writer_t* writer)
		{
			log_outputs.push_back(writer);
			return *this;
		}

		logging_config_t& set_log_format(std::string format)
		{
			log_format = std::move(format);
			compile();
			return *this;
		}

		logging_config_t& set_error_color(std::string color)
		{
			error_color = std::move(color);
			compile();
			return *this;
		}

		logging_config_t& set_log_level_colors(std::array<std::string, LOG_LEVEL_COUNT> colors)
		{
			log_level_colors = std::move(colors);
			compile();
			return *this;
		}

		logging_config_t& set_log_level_names(std::array<std::string, LOG_LEVEL_COUNT> names)
		{
			log_level_names = std::move(names);
			return *this;
		}

		logging_config_t& set_level(const log_level_t level)
		{
			this->level = level;
			return *this;
		}

		logging_config_t& set_use_color(const bool use_color)
		{
			this->use_color = use_color;
			compile();
			return *this;
		}

		logging_config_t& set_print_full_name(const bool print_full_name)
		{
			this->print_full_name = print_full_name;
			return *this;
		}

		logging_config_t& set_ensure_alignment(const bool ensure_alignment)
		{
			this->ensure_alignment = ensure_alignment;
			return *this;
		}

		[[nodiscard]] std::pair<const std::vector<tags::detail::log_tag_token_t>&, const std::vector<std::string>&> get_log_tag_tokens() const
		{
			return {log_tag_tokens, log_tag_content};
		}

	private:
		std::vector<std::string> log_tag_content;
		std::vector<tags::detail::log_tag_token_t> log_tag_tokens;
		// wrappers for streams exist in blt/fs/stream_wrappers.h
		std::vector<fs::writer_t*> log_outputs = get_default_log_outputs();
		std::string log_format = get_default_log_format();
		std::string error_color = get_default_error_color();
		std::array<std::string, LOG_LEVEL_COUNT> log_level_colors = get_default_log_level_colors();
		std::array<std::string, LOG_LEVEL_COUNT> log_level_names = get_default_log_level_names();
		log_level_t level = log_level_t::TRACE;
		bool use_color = true;
		// if true prints the whole path to the file (eg /home/user/.../.../project/src/source.cpp:line#)
		bool print_full_name = false;
		// this will attempt to use the maximum possible size for each printed element, then align to that.
		// This creates output where the user message always starts at the same column.
		bool ensure_alignment = true;

		static std::string get_default_log_format();
		static std::vector<fs::writer_t*> get_default_log_outputs();
		static std::array<std::string, LOG_LEVEL_COUNT> get_default_log_level_colors();
		static std::array<std::string, LOG_LEVEL_COUNT> get_default_log_level_names();
		static std::string get_default_error_color();
	};
}

#endif //BLT_LOGGING_LOGGING_CONFIG_H
