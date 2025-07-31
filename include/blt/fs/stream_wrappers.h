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

#ifndef BLT_FS_STREAM_WRAPPERS_H
#define BLT_FS_STREAM_WRAPPERS_H

#include <iosfwd>
#include <sstream>
#include <blt/fs/fwddecl.h>
#include <blt/meta/serialization.h>
#include <blt/meta/type_traits.h>

namespace blt::fs
{
	namespace detail
	{
		template <typename, typename = void>
		struct has_save : std::false_type
		{};


		template <typename T>
		struct has_save<T, std::void_t<decltype(std::declval<T>().save(std::declval<writer_t&>()))>> : std::true_type
		{};


		template <typename T>
		inline constexpr bool has_save_v = has_save<T>::value;


		template <typename, typename = void>
		struct has_load : std::false_type
		{};


		template <typename T>
		struct has_load<T, std::void_t<decltype(std::declval<T>().load(std::declval<reader_t&>()))>> : std::true_type
		{};


		template <typename T>
		inline constexpr bool has_load_v = has_load<T>::value;
	}

	/**
		* reader_t wrapper for fstream
		*/
	class fstream_reader_t final : public reader_t
	{
	public:
		explicit fstream_reader_t(std::istream& stream);

		i64 read(char* buffer, size_t bytes) override;

	private:
		std::istream* m_stream;
	};


	class fstream_writer_t final : public writer_t
	{
	public:
		explicit fstream_writer_t(std::ostream& stream);

		i64 write(const char* buffer, size_t bytes) override;

		void flush() override;

		i64 tell() override;

		void seek(i64 offset, seek_origin origin = seek_origin::seek_set) override;

		virtual ~fstream_writer_t() override // NOLINT
		{
			flush();
		}

	private:
		std::ostream* m_stream;
	};


	class reader_serializer_t
	{
	public:
		explicit reader_serializer_t(reader_t& reader): m_reader(&reader)
		{}

		std::string read_string()
		{
			std::string str;
			size_t      size;
			read_mem(size);
			str.resize(size);
			m_reader->read(str.data(), size);
			return str;
		}

		template <typename T>
		void read_mem(T& out)
		{
			if (!m_reader->read(reinterpret_cast<char*>(&out), sizeof(T)))
				throw std::runtime_error("Failed to read from reader");
		}

		template <typename T>
		void read_container(T& t)
		{
			using result_t = decltype(*t.data());
			size_t size;
			read_mem(size);
			t.resize(size);
			if constexpr (std::is_trivially_copyable_v<result_t>)
			{
				m_reader->read(reinterpret_cast<char*>(t.data()), sizeof(result_t) * size);
			} else
			{
				for (size_t i = 0; i < size; i++)
					read(t.data()[i]);
			}
		}

		template <typename T>
		void read_iterator(T& t)
		{
			using result_t = decltype(*t.begin());
			size_t size;
			read_mem(size);
			for (size_t i = 0; i < size; i++)
			{
				result_t v;
				read(v);
				if constexpr (std::is_trivially_copyable_v<result_t> || !std::is_move_constructible_v<result_t>)
				{
					meta::insert_helper_t{t, v};
				} else
				{
					meta::insert_helper_t{t, std::move(v)};
				}
			}
		}

		template <typename T>
		void read(T& t)
		{
			static_assert(!std::is_pointer_v<std::decay_t<T>>,
						  "We cannot serialize pointers due to a lack of size information. "
						  "If you intended to read the data behind the pointer please pass a blt::span.");
			if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
			{
				t = read_string();
			} else if constexpr (detail::has_load_v<T>)
			{
				t.load(*this);
			} else if constexpr (meta::is_tuple_like_v<T>)
			{
				std::apply([this](auto&... elems) {
							   (read(elems), ...);
						   },
						   t);
			} else if constexpr (meta::is_container_v<T>)
			{
				read_container(t);
			} else if constexpr (meta::is_iterable_v<T>)
			{
				read_iterator(t);
			} else if constexpr (std::is_trivially_copyable_v<T>)
			{
				read_mem(t);
			} else
			{
				throw std::runtime_error("Unable to determine how to read this value!");
			}
		}

		template <typename T>
		friend reader_serializer_t& operator>>(reader_serializer_t& reader, T& t)
		{
			reader.read(t);
			return reader;
		}

	private:
		reader_t* m_reader;
	};


	class writer_serializer_t
	{
	public:
		explicit writer_serializer_t(writer_t& writer): m_writer(&writer)
		{}

		void write_string(const std::string_view str)
		{
			write_mem(str.size());
			m_writer->write(str.data(), str.size());
		}

		template <typename T>
		void write_mem(const T& t)
		{
			static_assert(std::is_trivially_copyable_v<T>);
			m_writer->write(reinterpret_cast<const char*>(&t), sizeof(T));
		}

		template <typename T>
		void write_container(const T& t)
		{
			using result_t = decltype(*t.data());
			write_mem(t.size());
			if constexpr (std::is_trivially_copyable_v<result_t>)
			{
				m_writer->write(reinterpret_cast<const char*>(t.data()), sizeof(result_t) * t.size());
			} else
			{
				for (size_t i = 0; i < t.size(); i++)
					write(t.data()[i]);
			}
		}

		template <typename T>
		void write_iterator(const T& t)
		{
			auto begin = t.begin();
			auto end   = t.end();
			write_mem(static_cast<size_t>(std::distance(begin, end)));
			for (; begin != end; ++begin)
				write(*begin);
		}

		template <typename T>
		void write(const T& t)
		{
			static_assert(!std::is_pointer_v<std::decay_t<T>>,
						  "We cannot serialize pointers due to a lack of size information. "
						  "If you intended to write the data behind the pointer please pass a blt::span.");
			if constexpr (std::is_same_v<T, std::string> || std::is_same_v<T, std::string_view>)
			{
				write_string(t);
			} else if constexpr (detail::has_save_v<T>)
			{
				t.save(*this);
			} else if constexpr (meta::is_tuple_like_v<T>)
			{
				std::apply([this](const auto&... elems) {
							   (write(elems), ...);
						   },
						   t);
			} else if constexpr (meta::is_container_v<T>)
			{
				write_container(t);
			} else if constexpr (meta::is_iterable_v<T>)
			{
				write_iterator(t);
			} else if constexpr (std::is_trivially_copyable_v<T>)
			{
				write_mem(t);
			} else
			{
				throw std::runtime_error("Unable to determine how to write this value!");
			}
		}

		template <typename T>
		friend writer_serializer_t& operator<<(writer_serializer_t& writer, const T& t)
		{
			writer.write(t);
			return writer;
		}

	private:
		writer_t* m_writer;
	};


	class writer_string_wrapper_t
	{
	public:
		explicit writer_string_wrapper_t(writer_t& writer): m_writer(&writer)
		{}

		template <typename T>
		void write(const T& t)
		{
			std::stringstream ss;
			ss << t;
			const auto str = ss.str();
			m_writer->write(str.data(), str.size());
		}

		template <typename T>
		friend writer_string_wrapper_t& operator<<(writer_string_wrapper_t& writer, const T& t)
		{
			writer.write(t);
			return writer;
		}

	private:
		writer_t* m_writer;
	};
}

#endif //BLT_FS_STREAM_WRAPPERS_H
