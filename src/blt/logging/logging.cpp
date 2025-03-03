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
#include <blt/iterator/enumerate.h>
#include <blt/logging/logging.h>
#include <blt/std/types.h>

namespace blt::logging
{
	struct logging_thread_context_t
	{
		logger_t logger;
	};

	std::string logger_t::to_string()
	{
		auto str = m_stream.str();
		m_stream.str("");
		m_stream.clear();
		return str;
	}

	void logger_t::handle_fmt(const std::string_view fmt)
	{
		std::cout << fmt << std::endl;
	}

	const std::string& logger_t::get(const size_t index)
	{
		if (index >= m_args_to_str.size())
		{
			std::cerr << "Insufficient number of arguments provided to format string '" << m_fmt << "' got ";
			for (const auto& [i, arg] : enumerate(std::as_const(m_args_to_str)))
			{
				std::cerr << '\'' << arg << "'";
				if (i != m_args_to_str.size() - 1)
					std::cerr << " ";
			}
			std::exit(EXIT_FAILURE);
		}
		return m_args_to_str[index];
	}

	void logger_t::compile(std::string fmt)
	{
		m_fmt = std::move(fmt);
		m_last_fmt_pos = 0;
		m_arg_pos = 0;
	}

	bool logger_t::consume_until_fmt()
	{
		const auto begin = m_fmt.find('{', m_last_fmt_pos);
		if (begin == std::string::npos)
			return false;
		const auto end = m_fmt.find('}', begin);
		if (end == std::string::npos)
		{
			std::cerr << "Invalid format string, missing closing '}' near " << m_fmt.substr(std::min(static_cast<i64>(begin) - 5, 0l)) << std::endl;
			std::exit(EXIT_FAILURE);
		}
		m_stream << std::string_view(m_fmt.data() + static_cast<ptrdiff_t>(m_last_fmt_pos), begin - m_last_fmt_pos);\
		if (end - begin > 1)
			handle_fmt(std::string_view(m_fmt.data() + static_cast<ptrdiff_t>(begin + 1), end - begin - 1));
		else
		{
			// no arguments, must consume from args
			m_stream << get(m_arg_pos++);
		}
		m_last_fmt_pos = end + 1;
		return true;
	}

	void logger_t::finish()
	{
		m_stream.str("");
		m_stream.clear();

		while (consume_until_fmt())
		{}

		m_stream << std::string_view(m_fmt.data() + static_cast<ptrdiff_t>(m_last_fmt_pos), m_fmt.size() - m_last_fmt_pos);
		m_last_fmt_pos = m_fmt.size();
	}

	logger_t& get_global_logger()
	{
		thread_local logging_thread_context_t context;
		return context.logger;
	}

	void print(const std::string& fmt)
	{
		std::cout << fmt;
	}

	void newline()
	{
		std::cout << std::endl;
	}
}
