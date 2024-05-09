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
#include <variant>

namespace blt
{
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
        TOKENIZER_FAILURE,
        NO_MATCHING_CURLY
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
            explicit template_token_consumer_t(const std::vector<template_token_data_t>& statement): template_consumer_base_t(statement)
            {}
            
            std::string_view from_last(std::string_view raw_string)
            {
                if (current_index == 0)
                    return "";
                auto token = storage[getPreviousIndex()];
                auto len = (&token.token.back() - &raw_string.front()) - last_read_index;
                auto str = std::string_view(&raw_string[last_read_index], len);
                last_read_index += len;
                return str;
            }
        
        private:
            size_t last_read_index = 0;
    };
    
    class template_parser_t
    {
        public:
            using estring = blt::expected<std::string, template_parser_failure_t>;
            template_parser_t(blt::hashmap_t<std::string, std::string>& substitutions, template_token_consumer_t& consumer):
                    substitutions(substitutions), consumer(consumer)
            {}
            
            estring parse()
            {
                consumer.advance(2);
                auto str = statement();
                if (!str)
                    return str;
                // should never occur
                if (consumer.hasNext() && consumer.next().type != template_token_t::CURLY_CLOSE)
                    return blt::unexpected(template_parser_failure_t::NO_MATCHING_CURLY);
                consumer.advance();
                return str;
            }
        
        private:
            estring statement()
            {
            
            }
            
            blt::hashmap_t<std::string, std::string>& substitutions;
            template_token_consumer_t& consumer;
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
            
            static blt::expected<std::vector<template_token_data_t>, template_tokenizer_failure_t> process_string(std::string_view str);
            
            blt::expected<std::string, template_parser_failure_t> evaluate(std::string_view str);
        
        private:
            blt::hashmap_t<std::string, std::string> substitutions;
    };
    
}

#endif //BLT_TEMPLATING_H
