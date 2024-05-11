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

#ifndef BLT_TEMPLATING_H
#define BLT_TEMPLATING_H

#include <string>
#include <string_view>
#include <utility>
#include <vector>
#include <utility>
#include <blt/std/hashmap.h>
#include <blt/std/types.h>
#include <blt/std/expected.h>
#include <blt/std/logging.h>
#include <variant>

namespace blt
{
    
    class template_engine_t;
    
    template<typename Storage, typename Consumable>
    class template_consumer_base_t
    {
        public:
            explicit template_consumer_base_t(Storage storage): storage(std::move(storage))
            {}
            
            [[nodiscard]] Consumable next(size_t offset = 0) const
            {
                return storage[current_index + offset];
            }
            
            void advance(size_t offset = 1)
            {
                current_index += offset;
            }
            
            [[nodiscard]] bool hasNext(size_t offset = 1) const
            {
                return (current_index + (offset - 1)) < storage.size();
            }
            
            [[nodiscard]] Consumable consume()
            {
                Consumable c = next();
                advance();
                return c;
            }
            
            [[nodiscard]] size_t getCurrentIndex() const
            {
                return current_index;
            }
            
            [[nodiscard]] size_t getPreviousIndex() const
            {
                return current_index - 1;
            }
        
        protected:
            size_t current_index = 0;
            Storage storage;
    };
    
    enum class template_token_t
    {
        //STRING,         // A string of characters not $ { or }
        IDENT,          // $
        ADD,            // +
        CURLY_OPEN,     // {
        CURLY_CLOSE,    // }
        IF,             // IF
        ELSE,           // ELSE
        PAR_OPEN,       // (
        PAR_CLOSE,      // )
        OR,             // ||
        AND,            // &&
        XOR,            // ^
        NOT,            // !
        QUOTE,          // "
        SEMI,           // ;
        COMMA,          // ,
        PERIOD,         // .
        FUNCTION,       // ~
        STRING          // variable name
    };
    
    namespace detail
    {
        inline const blt::hashmap_t<std::string_view, template_token_t> identifiers = {
                {"IF",   template_token_t::IF},
                {"ELSE", template_token_t::ELSE}
        };
    }
    
    inline std::string template_token_to_string(template_token_t token)
    {
        switch (token)
        {
            case template_token_t::IDENT:
                return "[Template Identifier]";
            case template_token_t::CURLY_OPEN:
                return "[Curly Open]";
            case template_token_t::CURLY_CLOSE:
                return "[Curly Close]";
            case template_token_t::IF:
                return "[IF]";
            case template_token_t::ELSE:
                return "[ELSE]";
            case template_token_t::PAR_OPEN:
                return "[Par Open]";
            case template_token_t::PAR_CLOSE:
                return "[Par Close]";
            case template_token_t::OR:
                return "[OR]";
            case template_token_t::AND:
                return "[AND]";
            case template_token_t::XOR:
                return "[XOR]";
            case template_token_t::NOT:
                return "[NOT]";
            case template_token_t::QUOTE:
                return "[QUOTE]";
            case template_token_t::FUNCTION:
                return "[FUNC]";
            case template_token_t::STRING:
                return "[STR]";
            case template_token_t::SEMI:
                return "[SEMI]";
            case template_token_t::COMMA:
                return "[COMMA]";
            case template_token_t::PERIOD:
                return "[PERIOD]";
            case template_token_t::ADD:
                return "[ADD]";
        }
    }
    
    enum class template_tokenizer_failure_t
    {
        MISMATCHED_CURLY,
        MISMATCHED_PAREN,
        MISMATCHED_QUOTE,
    };
    
    enum class template_parser_failure_t
    {
        SUBSTITUTION_NOT_FOUND,
        TOKENIZER_FAILURE,
        NO_MATCHING_CURLY,
        MISSING_IDENT_BRACES,
        FUNCTION_EXPECTED_STRING,
        FUNCTION_NOT_FOUND,
        FUNCTION_DISCARD,
        STRING_EXPECTED_CONCAT,
        IF_EXPECTED_PAREN,
        BOOL_EXPECTED_PAREN,
        BOOL_TYPE_NOT_FOUND,
        UNKNOWN_STATEMENT_ERROR,
        UNKNOWN_ERROR
    };
    
