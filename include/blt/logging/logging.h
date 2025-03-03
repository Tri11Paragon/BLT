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
#include <sstream>
#include <string>
#include <blt/std/utility.h>
#include <blt/meta/meta.h>

namespace blt::logging
{
	struct logger_t
	{
		explicit logger_t() = default;

		template <typename T>
		void print_value(T&& t)
		{
			static_assert(meta::is_streamable_v<T>, "T must be streamable in order to work with blt::logging!");
			m_stream << std::forward<T>(t);
		}

		template <typename... Args>
		std::string log(std::string fmt, Args&&... args)
		{
			compile(std::move(fmt));
			((consume_until_fmt(), print_value(std::forward<Args>(args))), ...);
			return to_string();
		}

		std::string to_string();

	private:
		void compile(std::string fmt);

		void consume_until_fmt();

		std::string m_fmt;
		std::stringstream m_stream;
		size_t m_last_fmt_pos = 0;
	};

	void print(const std::string& fmt);

	void newline();

	logger_t& get_global_logger();

	template <typename... Args>
	void print(std::string fmt, Args&&... args)
	{
		auto& logger = get_global_logger();
		print(logger.log(std::move(fmt), std::forward<Args>(args)...));
	}

	template <typename... Args>
	void println(std::string fmt, Args&&... args)
	{
		print(std::move(fmt), std::forward<Args>(args)...);
		newline();
	}
}

#endif // BLT_LOGGING_LOGGING_H
