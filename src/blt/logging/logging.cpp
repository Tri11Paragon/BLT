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

	void logger_t::compile(std::string fmt)
	{
		m_fmt = std::move(fmt);
		m_last_fmt_pos = 0;
	}

	void logger_t::consume_until_fmt()
	{
		const auto begin = m_fmt.find('{', m_last_fmt_pos);
		const auto end = m_fmt.find('}', begin);
		m_stream << std::string_view(m_fmt.data() + static_cast<ptrdiff_t>(m_last_fmt_pos), begin - m_last_fmt_pos);
		m_last_fmt_pos = end;
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
