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

#include <blt/fs/fwddecl.h>
#include <functional>
#include <string>
#include <blt/std/types.h>
#icnlude <optional>

namespace blt::fs
{
	inline auto basic_naming_function = [](size_t invocation, std::optional<std::string> prefix) {
  		return prefix.value_or("") + "-" + std::to_string(invocation) + ".txt";
  	};

	/**
	* 	Creates a bounded writer where after a specified number of bytes a new file will be opened and written to instead.
	*/
	class bounded_writer : public writer_t
	{
	public:
		explicit bounded_writer(std::optional<std::string> base_name, std::function<std::string(size_t, std::optional<std::string>)> naming_function = basic_naming_function, size_t max_size = 1024 * 1024 * 10);

	private:
          std::optional<std::string> m_base_name;
          size_t m_current_invocation = 0;
          // inputs: current invocation, optional string provided to the constructor
          // returns: name of the file to write to
          std::function<std::string(size_t, std::optional<std::string>)> m_naming_function;
	};
}

#endif //BLT_FS_BOUNDED_WRITER_H
