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

#ifndef BLT_FS_BOUNDED_WRITER_H
#define BLT_FS_BOUNDED_WRITER_H

#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <blt/fs/fwddecl.h>
#include <blt/std/types.h>

namespace blt::fs
{
	inline auto basic_naming_function = [](const size_t invocation, const std::optional<std::string>& prefix) {
		return prefix.value_or("") + "-" + std::to_string(invocation) + ".txt";
	};

	// ReSharper disable once CppClassCanBeFinal
	class fwriter_t : public writer_t
	{
	public:
		explicit fwriter_t(const std::string& name, std::string mode = "ab"): m_mode(std::move(mode))
		{
			fwriter_t::newfile(name);
		}

		i64 write(const char* buffer, size_t bytes) override;

		virtual void newfile(const std::string& new_name);

		void flush() override;

	protected:
		std::string m_mode;
		FILE* m_file = nullptr;
	};

	// ReSharper disable once CppClassCanBeFinal
	class buffered_writer : public fwriter_t
	{
	public:
		explicit buffered_writer(const std::string& name, size_t buffer_size = 1024 * 128);

		i64 write(const char* buffer, size_t bytes) override;

		void flush() override;

	protected:
		size_t m_current_pos = 0;
		std::vector<char> m_buffer;
	};

	/**
	* 	Creates a bounded writer where after a specified number of bytes a new file will be opened and written to instead.
	*/
	// ReSharper disable once CppClassCanBeFinal
	class bounded_writer : public fwriter_t
	{
	public:
		explicit bounded_writer(fwriter_t& writer, std::optional<std::string> base_name,
								const std::function<std::string(size_t, std::optional<std::string>)>& naming_function = basic_naming_function,
								size_t max_size = 1024 * 1024 * 10);

		i64 write(const char* buffer, size_t bytes) override;

	private:
		fwriter_t* m_writer = nullptr;
		std::optional<std::string> m_base_name;
		size_t m_current_invocation = 0;
		size_t m_max_size = 0;
		size_t m_currently_written = 0;
		// inputs: current invocation, optional string provided to the constructor
		// returns: name of the file to write to
		std::function<std::string(size_t, const std::optional<std::string>&)> m_naming_function;
	};
}

#endif //BLT_FS_BOUNDED_WRITER_H