    struct template_token_data_t
    {
        template_token_t type;
        size_t level;
        std::string_view token;
        size_t paren_level = 0;
        
        template_token_data_t(template_token_t type, size_t level, const std::string_view& token): type(type), level(level), token(token)
        {}
        
        template_token_data_t(template_token_t type, size_t level, const std::string_view& token, size_t parenLevel):
                type(type), level(level), token(token), paren_level(parenLevel)
        {}
    };
    
    class template_char_consumer_t : public template_consumer_base_t<std::string_view, char>
    {
        public:
            explicit template_char_consumer_t(std::string_view statement): template_consumer_base_t(statement)
            {}
            
            [[nodiscard]] std::string_view from(size_t begin, size_t end)
            {
                return std::string_view{&storage[begin], end - begin};
            }
    };
    
    class template_token_consumer_t : public template_consumer_base_t<std::vector<template_token_data_t>, template_token_data_t>
    {
        public:
            explicit template_token_consumer_t(const std::vector<template_token_data_t>& statement, std::string_view raw_string):
                    template_consumer_base_t(statement), raw_string(raw_string)
            {}
            
            void set_marker()
            {
                // when setting the marker, we need to go from the last closing brace
                auto index = storage.begin() + getCurrentIndex();
                while (index->type != template_token_t::CURLY_CLOSE)
                    index--;
                last_read_index = ((&index->token.front() + index->token.size()) - &raw_string[last_read_index]);
            }
            
            std::string_view from_last()
            {
                if (!hasNext())
                    return std::string_view(&raw_string[last_read_index], raw_string.size() - last_read_index);
                auto token = storage[getCurrentIndex()];
                auto len = ((&token.token.back()) - &raw_string[last_read_index]);
                auto str = std::string_view(&raw_string[last_read_index], len);
                return str;
            }
            
            void back()
            {
                current_index--;
            }
        
        private:
            std::string_view raw_string;
            size_t last_read_index = 0;
    };
    
    class template_engine_t
    {
        public:
            inline std::string& operator[](const std::string& key)
            {
                return substitutions[key];
            }
            
            inline std::string& operator[](std::string_view key)
            {
                return substitutions[key];
            }
            
            inline template_engine_t& set(std::string_view key, std::string_view replacement)
            {
                substitutions[key] = replacement;
                return *this;
            }
            
            inline bool contains(std::string_view token)
            {
                return substitutions.contains(token);
            }
            
            inline auto get(std::string_view token)
            {
                return evaluate(substitutions[token]);
            }
            
            static blt::expected<std::vector<template_token_data_t>, template_tokenizer_failure_t> process_string(std::string_view str);
            
            blt::expected<std::string, template_parser_failure_t> evaluate(std::string_view str);
        
        private:
            blt::hashmap_t<std::string, std::string> substitutions;
    };
    
    class template_parser_t
    {
        public:
            using estring = blt::expected<std::string, template_parser_failure_t>;
            using ebool = blt::expected<bool, template_parser_failure_t>;
            
            template_parser_t(template_engine_t& engine, template_token_consumer_t& consumer):
                    engine(engine), consumer(consumer)
            {}
            
            estring parse()
            {
                auto next = consumer.consume();
                if (next.type == template_token_t::IDENT && consumer.next().type == template_token_t::CURLY_OPEN)
                {
                    consumer.advance();
                    auto str = statement();
                    consumer.advance();
                    return str;
                }
                return blt::unexpected(template_parser_failure_t::MISSING_IDENT_BRACES);
            }
        
