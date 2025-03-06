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

#ifndef BLT_FILESYSTEM_H
#define BLT_FILESYSTEM_H

#include <iosfwd>
#include <blt/fs/fwddecl.h>

namespace blt::fs
{
	/**
	* reader_t wrapper for fstream
	*/
	class fstream_reader_t final : public reader_t
	{
	public:
		explicit fstream_reader_t(std::istream& stream);

		explicit fstream_reader_t(fstream_reader_t& copy) = delete;

		fstream_reader_t& operator=(const fstream_reader_t& copy) = delete;

		i64 read(char* buffer, size_t bytes) override;
	private:
		std::istream* m_stream;
	};

	class fstream_writer_t final : public writer_t
	{
	public:
		explicit fstream_writer_t(std::ostream& stream);

		explicit fstream_writer_t(fstream_writer_t& copy) = delete;

		fstream_writer_t& operator=(const fstream_writer_t& copy) = delete;

		i64 write(char* buffer, size_t bytes) override;

		void flush() override;

		virtual ~fstream_writer_t() override // NOLINT
		{
			flush();
		}

	private:
		std::ostream* m_stream;
	};
}

#endif //BLT_FILESYSTEM_H
