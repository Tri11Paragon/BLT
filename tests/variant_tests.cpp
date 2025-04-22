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
#include <blt/std/variant.h>
#include <blt/logging/logging.h>
#include <blt/std/assert.h>

struct base_type
{
	[[nodiscard]] virtual int simple() const = 0;
	[[nodiscard]] virtual std::string to_string() const = 0;

	virtual ~base_type() = default;
};

struct mutate_type : base_type
{
	virtual void mutate(int i) = 0;
};

struct type1 final : base_type
{
	[[nodiscard]] int simple() const override // NOLINT
	{
		return 1;
	}

	[[nodiscard]] std::string to_string() const override // NOLINT
	{
		return "Type1";
	}
};


struct type2 final : base_type
{
	[[nodiscard]] int simple() const override // NOLINT
	{
		return 2;
	}

	[[nodiscard]] std::string to_string() const override // NOLINT
	{
		return "Type2";
	}
};


struct type3 final : base_type
{
	[[nodiscard]] int simple() const override // NOLINT
	{
		return 3;
	}

	[[nodiscard]] std::string to_string() const override // NOLINT
	{
		return "Type3";
	}
};

struct storing_type1 final : mutate_type
{
	explicit storing_type1(const int i): internal(i)
	{
	}

	[[nodiscard]] int simple() const override // NOLINT
	{
		return internal;
	}

	void mutate(const int i) override
	{
		internal = i;
	}

	[[nodiscard]] std::string to_string() const override // NOLINT
	{
		return "Storing Type: {" + std::to_string(internal) + "}";
	}

	int internal;
};

struct storing_type2 final : mutate_type
{
	explicit storing_type2(const float i): internal(i * 2.2534f)
	{
	}

	[[nodiscard]] int simple() const override // NOLINT
	{
		return static_cast<int>(internal);
	}

	void mutate(const int i) override
	{
		internal = static_cast<float>(i) * 2.2534f;
	}

	[[nodiscard]] std::string to_string() const override // NOLINT
	{
		return "Storing Type: {" + std::to_string(internal) + "}";
	}

	float internal;
};

int main()
{

	blt::variant_t<type1, type2, type3> v1{type1{}};
	blt::variant_t<type1, type2, type3> v2{type2{}};
	blt::variant_t<type1, type2, type3> v3{type3{}};

	BLT_TRACE("Variants to_string():");
	BLT_TRACE("V1: {}", v1.call_member(&base_type::to_string));
	BLT_TRACE("V2: {}", v2.call_member(&base_type::to_string));
	BLT_TRACE("V3: {}", v3.call_member(&base_type::to_string));
}