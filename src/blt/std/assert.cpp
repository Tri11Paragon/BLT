/*
 * Created by Brett on 23/08/23.
 * Licensed under GNU General Public License V3.0
 * See LICENSE file for license detail
 */
#include <cstring>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <blt/logging/logging.h>
#include <blt/std/assert.h>
#include <blt/std/string.h>
#include <blt/std/utility.h>

// TODO
#undef BLT_STACK_TRACE

struct abort_exception final : public std::exception
{
public:
	explicit abort_exception(const char* what)
	{
		auto len = std::strlen(what) + 1;
		error = static_cast<char*>(std::malloc(len));
		std::memcpy(static_cast<char*>(error), what, len);
	}

	abort_exception(const abort_exception& copy) = delete;
	abort_exception& operator=(const abort_exception& copy) = delete;

	[[nodiscard]] const char* what() const noexcept override
	{
		if (error == nullptr)
			return "Abort called";
		return error;
	}

	~abort_exception() override
	{
		std::free(static_cast<void*>(error));
	}

private:
	char* error{nullptr};
};

#if defined(__GNUC__) && !defined(__EMSCRIPTEN__) && !defined(WIN32)
#define IS_GNU_BACKTRACE
#endif

#ifdef IS_GNU_BACKTRACE

#include <cstdlib>
#include <execinfo.h>

#endif

#ifdef IS_GNU_BACKTRACE
#define BLT_STACK_TRACE(number) void* ptrs[number]; \
            int size = backtrace(ptrs, number);         \
            char** messages = backtrace_symbols(ptrs, size);

#define BLT_FREE_STACK_TRACE() free(messages);

#else
    #define BLT_STACK_TRACE(number) void();
    #define BLT_FREE_STACK_TRACE() void();
#endif

namespace blt
{
	#ifdef IS_GNU_BACKTRACE

	static inline std::string _macro_filename(const std::string& path)
	{
		auto paths = blt::string::split(path, "/");
		auto final = paths[paths.size() - 1];
		if (final == "/")
			return paths[paths.size() - 2];
		return final;
	}

	#endif

	void b_throw(const char* what, const char* path, int line)
	{
		#ifdef IS_GNU_BACKTRACE
		BLT_STACK_TRACE(50);

		BLT_ERROR("An exception '{}' has occurred in file '{}:{:d}'", what, path, line);
		BLT_ERROR("Stack Trace:");
		detail::print_stack_trace(messages, size, path, line);

		BLT_FREE_STACK_TRACE();
		#else
        (void) what;
        (void) path;
        (void) line;
		#endif
	}

	void b_assert_failed(const char* expression, const char* msg, const char* path, int line)
	{
		#ifdef IS_GNU_BACKTRACE
		BLT_STACK_TRACE(50);

		BLT_ERROR("The assertion '{}' has failed in file '{}:{:d}'", expression, path, line);
		if (msg != nullptr)
			BLT_ERROR(msg);
		BLT_ERROR("Stack Trace:");

		detail::print_stack_trace(messages, size, path, line);

		BLT_FREE_STACK_TRACE();
		#else
        (void) expression;
        (void) msg;
        (void) path;
        (void) line;
		#endif
		if (msg != nullptr)
			throw abort_exception(msg);
		throw abort_exception(expression);
	}

	namespace detail
	{
		void print_stack_trace(char** messages, int size, const char* path, int line)
		{
			if (messages == nullptr)
				return;
			#ifdef IS_GNU_BACKTRACE
			for (int i = 1; i < size; i++)
			{
				int tabs = i - 1;
				std::string buffer;
				for (int j = 0; j < tabs; j++)
					buffer += '\t';
				if (i != 1)
					buffer += "⮡";

				std::string message(messages[i]);

				auto f = message.find('(');

				auto mes = message.substr(f + 1, message.size());
				std::string loc;

				buffer += message.substr(0, f);
				if (i == 1)
				{
					loc = '\'';
					loc += _macro_filename(path);
					loc += ':';
					loc += std::to_string(line);
					loc += '\'';
				} else
					loc = demangle(mes.substr(0, mes.find('+')));

				if (!loc.empty())
					buffer += " in ";
				buffer += loc;

				BLT_ERROR(buffer);
			}
			#else
            (void) size;
            (void) path;
            (void) line;
			#endif
		}

		void print_stack_trace(const char* path, int line)
		{
			#ifdef IS_GNU_BACKTRACE
			BLT_STACK_TRACE(50);
			for (int i = 1; i < size; i++)
			{
				int tabs = i - 1;
				std::string buffer;
				for (int j = 0; j < tabs; j++)
					buffer += '\t';
				if (i != 1)
					buffer += "⮡";

				std::string message(messages[i]);

				auto f = message.find('(');

				auto mes = message.substr(f + 1, message.size());
				std::string loc;

				buffer += message.substr(0, f);
				if (i == 1)
				{
					loc = '\'';
					loc += _macro_filename(path);
					loc += ':';
					loc += std::to_string(line);
					loc += '\'';
				} else
					loc = demangle(mes.substr(0, mes.find('+')));

				if (!loc.empty())
					buffer += " in ";
				buffer += loc;

				BLT_ERROR(buffer);
			}
			BLT_FREE_STACK_TRACE();
			#else
            (void) path;
            (void) line;
			#endif
		}
	}

	void b_abort(const char* what, const char* path, int line)
	{
		#ifdef IS_GNU_BACKTRACE
		BLT_STACK_TRACE(50);
		#endif
		BLT_FATAL("----\\{BLT ABORT}----");
		BLT_FATAL("\tWhat: {}", what);
		BLT_FATAL("\tCalled from {}:{:d}", path, line);
		#ifdef IS_GNU_BACKTRACE
		detail::print_stack_trace(messages, size, path, line);

		BLT_FREE_STACK_TRACE();
		#endif
		throw abort_exception(what);
	}
}
