/*
 *  <Short Description>
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
#include <blt/parse/templating.h>
#include <blt/std/string.h>
#include <cctype>
#include "blt/std/logging.h"

namespace blt
{
    
    bool isNonStringNext(char c)
    {
        switch (c)
        {
            case '$':
            case '{':
            case '}':
            case '(':
            case ')':
            case '"':
            case '^':
            case '!':
            case '&':
            case ';':
            case ',':
            case '.':
            case '|':
            case '+':
                return true;
            default:
                return false;
        }
    }
    
    blt::expected<std::vector<template_token_data_t>, template_tokenizer_failure_t> template_engine_t::process_string(std::string_view str)
    {
        std::vector<template_token_data_t> tokens;
        
        template_char_consumer_t consumer(str);
        
        i64 start = -1;
        size_t paren_level = 0;
        size_t level = 0;
        bool open = false;
        while (consumer.hasNext())
        {
            i64 current_start = static_cast<i64>(consumer.getCurrentIndex());
            char c = consumer.consume();
            switch (c)
            {
                case '$':
                    tokens.emplace_back(template_token_t::IDENT, level, consumer.from(current_start, current_start + 1));
                    if (consumer.next() == '{')
                    {
                        paren_level = 0;
                        open = true;
                    }
                    continue;
                case '{':
                    tokens.emplace_back(template_token_t::CURLY_OPEN, level, consumer.from(current_start, current_start + 1));
                    if (open)
                        level++;
                    continue;
                case '}':
                    tokens.emplace_back(template_token_t::CURLY_CLOSE, level, consumer.from(current_start, current_start + 1));
                    if (open)
                        level--;
                    if (level == 0)
                    {
                        open = false;
                        if (paren_level != 0)
                            return blt::unexpected(template_tokenizer_failure_t::MISMATCHED_PAREN);
                    }
                    continue;
                case '(':
                    tokens.emplace_back(template_token_t::PAR_OPEN, level, consumer.from(current_start, current_start + 1), paren_level);
                    paren_level++;
                    break;
                case ')':
                    tokens.emplace_back(template_token_t::PAR_CLOSE, level, consumer.from(current_start, current_start + 1), paren_level);
                    paren_level--;
                    break;
                case '"':
                    tokens.emplace_back(template_token_t::QUOTE, level, consumer.from(current_start, current_start + 1));
                    // if we just encountered a quote, we need to consume characters until we find its matching quote
                    // only if we are currently inside a template though...
                    if (open)
                    {
                        current_start = static_cast<i64>(consumer.getCurrentIndex());
                        while (consumer.hasNext() && consumer.next() != '"')
                            consumer.advance();
                        if (!consumer.hasNext())
                            return blt::unexpected(template_tokenizer_failure_t::MISMATCHED_QUOTE);
                        tokens.emplace_back(template_token_t::STRING, level, consumer.from(current_start, consumer.getCurrentIndex()));
                        consumer.advance();
                        current_start = static_cast<i64>(consumer.getCurrentIndex());
                        tokens.emplace_back(template_token_t::QUOTE, level, consumer.from(current_start, current_start + 1));
                    }
                    break;
                case '^':
                    tokens.emplace_back(template_token_t::XOR, level, consumer.from(current_start, current_start + 1));
                    break;
                case '!':
                    tokens.emplace_back(template_token_t::NOT, level, consumer.from(current_start, current_start + 1));
                    break;
                case ';':
                    tokens.emplace_back(template_token_t::SEMI, level, consumer.from(current_start, current_start + 1));
                    break;
                case ',':
                    tokens.emplace_back(template_token_t::COMMA, level, consumer.from(current_start, current_start + 1));
                    break;
                case '+':
                    tokens.emplace_back(template_token_t::ADD, level, consumer.from(current_start, current_start + 1));
                    break;
                case '.':
                    tokens.emplace_back(template_token_t::PERIOD, level, consumer.from(current_start, current_start + 1));
                    break;
                case '~':
                    tokens.emplace_back(template_token_t::FUNCTION, level, consumer.from(current_start, current_start + 1));
                    break;
                case '|':
                    if (consumer.hasNext() && consumer.next() == '|')
                    {
                        consumer.advance();
                        tokens.emplace_back(template_token_t::OR, level, consumer.from(current_start, current_start + 2));
                        continue;
                    }
                    start = current_start;
                    break;
                case '&':
                    if (consumer.hasNext() && consumer.next() == '&')
                    {
                        consumer.advance();
                        tokens.emplace_back(template_token_t::AND, level, consumer.from(current_start, current_start + 2));
                        continue;
                    }
                    start = current_start;
                    break;
                default:
                    // do not add whitespace to anything
                    if (std::isspace(c))
                        break;
                    if (start == -1)
                        start = current_start;
                    if (consumer.hasNext() && (isNonStringNext(consumer.next()) || std::isspace(consumer.next())))
                    {
                        tokens.emplace_back(template_token_t::STRING, level, consumer.from(start, consumer.getCurrentIndex()));
                        start = -1;
                    }
                    break;
            }
        }
        
        if (start != -1)
            tokens.emplace_back(template_token_t::STRING, level, consumer.from(start, consumer.getCurrentIndex()));
        
        for (auto& token : tokens)
        {
            if (token.type == template_token_t::STRING && detail::identifiers.contains(token.token))
                token.type = detail::identifiers.at(token.token);
        }
        
        if (level != 0)
            return unexpected(template_tokenizer_failure_t::MISMATCHED_CURLY);
        
        return tokens;
    }
    
    blt::expected<std::string, template_parser_failure_t> template_engine_t::evaluate(std::string_view str)
    {
        auto tokens = process_string(str);
        
        if (!tokens)
        {
            switch (tokens.error())
            {
                case template_tokenizer_failure_t::MISMATCHED_CURLY:
                    BLT_ERROR("Mismatched curly braces");
                    break;
                case template_tokenizer_failure_t::MISMATCHED_PAREN:
                    BLT_ERROR("Mismatched parentheses");
                    break;
                case template_tokenizer_failure_t::MISMATCHED_QUOTE:
                    BLT_ERROR("Mismatched quotes");
                    break;
            }
            return blt::unexpected(template_parser_failure_t::TOKENIZER_FAILURE);
        }
        
        std::string return_str;
        return_str.reserve(str.size());
        
        template_token_consumer_t consumer{tokens.value()};
        
        template_parser_t parser(substitutions, consumer);
        
        while (consumer.hasNext())
        {
            BLT_TRACE("Running with next %d", static_cast<int>(consumer.next().type));
            while (consumer.hasNext(2))
            {
                if (consumer.next().type == template_token_t::IDENT && consumer.next(1).type == template_token_t::CURLY_OPEN)
                {
                    BLT_TRACE("From Last: %s", std::string(consumer.from_last(str)).c_str());
                    return_str += consumer.from_last(str);
                    break;
                }
                consumer.advance();
            }
            if (auto result = parser.parse())
                return_str += result.value();
            else
            {
                if (result.error() == template_parser_failure_t::FUNCTION_DISCARD)
                    continue;
                return result;
            }
        }
        
        BLT_TRACE(return_str);
        
        return return_str;
    }
}