        private:
            estring statement()
            {
                auto next = consumer.consume();
                if (next.type == template_token_t::STRING || next.type == template_token_t::QUOTE)
                {
                    consumer.back();
                    return string();
                } else if (next.type == template_token_t::FUNCTION)
                {
                    return function();
                } else if (next.type == template_token_t::IDENT && consumer.hasNext() && consumer.next().type == template_token_t::CURLY_OPEN)
                {
                    consumer.advance();
                    auto stmt = statement();
                    // should never occur
                    if (consumer.hasNext() && consumer.next().type != template_token_t::CURLY_CLOSE)
                        return blt::unexpected(template_parser_failure_t::NO_MATCHING_CURLY);
                    consumer.advance();
                    return stmt;
                } else if (next.type == template_token_t::IF)
                {
                    return if_func();
                }
                return blt::unexpected(template_parser_failure_t::UNKNOWN_STATEMENT_ERROR);
            }
            
            estring function()
            {
                auto str = consumer.consume();
                if (str.type != template_token_t::STRING)
                    return blt::unexpected(template_parser_failure_t::FUNCTION_EXPECTED_STRING);
                if (str.token == "DISCARD")
                    return blt::unexpected(template_parser_failure_t::FUNCTION_DISCARD);
                return blt::unexpected(template_parser_failure_t::FUNCTION_NOT_FOUND);
            }
            
            estring if_func()
            {
                // IF(
                if (consumer.consume().type != template_token_t::PAR_OPEN)
                    return blt::unexpected(template_parser_failure_t::IF_EXPECTED_PAREN);
                // (statement)
                auto bool_eval = bool_statement();
                if (!bool_eval)
                    return blt::unexpected(bool_eval.error());
                BLT_TRACE(bool_eval.value());
                if (consumer.consume().type != template_token_t::PAR_CLOSE)
                    return blt::unexpected(template_parser_failure_t::IF_EXPECTED_PAREN);
                BLT_TRACE("Statement");
                auto true_statement = statement();
                estring false_statement = blt::unexpected(template_parser_failure_t::UNKNOWN_ERROR);
                BLT_TRACE(consumer.next().token);
                if (consumer.next().type == template_token_t::ELSE)
                {
                    consumer.advance();
                    false_statement = statement();
                }
                if (bool_eval.value())
                {
                    return true_statement;
                } else
                {
                    if (false_statement)
                        return false_statement;
                    return "";
                }
            }
            
            estring string()
            {
                auto next = consumer.consume();
                if (next.type == template_token_t::STRING)
                {
                    if (!engine.contains(next.token))
                        return blt::unexpected(template_parser_failure_t::SUBSTITUTION_NOT_FOUND);
                    if (consumer.next().type == template_token_t::SEMI || consumer.next().type == template_token_t::ELSE ||
                        consumer.next().type == template_token_t::CURLY_CLOSE || consumer.next().type == template_token_t::PAR_CLOSE)
                    {
                        consumer.advance();
                        return engine.get(next.token);
                    }
                    
                    if (consumer.next().type != template_token_t::ADD)
                        return blt::unexpected(template_parser_failure_t::STRING_EXPECTED_CONCAT);
                    consumer.advance();
                    auto str = string();
                    if (!str)
                        return str;
                    auto sub = engine.get(next.token);
                    if (!sub)
                        return sub;
                    return sub.value() + str.value();
                } else
                {
                    if (consumer.next().type == template_token_t::SEMI)
                    {
                        consumer.advance();
                        return std::string(next.token);
                    }
                    auto str = string();
                    if (str)
                        return std::string(next.token) + str.value();
                    else
                        return str;
                }
            }
            
            ebool bool_statement()
            {
                auto next = consumer.next();
                if (next.type == template_token_t::PAR_OPEN)
                {
                    consumer.advance();
                    auto b = bool_statement();
                    if (consumer.consume().type != template_token_t::PAR_CLOSE)
                        return blt::unexpected(template_parser_failure_t::BOOL_EXPECTED_PAREN);
                    consumer.advance();
                    return b;
                }
                return bool_expression();
            }
            
