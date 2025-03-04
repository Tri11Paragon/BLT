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

    void logger_t::setup_stream(const fmt_spec_t& spec)
    {
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
            m_stream << std::setprecision(static_cast<int>(std::cout.precision()));
        if (spec.uppercase)
            m_stream << std::uppercase;
        else
            m_stream << std::nouppercase;
    }

    void logger_t::process_strings()
    {
        auto spec_it = m_fmt_specs.begin();
        auto str_it = m_string_sections.begin();
        for (; spec_it != m_fmt_specs.end(); ++spec_it, ++str_it)
        {
            m_stream << *str_it;
            auto arg_pos = spec_it->arg_id;
            if (arg_pos == -1)
                arg_pos = static_cast<i64>(m_arg_pos++);

            setup_stream(*spec_it);
            m_arg_print_funcs[arg_pos](m_stream, *spec_it);
        }
        m_stream << *str_it;
    }

    void logger_t::handle_type(std::ostream& stream, const fmt_spec_t& spec)
    {
        switch (spec.type)
        {
        case fmt_type_t::DECIMAL:
            stream << std::dec;
            break;
        case fmt_type_t::OCTAL:
            if (spec.alternate_form)
                stream << "0";
            stream << std::oct;
            break;
        case fmt_type_t::HEX:
            if (spec.alternate_form)
                stream << (spec.uppercase ? "0X" : "0x");
            stream << std::hex;
            break;
        case fmt_type_t::HEX_FLOAT:
            if (spec.alternate_form)
                stream << (spec.uppercase ? "0X" : "0x");
            stream << std::hexfloat;
            break;
        case fmt_type_t::EXPONENT:
            stream << std::scientific;
            break;
        case fmt_type_t::FIXED_POINT:
            stream << std::fixed;
            break;
        default:
            break;
        }
    }

    void logger_t::exponential(std::ostream& stream)
    {
        stream << std::scientific;
    }

    void logger_t::fixed(std::ostream& stream)
    {
        stream << std::fixed;
    }

    void logger_t::compile(std::string fmt)
    {
        m_fmt = std::move(fmt);
        m_last_fmt_pos = 0;
        m_arg_pos = 0;
        m_stream.str("");
        m_stream.clear();
        m_string_sections.clear();
        m_fmt_specs.clear();
        ptrdiff_t last_pos = 0;
        while (auto pair = consume_to_next_fmt())
        {
            const auto [begin, end] = *pair;
            m_string_sections.emplace_back(m_fmt.data() + last_pos, begin - last_pos);
            if (end - begin > 1)
                m_fmt_specs.push_back(m_parser.parse(std::string_view{m_fmt.data() + static_cast<ptrdiff_t>(begin) + 1, end - begin - 1}));
            else
                m_fmt_specs.emplace_back();
            last_pos = static_cast<ptrdiff_t>(end) + 1;
        }
        m_string_sections.emplace_back(m_fmt.data() + last_pos, m_fmt.size() - last_pos);
        m_last_fmt_pos = 0;
    }

    std::optional<std::pair<size_t, size_t>> logger_t::consume_to_next_fmt()
    {
        const auto begin = m_fmt.find('{', m_last_fmt_pos);
        if (begin == std::string::npos)
            return {};
        const auto next_begin = m_fmt.find('{', begin + 1);
        const auto end = m_fmt.find('}', begin);
        if (end == std::string::npos || (next_begin != std::string::npos && next_begin < end))
        {
            std::stringstream ss;
            ss << "Invalid format string, missing closing '}' near " << m_fmt.substr(std::min(static_cast<i64>(begin) - 5, 0l));
            throw std::runtime_error(ss.str());
        }
        m_last_fmt_pos = end + 1;
        return std::pair{begin, end};
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
