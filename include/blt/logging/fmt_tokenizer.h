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

#ifndef BLT_LOGGING_FMT_TOKENIZER_H
#define BLT_LOGGING_FMT_TOKENIZER_H

#include <optional>
#include <string_view>
#include <vector>
#include <blt/std/types.h>

namespace blt::logging
{
	enum class fmt_token_type : u8
	{
		STRING,
		NUMBER,
		SPACE,
		COLON,
		DOT,
		MINUS,
		PLUS
	};

	struct fmt_token_t
	{
		fmt_token_type type;
		std::string_view value;
	};

	class fmt_tokenizer_t
	{
	public:
		explicit fmt_tokenizer_t(const std::string_view fmt): m_fmt(fmt)
		{}

		static fmt_token_type get_type(char c);

		std::optional<fmt_token_t> next();

		std::vector<fmt_token_t> tokenize();

	private:
		size_t pos = 0;
		std::string_view m_fmt;
	};
}

#endif //BLT_LOGGING_FMT_TOKENIZER_H