            ebool bool_value()
            {
                bool b1;
                auto next = consumer.next();
                bool invert = false;
                // prefixes
                if (next.type == template_token_t::NOT)
                {
                    invert = true;
                    consumer.advance();
                    next = consumer.next();
                }
                if (next.type == template_token_t::PAR_OPEN)
                {
                    auto b = bool_statement();
                    if (!b)
                        return b;
                    b1 = b.value();
                } else
                {
                    if (consumer.next().type == template_token_t::PAR_OPEN)
                    {
                        auto b = bool_statement();
                        if (!b)
                            return b;
                        b1 = b.value();
                    } else
                    {
                        auto b = statement();
                        if (!b)
                            return blt::unexpected(b.error());
                        BLT_DEBUG(b.value());
                        BLT_DEBUG(consumer.next().token);
                        b1 = !b.value().empty();
                    }
                }
                if (invert)
                    b1 = !b1;
                return b1;
            }
            
            ebool bool_expression()
            {
                // this whole thing is just bad. please redo. TODO
                std::vector<int> values;
                while (consumer.next().type != template_token_t::PAR_CLOSE)
                {
                    auto next = consumer.next();
                    auto bv = bool_value();
                    if (!bv)
                        return bv;
                    values.push_back(bv.value());
                    
                    if (values.size() == 2)
                    {
                        auto b1 = values[0];
                        auto b2 = values[1];
                        values.pop_back();
                        values.pop_back();
                        switch (next.type)
                        {
                            case template_token_t::AND:
                                values.push_back(b1 && b2);
                                break;
                            case template_token_t::OR:
                                values.push_back(b1 || b2);
                                break;
                            case template_token_t::XOR:
                                values.push_back(b1 ^ b2);
                                break;
                            default:
                                return blt::unexpected(template_parser_failure_t::BOOL_TYPE_NOT_FOUND);
                        }
                    }
                    next = consumer.next();
                    BLT_TRACE(next.token);
                    BLT_TRACE("Current State:");
                    for (auto b : values)
                        BLT_INFO(b);
                    if (next.type == template_token_t::PAR_CLOSE)
                        break;
                    consumer.advance();
//                    bv = bool_value();
//                    if (!bv)
//                        return bv;
//                    values.push_back(bv.value());
//
//                    switch (next.type)
//                    {
//                        case template_token_t::AND:
//                            ret =
//                        case template_token_t::OR:
//                            break;
//                        case template_token_t::XOR:
//                            break;
//                        default:
//                            return blt::unexpected(template_parser_failure_t::BOOL_TYPE_NOT_FOUND);
//                    }
                }
                BLT_INFO(consumer.next().token);
                consumer.advance();
                if (values.empty())
                    BLT_WARN("This is not possible!");
                return values[0];
//                if (next.type == template_token_t::NOT)
//                {
//                    auto b = bool_statement();
//                    if (b)
//                        return !b.value();
//                    else
//                        return b;
//                } else if (next.type == template_token_t::STRING)
//                {
//                    auto bool_val = next.token.empty();
//                    next = consumer.next();
//                    if (next.type == template_token_t::PAR_CLOSE)
//                        return bool_val;
//                    consumer.advance();
//                    if (next.type == template_token_t::AND)
//                    {
//                        auto other_val = bool_expression();
//                        if (!other_val)
//                            return other_val;
//                        return bool_val && other_val.value();
//                    } else if (next.type == template_token_t::OR)
//                    {
//                        auto other_val = bool_expression();
//                        if (!other_val)
//                            return other_val;
//                        return bool_val || other_val.value();
//                    } else if (next.type == template_token_t::XOR)
//                    {
//                        auto other_val = bool_expression();
//                        if (!other_val)
//                            return other_val;
//                        return bool_val ^ other_val.value();
//                    }
//                }
//                return unexpected(template_parser_failure_t::UNKNOWN_ERROR);
            }
            
            template_engine_t& engine;
            template_token_consumer_t& consumer;
    };
    
}

#endif //BLT_TEMPLATING_H
