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
#include <blt/logging/fmt_tokenizer.h>

namespace blt::logging
{
	fmt_token_type fmt_tokenizer_t::get_type(const char c)
	{
		switch (c)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				return fmt_token_type::NUMBER;
			case '+':
				return fmt_token_type::PLUS;
			case '-':
				return fmt_token_type::MINUS;
			case '.':
				return fmt_token_type::DOT;
			case ':':
				return fmt_token_type::COLON;
			case ' ':
				return fmt_token_type::SPACE;
			default:
				return fmt_token_type::STRING;
		}
	}

	std::optional<fmt_token_t> fmt_tokenizer_t::next()
	{
		if (pos >= m_fmt.size())
			return {};
		switch (const auto base_type = get_type(m_fmt[pos]))
		{
			case fmt_token_type::SPACE:
			case fmt_token_type::PLUS:
			case fmt_token_type::MINUS:
			case fmt_token_type::DOT:
			case fmt_token_type::COLON:
				return fmt_token_t{base_type, std::string_view{m_fmt.data() + pos++, 1}};
			default:
			{
				const auto begin = pos;
				for (; pos < m_fmt.size() && get_type(m_fmt[pos]) == base_type; ++pos)
				{}
				return fmt_token_t{base_type, std::string_view{m_fmt.data() + begin, pos - begin}};
			}
		}
	}

	std::vector<fmt_token_t> fmt_tokenizer_t::tokenize()
	{
		std::vector<fmt_token_t> tokens;
		while (auto token = next())
			tokens.push_back(*token);
		return tokens;
	}
}
