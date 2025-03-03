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

#include <sstream>
#include <string>
#include <vector>
#include <blt/meta/meta.h>

namespace blt::logging
{
	struct logger_t
	{
		explicit logger_t() = default;

		template <typename T>
		std::string print_value(T&& t)
		{
			static_assert(meta::is_streamable_v<T>, "T must be streamable in order to work with blt::logging!");
			m_stream.str("");
			m_stream.clear();
			m_stream << std::forward<T>(t);
			return m_stream.str();
		}

		template <typename... Args>
		std::string log(std::string fmt, Args&&... args)
		{
			compile(std::move(fmt));
			m_args_to_str.clear();
			m_args_to_str.resize(sizeof...(Args));
			insert(std::make_integer_sequence<size_t, sizeof...(Args)>{}, std::forward<Args>(args)...);
			finish();
			return to_string();
		}

		std::string to_string();

	private:
		template<typename... Args, size_t... Indexes>
		void insert(std::integer_sequence<size_t, Indexes...>, Args&&... args)
		{
			((handle_insert<Indexes>(std::forward<Args>(args))), ...);
		}

		template<size_t index, typename T>
		void handle_insert(T&& t)
		{
			m_args_to_str[index] = print_value(std::forward<T>(t));
		}

		void handle_fmt(std::string_view fmt);

		const std::string& get(size_t index);

		void compile(std::string fmt);

		bool consume_until_fmt();

		void finish();

		std::string m_fmt;
		std::stringstream m_stream;
		std::vector<std::string> m_args_to_str;
		size_t m_last_fmt_pos = 0;
		size_t m_arg_pos = 0;
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
