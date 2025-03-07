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
#include <blt/logging/logging.h>
#include <blt/std/types.h>
#include <blt/fs/fwddecl.h>

namespace blt::logging
{
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
		// wrappers for streams exist in blt/fs/stream_wrappers.h
		std::vector<fs::writer_t*> log_outputs = get_default_log_outputs();
		std::string log_format = get_default_log_format();
		std::array<std::string, LOG_LEVEL_COUNT> log_level_colors = get_default_log_level_colors();
		std::array<std::string, LOG_LEVEL_COUNT> log_level_names = get_default_log_level_names();
		log_level_t level = log_level_t::TRACE;
		bool use_color = true;
		// if true prints the whole path to the file (eg /home/user/.../.../project/src/source.cpp:line#)
		bool print_full_name = false;
		// this will attempt to use the maximum possible size for each printed element, then align to that.
		// This creates output where the user message always starts at the same column.
		bool ensure_alignment = true;
	private:
		static std::string get_default_log_format();
		static std::vector<fs::writer_t*> get_default_log_outputs();
		static std::array<std::string, LOG_LEVEL_COUNT> get_default_log_level_colors();
		static std::array<std::string, LOG_LEVEL_COUNT> get_default_log_level_names();
	};
}

#endif //BLT_LOGGING_LOGGING_CONFIG_H
