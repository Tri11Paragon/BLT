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
#include <iomanip>
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

	std::pair<i64, std::optional<fmt_type_t>> logger_t::handle_fmt(const std::string_view fmt)
	{
		const auto spec = m_parser.parse(fmt);
		if (spec.leading_zeros)
			m_stream << std::setfill('0');
		else
			m_stream << std::setfill(' ');
		if (spec.width > 0)
			m_stream << std::setw(static_cast<i32>(spec.width));
		else
			m_stream << std::setw(0);
		if (spec.precision > 0)
			m_stream << std::setprecision(static_cast<i32>(spec.precision));
		else
			m_stream << std::setprecision(2);
		if (spec.uppercase)
			m_stream << std::uppercase;
		else
			m_stream << std::nouppercase;
		std::optional<fmt_type_t> type;
		switch (spec.type)
		{
		case fmt_type_t::BINARY:
		case fmt_type_t::CHAR:
		case fmt_type_t::GENERAL:
			type = spec.type;
			break;
		case fmt_type_t::DECIMAL:
			m_stream << std::dec;
			break;
		case fmt_type_t::OCTAL:
			m_stream << std::oct;
			break;
		case fmt_type_t::HEX:
			m_stream << std::hex;
			break;
		case fmt_type_t::HEX_FLOAT:
			m_stream << std::hexfloat;
			break;
		case fmt_type_t::EXPONENT:
			m_stream << std::scientific;
			break;
		case fmt_type_t::FIXED_POINT:
			m_stream << std::fixed;
			break;
		}
		return {spec.arg_id, type};
	}

	void logger_t::exponential()
	{
		m_stream << std::scientific;
	}

	void logger_t::fixed()
	{
		m_stream << std::fixed;
	}

	void logger_t::compile(std::string fmt)
	{
		m_fmt = std::move(fmt);
		m_last_fmt_pos = 0;
		m_arg_pos = 0;
		m_stream.str("");
		m_stream.clear();
	}

	std::optional<std::pair<size_t, size_t>> logger_t::consume_until_fmt()
	{
		const auto begin = m_fmt.find('{', m_last_fmt_pos);
		if (begin == std::string::npos)
			return {};
		const auto end = m_fmt.find('}', begin);
		if (end == std::string::npos)
		{
			std::cerr << "Invalid format string, missing closing '}' near " << m_fmt.substr(std::min(static_cast<i64>(begin) - 5, 0l)) << std::endl;
			std::exit(EXIT_FAILURE);
		}
		m_stream << std::string_view(m_fmt.data() + static_cast<ptrdiff_t>(m_last_fmt_pos), begin - m_last_fmt_pos);\
		m_last_fmt_pos = end + 1;
		return std::pair{begin, end};
	}

	void logger_t::finish()
	{
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
