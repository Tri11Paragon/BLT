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
#include <blt/fs/file_writers.h>
#include <cstdio>
#include <utility>
#include <cstring>
#include <stdexcept>

namespace blt::fs
{
	i64 fwriter_t::write(const char* buffer, const size_t bytes)
	{
		return static_cast<i64>(std::fwrite(buffer, 1, bytes, m_file));
	}

	void fwriter_t::flush()
	{
		writer_t::flush();
		std::fflush(m_file);
	}

	void fwriter_t::newfile(const std::string& new_name)
	{
		if (m_file != nullptr)
			std::fclose(m_file);
		m_file = std::fopen(new_name.c_str(), m_mode.c_str());
		if (!m_file)
			throw std::runtime_error("Failed to open file for writing");
	}

	buffered_writer::buffered_writer(const std::string& name, const size_t buffer_size): fwriter_t{name}
	{
		m_buffer.resize(buffer_size);
	}

	i64 buffered_writer::write(const char* buffer, const size_t bytes)
	{
		if (bytes > m_buffer.size())
			return fwriter_t::write(buffer, bytes);
		if (bytes + m_current_pos > m_buffer.size())
			flush();
		std::memcpy(m_buffer.data() + m_current_pos, buffer, bytes);
		m_current_pos += bytes;
		return static_cast<i64>(bytes);
	}

	void buffered_writer::flush()
	{
		fwriter_t::flush();
		fwriter_t::write(m_buffer.data(), m_current_pos);
		m_current_pos = 0;
	}

	bounded_writer::bounded_writer(fwriter_t& writer, std::optional<std::string> base_name,
									const std::function<std::string(size_t, std::optional<std::string>)>& naming_function,
									const size_t max_size): fwriter_t{naming_function(0, base_name)}, m_writer(&writer),
															m_base_name(std::move(base_name)), m_max_size(max_size),
															m_naming_function(naming_function)
	{}

	i64 bounded_writer::write(const char* buffer, size_t bytes)
	{
		return fwriter_t::write(buffer, bytes);
	}
}
