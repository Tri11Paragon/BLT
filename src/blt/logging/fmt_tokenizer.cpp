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
#include <sstream>
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
        if (m_pos >= m_fmt.size())
            return {};
        switch (const auto base_type = get_type(m_fmt[m_pos]))
        {
        case fmt_token_type::SPACE:
        case fmt_token_type::PLUS:
        case fmt_token_type::MINUS:
        case fmt_token_type::DOT:
        case fmt_token_type::COLON:
            return fmt_token_t{base_type, std::string_view{m_fmt.data() + m_pos++, 1}};
        default:
            {
                const auto begin = m_pos;
                for (; m_pos < m_fmt.size() && get_type(m_fmt[m_pos]) == base_type; ++m_pos)
                {
                }
                return fmt_token_t{base_type, std::string_view{m_fmt.data() + begin, m_pos - begin}};
            }
        }
    }

    std::vector<fmt_token_t> fmt_tokenizer_t::tokenize(const std::string_view fmt)
    {
        m_fmt = fmt;
        m_pos = 0;
        std::vector<fmt_token_t> tokens;
        while (auto token = next())
            tokens.push_back(*token);
        return tokens;
    }

    const fmt_spec_t& fmt_parser_t::parse(const std::string_view fmt)
    {
        m_spec = {};
        m_pos = 0;
        m_tokens = m_tokenizer.tokenize(fmt);

        parse_fmt_field();

        return m_spec;
    }

    void fmt_parser_t::parse_fmt_field()
    {
        if (!has_next())
            throw std::runtime_error("Expected token when parsing format field");
        const auto [type, value] = peek();
        if (type == fmt_token_type::COLON)
        {
            consume();
            parse_fmt_spec_stage_1();
        }
        else if (type == fmt_token_type::NUMBER)
        {
            parse_arg_id();
            if (has_next())
            {
                if (peek().type == fmt_token_type::COLON)
                {
                    consume();
                    parse_fmt_spec_stage_1();
                }
                else
                    throw std::runtime_error("Expected ':' when parsing format field after arg id!");
            }
        }
        else
        {
            std::stringstream ss;
            ss << "Expected unknown token '" << static_cast<u8>(type) << "' value '" << value << "' when parsing format field";
            throw std::runtime_error(ss.str());
        }
        if (has_next())
            parse_type();
    }

    void fmt_parser_t::parse_arg_id()
    {
        if (!has_next())
            throw std::runtime_error("Missing token when parsing arg id");
        const auto [type, value] = next();
        if (type != fmt_token_type::NUMBER)
        {
            std::stringstream ss;
            ss << "Expected number when parsing arg id, unexpected value '" << value << '\'';
            throw std::runtime_error(ss.str());
        }
        m_spec.arg_id = std::stoll(std::string(value));
    }

    // handle start of fmt, with sign
    void fmt_parser_t::parse_fmt_spec_stage_1()
    {
        if (!has_next())
            return;
        auto [type, value] = peek();
        switch (type)
        {
        case fmt_token_type::STRING:
        case fmt_token_type::COLON:
            {
                std::stringstream ss;
                ss << "(Stage 1) Invalid token type " << static_cast<u8>(type) << " value " << value;
                throw std::runtime_error(ss.str());
            }
        case fmt_token_type::NUMBER:
            parse_width();
            parse_fmt_spec_stage_3();
            break;
        case fmt_token_type::DOT:
            consume();
            parse_precision();
            break;
        case fmt_token_type::SPACE:
        case fmt_token_type::MINUS:
        case fmt_token_type::PLUS:
            parse_sign();
            parse_fmt_spec_stage_2();
            break;
        }
    }

    // handle width parsing
    void fmt_parser_t::parse_fmt_spec_stage_2()
    {
        if (!has_next())
            return;
        auto [type, value] = peek();
        switch (type)
        {
        case fmt_token_type::STRING:
        case fmt_token_type::COLON:
        case fmt_token_type::MINUS:
        case fmt_token_type::PLUS:
        case fmt_token_type::SPACE:
            {
                std::stringstream ss;
                ss << "(Stage 2) Invalid token type " << static_cast<u8>(type) << " value " << value;
                throw std::runtime_error(ss.str());
            }
        case fmt_token_type::NUMBER:
            parse_width();
            parse_fmt_spec_stage_3();
            break;
        case fmt_token_type::DOT:
            consume();
            parse_precision();
            break;
        }
    }

    void fmt_parser_t::parse_fmt_spec_stage_3()
    {
        if (!has_next())
            return;
        auto [type, value] = peek();
        switch (type)
        {
        case fmt_token_type::STRING:
        case fmt_token_type::COLON:
        case fmt_token_type::MINUS:
        case fmt_token_type::PLUS:
        case fmt_token_type::SPACE:
        case fmt_token_type::NUMBER:
            {
                std::stringstream ss;
                ss << "(Stage 3) Invalid token type " << static_cast<u8>(type) << " value " << value;
                throw std::runtime_error(ss.str());
            }
        case fmt_token_type::DOT:
            consume();
            parse_precision();
            break;
        }
    }

    void fmt_parser_t::parse_sign()
    {
        auto [_, value] = next();
        if (value.size() > 1)
        {
            std::stringstream ss;
            ss << "Sign contains more than one character, we are not sure how to interpret this. Value '" << value << "'";
            throw std::runtime_error(ss.str());
        }
        switch (value[0])
        {
        case '+':
            m_spec.sign = fmt_sign_t::PLUS;
            break;
        case '-':
            m_spec.sign = fmt_sign_t::MINUS;
            break;
        case ' ':
            m_spec.sign = fmt_sign_t::SPACE;
            break;
        default:
            {
                std::stringstream ss;
                ss << "Invalid sign " << value[0];
                throw std::runtime_error(ss.str());
            }
        }
    }

    void fmt_parser_t::parse_width()
    {
        auto [_, value] = next();
        if (value.front() == '0')
            m_spec.leading_zeros = true;
        m_spec.width = std::stoll(std::string(value));
    }

    void fmt_parser_t::parse_precision()
    {
        if (!has_next())
            throw std::runtime_error("Missing token when parsing precision");
        auto [_, value] = next();
        m_spec.precision = std::stoll(std::string(value));
    }

    void fmt_parser_t::parse_type()
    {
        if (!has_next())
            throw std::runtime_error("Missing token when parsing type");
        auto [_, value] = next();
        if (value.size() != 1)
        {
            std::stringstream ss;
            ss << "Type contains more than one character, we are not sure how to interpret this value '" << value << "'";
            throw std::runtime_error(ss.str());
        }
        m_spec.uppercase = std::isupper(value.front());
        switch (value.front())
        {
        case 'b':
        case 'B':
            m_spec.type = fmt_type_t::BINARY;
            break;
        case 'c':
            m_spec.type = fmt_type_t::CHAR;
            break;
        case 'd':
            m_spec.type = fmt_type_t::DECIMAL;
            break;
        case 'o':
            m_spec.type = fmt_type_t::OCTAL;
            break;
        case 'x':
        case 'X':
            m_spec.type = fmt_type_t::HEX;
            break;
        case 'a':
        case 'A':
            m_spec.type = fmt_type_t::HEX_FLOAT;
            break;
        case 'e':
        case 'E':
            m_spec.type = fmt_type_t::EXPONENT;
            break;
        case 'f':
        case 'F':
            m_spec.type = fmt_type_t::FIXED_POINT;
            break;
        case 'g':
        case 'G':
            m_spec.type = fmt_type_t::GENERAL;
            break;
        default:
            std::stringstream ss;
            ss << "Invalid type " << value;
            throw std::runtime_error(ss.str());
        }
    }
}
