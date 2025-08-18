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

#ifndef BLT_STD_STRING_H
#define BLT_STD_STRING_H
#include <cstring>
#include <bits/allocator.h>

template<typename CharT, typename Alloc = std::allocator<CharT>>
class string_t
{
public:
	string_t(Alloc alloc = {}): storage_{}, size_{0}, alloc_{alloc}
	{}

	string_t(const CharT* str, Alloc alloc = {}): size_{std::strlen(str)}, alloc_{alloc}
	{
		
	}

	CharT& operator[](const size_t index) {
		if (size_ <= 16)
			return storage_[index];
		return buffer_[index];
	}

	const CharT& operator[](const size_t index) const {
		if (size_ <= 16)
			return storage_[index];
		return buffer_[index];
	}
private:
	union
	{
		CharT* buffer_;
		CharT storage_[16];
	};
	size_t size_;
	Alloc alloc_;
};

#endif //BLT_STD_STRING_H
