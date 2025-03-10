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

#ifndef BLT_LOGGING_STATUS_H
#define BLT_LOGGING_STATUS_H

#include <string>
#include <blt/logging/injector.h>
#include <blt/std/types.h>

namespace blt::logging
{

	class status_item_t
	{
	public:
		virtual ~status_item_t() = default;

		[[nodiscard]] virtual i32 lines_used() const
		{
			return 1;
		}

		virtual std::string print();
	};

	class status_bar_t final : public injector_t
	{
	public:
		explicit status_bar_t(i32 status_size);

		injector_output_t inject(const std::string& input) override;

		virtual ~status_bar_t() override;
	private:
		i32 m_status_size;
	};

}

#endif //BLT_LOGGING_STATUS_H
